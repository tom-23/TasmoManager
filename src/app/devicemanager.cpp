#include "devicemanager.h"

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    deviceList = new QList<Device*>;
    mqttClient = new QMQTT::Client();
    connectionStatus = Disconnected;
}

void DeviceManager::connect(MQTTServerInfo *serverInfo) {
    mqttServerInfo = serverInfo;
    mqttClient = new QMQTT::Client();
    if (!mqttServerInfo->ipAddress.isNull()) {
        mqttClient->setHost(mqttServerInfo->ipAddress);
    } else {
        mqttClient->setHostName(mqttServerInfo->host);
    }
    mqttClient->setPort(mqttServerInfo->port);
    mqttClient->setUsername(mqttServerInfo->username);
    mqttClient->setPassword(mqttServerInfo->password);
    connectionStatus = Connecting;
    mqttClient->connectToHost();

    QObject::connect(mqttClient, &QMQTT::Client::received, this, &DeviceManager::on_mqttMessage);
    QObject::connect(mqttClient, &QMQTT::Client::connected, this, [=] (){
        qDebug() << "[MQTT Status] Connected to MQTT Broker";
        connectionStatus = Connected;
        emit mqtt_onConnect();
    });

    QObject::connect(mqttClient, &QMQTT::Client::error, this, [=] (QMQTT::ClientError clientError) {
        connectionStatus = Disconnected;
        emit mqtt_onError(clientError);
        qDebug() << "[MQTT Status] Got a connection error!";
        qDebug() << clientError;
    });
}

void DeviceManager::disconnect() {
    qDebug() << "[MQTT Status] Disconnected from MQTT Broker";
    mqttClient->disconnectFromHost();
    connectionStatus = Disconnected;
    deviceList->clear();
}

void DeviceManager::on_mqttMessage(QMQTT::Message message) {
    qDebug() << "[MQTT] Got message " << message.topic();
    QString topic = message.topic();

    // Handle Discovery
    if (topic.startsWith(discoveryTopic)) {
        QStringList subtopics = (topic.replace(discoveryTopic + "/", "")).split("/");
        if (subtopics[1] == "config") {

            QJsonDocument jsonDoc = QJsonDocument::fromJson(message.payload());
            QString macAddress = jsonDoc.object().value("mac").toString();
            QString ipAddress = jsonDoc.object().value("ip").toString();
            QString fullTopicWithWildC = jsonDoc.object().value("ft").toString();
            QString topic = jsonDoc.object().value("t").toString();

            if (!fullTopicWithWildC.endsWith("/")) { // Ensure the fulltopic has a trailing slash.
                fullTopicWithWildC = fullTopicWithWildC + "/";
            }

            Device *device = getDeviceByMAC(macAddress);
            if (device == nullptr) {
                device = new Device(this);
                device->deviceManager = this;
                device->deviceInfo.name = "Tasmota Device";
                device->deviceInfo.ipAddress = QHostAddress(ipAddress);
                device->deviceInfo.macAddress = macAddress;
                deviceList->append(device);
                emit device_Discovered(device->deviceInfo);
                qDebug() << "[Device Manager] Adding device:" << macAddress;
                if (macAddress == setupNewDeviceMac) {
                    stepIndex = stepIndex + 1;
                    setupNewDeviceMac = "";
                    emit setupNewDevices_Progress(stepIndex, 3, "Device has connected to mqtt...");
                }
            }
            QString fullTopic = fullTopicWithWildC.replace("%topic%", topic);
            device->getDeviceInfo(fullTopic, getDeviceDiscoveryInfoDelay);
            getDeviceDiscoveryInfoDelay = getDeviceDiscoveryInfoDelay + refreshDelay;
        }
    }

    for (int i = 0; i < deviceList->size(); i++) {
        if (topic.startsWith(deviceList->at(i)->statTopic) || topic.startsWith(deviceList->at(i)->teleTopic)) {
            deviceList->at(i)->on_Message(message);
        }
    }
}

Device* DeviceManager::getDeviceByMAC(QString mac) {
    mac = mac.replace(":", "");
    for (int i = 0; i < deviceList->size(); i++) {
        if (deviceList->at(i)->deviceInfo.macAddress.replace(":", "") == mac) {
            return deviceList->at(i);
        }
    }
    qDebug() << "[Device Manager] Device doesn't exist";
    return nullptr;
}

void DeviceManager::refreshDevices() {
    getDeviceInfoDelay = 0;
    for (int i = 0; i < deviceList->size(); i++) {
        Device *device = deviceList->at(i);
        device->getDeviceInfo("", getDeviceInfoDelay);
        getDeviceInfoDelay = getDeviceInfoDelay + refreshDelay;
    }
}

void DeviceManager::subscribeToDiscovery() {
    if (mqttClient->connectionState() == QMQTT::STATE_CONNECTED) {
        mqttClient->subscribe(discoveryTopic + "/#");
        qDebug() << "[MQTT] Subscribed to discovery topic";
    }

}

void DeviceManager::scanNewDevices() {
    int maxThreads = 99;

    addressesScanned = 0;
    addressesToScan.clear();

    QList<QNetworkAddressEntry> interfaces = getActiveAddresses();

    for (int i = 0; i < interfaces.size(); i++) {
        QList<QHostAddress> addresses = getIPAddresses(interfaces[i]);
        for (int x = 0; x < addresses.size(); x++) {
            addressesToScan.push_back(addresses[x]);
        }
    }

    for (int i = 0; i < maxThreads; i++) {
        NetworkScanThread *scanThread = new NetworkScanThread();
        scanThread->setIP(addressesToScan[addressesScanned]);

        QObject::connect(scanThread, &NetworkScanThread::resultReady, this, [=](DeviceInfo deviceInfo) {
            if (deviceInfo.macAddress != "") {
                if (getDeviceByMAC(deviceInfo.macAddress) == nullptr) {
                    qDebug() << "[Scan] A new device has been found!";
                    emit scanNewDevices_NewDeviceFound(deviceInfo);
                }
            }
            if (addressesScanned < addressesToScan.size()) {
                scanThread->setIP(addressesToScan[addressesScanned]);
                scanThread->go();
                addressesScanned = addressesScanned + 1;
                emit scanNewDevices_Progress(addressesScanned, addressesToScan.size());

            } else {
                delete scanThread;
            }
        });
        scanThread->go();
        addressesScanned = addressesScanned + 1;
        emit scanNewDevices_Progress(addressesScanned, addressesToScan.size());
    }
}

void DeviceManager::setupNewDevice(DeviceInfo baseInfo, DeviceInfo newInfo) {
    networkManager = new QNetworkAccessManager(this);
    stepIndex = 0;
    QNetworkRequest request;

    QUrl url("http://" + baseInfo.ipAddress.toString() + "/cm");
    QUrlQuery urlQuery;

    QString query = "Backlog MqttHost " + newInfo.mqttServer.ipAddress.toString() +
            "; MqttUser " + newInfo.mqttServer.username +
            "; MqttPassword " + newInfo.mqttServer.password +
            "; MqttPort " + QString::number(newInfo.mqttServer.port) +
            "; MqttTopic " + newInfo.mqttTopic +
            "/; MqttFullTopic " + newInfo.mqttFullTopic +
            "; SetOption19 0";
    qDebug() << newInfo.mqttFullTopic;
    if (newInfo.name != "") {
        query.append("; DeviceName " + newInfo.name);
    }

    if (newInfo.friendlyName != "") {
        query.append("; FriendlyName1 " + newInfo.friendlyName);
    }

    if (newInfo.moduleTemplate != "") {
        query.append("; Template " + newInfo.moduleTemplate +"; Module 0");
    }

    urlQuery.addQueryItem("cmnd", QUrl(query).toPercentEncoding(query));
    url.setQuery(urlQuery);
    request.setUrl(url);

    qDebug() << "[Device Setup] URL:" << request.url().toEncoded();
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, [=] (QNetworkReply *reply) {

        QString answer = reply->readAll();
        qDebug() << "[Device Setup] Device Returned: " << answer;
        stepIndex = stepIndex + 1;
        setupNewDeviceMac = baseInfo.macAddress;
        setupNewDeviceMac.replace(":", "");
        emit setupNewDevices_Progress(stepIndex, 3, "Device settings saved! Waiting for device to reconnect...");

    });
    networkManager->get(request);
    stepIndex = stepIndex + 1;
    emit setupNewDevices_Progress(stepIndex, 3, "Saving device settings...");
}

QList<QHostAddress> DeviceManager::getIPAddresses(QNetworkAddressEntry networkAddress) {

    quint32 ipAddress = networkAddress.ip().toIPv4Address();
    quint32 netmask = networkAddress.netmask().toIPv4Address();

    quint32 first = ipAddress & netmask;
    quint32 last = ~(netmask) + first;


    QList<QHostAddress> ipAddressList;

    for (quint32 i = first; i < last; i++) {
        QHostAddress ip(i);
        ipAddressList.append(ip);
    }

    ipAddressList.pop_back();

    return ipAddressList;
}

QList<QNetworkAddressEntry> DeviceManager::getActiveAddresses() {
    QList<QString> possibleMatches;
    QList<QNetworkAddressEntry> possibleInterfaceMatches;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    if ( !ifaces.isEmpty() )
    {
      for(int i=0; i < ifaces.size(); i++)

      {
        unsigned int flags = ifaces[i].flags();
        bool isLoopback = (bool)(flags & QNetworkInterface::IsLoopBack);
        bool isP2P = (bool)(flags & QNetworkInterface::IsPointToPoint);
        bool isRunning = (bool)(flags & QNetworkInterface::IsRunning);

        // If this interface isn't running, we don't care about it
        if ( !isRunning ) continue;
        // We only want valid interfaces that aren't loopback/virtual and not point to point
        if ( !ifaces[i].isValid() || isLoopback || isP2P ) continue;
        QList<QNetworkAddressEntry> addresses = ifaces[i].addressEntries();
        for(int a=0; a < addresses.size(); a++)
        {
          // Ignore local host
          if ( addresses[a].ip() == QHostAddress::LocalHost ) continue;

          // Ignore non-ipv4 addresses
          if ( !addresses[a].ip().toIPv4Address() ) continue;

          QString ip = addresses[a].ip().toString();
          if ( ip.isEmpty() ) continue;
          bool foundMatch = false;
          for (int j=0; j < possibleMatches.size(); j++) if ( ip == possibleMatches[j] ) { foundMatch = true; break; }
          if ( !foundMatch ) {
              possibleMatches.push_back( ip ); qDebug() << "possible address: " << ifaces[i].humanReadableName() << "->" << ip;
              possibleInterfaceMatches.push_back(addresses[a]);
          }
        }
      }
    }
    return possibleInterfaceMatches;
}
