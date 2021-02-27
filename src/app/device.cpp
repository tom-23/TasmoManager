#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{

}

void Device::getDeviceInfo(QString fullTopic, int delay) {
    if (fullTopic != "" && fullTopic != deviceInfo.mqttFullTopic) {
        deviceInfo.mqttFullTopic = fullTopic;
        cmndTopic = fullTopic;
        cmndTopic.replace("%prefix%", "cmnd");
        statTopic = fullTopic;
        statTopic.replace("%prefix%", "stat");
        teleTopic = fullTopic;
        teleTopic.replace("%prefix%", "tele");
        qDebug() << "[Device] Subscribing to STAT topic...";
        qDebug() << "[Device] TOPIC:" << statTopic + "#";
        qDebug() << "[Device] Subscribing to TELE topic...";
        qDebug() << "[Device] TOPIC:" << statTopic + "#";
        deviceManager->mqttClient->subscribe((statTopic + "#").toUtf8());
        deviceManager->mqttClient->subscribe((teleTopic + "#").toUtf8());
    }
    qDebug() << "[Device] Grabbing status...";
    qDebug() << "[Device] TOPIC:" << cmndTopic + "Status";
    QMQTT::Message message;

    message.setTopic((cmndTopic + "Status").toUtf8());
    message.setPayload("0");



    QTimer *delayTimer = new QTimer(this);
    delayTimer->setSingleShot(true);
    delayTimer->start(delay);
    connect(delayTimer, &QTimer::timeout, this, [=] () {
        deviceManager->mqttClient->publish(message);
        getState();
        delete delayTimer;
    });

}

void Device::setDeviceNames(QString name, QString friendlyName) {
    deviceInfo.name = name;
    deviceInfo.friendlyName = friendlyName;
    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");
    message.setPayload(("DeviceName " + name + "; FriendlyName1 " + friendlyName).toUtf8());
    deviceManager->mqttClient->publish(message);
    emit deviceManager->device_InfoUpdate(deviceInfo);
}


void Device::openWebUI() {
    QDesktopServices::openUrl(QUrl("http://" + deviceInfo.ipAddress.toString() + "/"));
}

void Device::getState() {
    QMQTT::Message message;
    message.setTopic((cmndTopic + "STATE").toUtf8());

    // Reset all capabilites (things might change if you changed the configuration)

    deviceInfo.capabilities.color = false;
    deviceInfo.capabilities.dimmer = false;
    deviceInfo.capabilities.colorTemp = false;

    for (int i = 0; i < deviceInfo.capabilities.power.size(); i++) {
        deviceInfo.capabilities.power[i] = false;
    }

    for (int i = 0; i < deviceInfo.capabilities.channels.size(); i++) {
        deviceInfo.capabilities.channels[i] = false;
    }

    deviceManager->mqttClient->publish(message);
}

void Device::restart() {
    QMQTT::Message message;
    message.setTopic((cmndTopic + "Restart").toUtf8());
    message.setPayload("1");
    deviceManager->mqttClient->publish(message);
}

void Device::setLogLevel(int level) {
    QMQTT::Message message;
    message.setTopic((cmndTopic + "mqttlog").toUtf8());
    message.setPayload(QString::number(level).toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::setPower(int powerID, bool on) {
    if (powerID <= 4 && powerID >= 1){
        deviceInfo.power[powerID - 1] = on;
        QMQTT::Message message;
        message.setTopic((cmndTopic + "POWER" + QString::number(powerID)).toUtf8());
        message.setPayload(boolToOnOff(on).toUtf8());
        deviceManager->mqttClient->publish(message);
    } else {
        qDebug() << "[Device] Power ID is out of range (1 - 4)";
    }
}

void Device::setChannel(int channelID, int value) {
    if (channelID <= 5 && channelID >= 1){
        deviceInfo.channels[channelID - 1] = value;
        QMQTT::Message message;
        message.setTopic((cmndTopic + "Channel" + QString::number(channelID)).toUtf8());
        message.setPayload(QString::number(map(value, 0, 255, 0, 100)).toUtf8());
        deviceManager->mqttClient->publish(message);
    } else {
        qDebug() << "[Device] Power ID is out of range (1 - 4)";
    }
}

void Device::setDimmer(int value) {
    deviceInfo.dimmer = value;
    QMQTT::Message message;
    message.setTopic((cmndTopic + "Dimmer").toUtf8());
    message.setPayload(QString::number(value).toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::setColor(QColor color) {
    deviceInfo.color = color;
    QMQTT::Message message;
    message.setTopic((cmndTopic + "Color").toUtf8());
    message.setPayload(color.name().toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::setColorTemp(int colorTemp) {
    deviceInfo.colorTemp = colorTemp;
    QMQTT::Message message;
    message.setTopic((cmndTopic + "CT").toUtf8());
    message.setPayload(QString::number(colorTemp).toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::sendCommand(QString command) {
    QMQTT::Message message;
    message.setTopic((cmndTopic + command.section(" ", 0, 0)).toUtf8());
    QString payload = command;
    payload.remove(0, command.section(" ", 0, 0).size() + 1);
    message.setPayload(payload.toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::on_Message(QMQTT::Message message) {

    QStringList topicSplit = message.topic().split("/");
    QString endTopic = topicSplit.at(topicSplit.count() - 1);

    QJsonDocument jsonDoc;

    if (endTopic == "LWT") {
        qDebug() << "[Device] Updating status...";
        if (message.payload() == "Online") {
            deviceInfo.status = Online;
        } else if (message.payload() == "Offline") {
            deviceInfo.status = Offline;
        }
        emit deviceManager->device_InfoUpdate(deviceInfo);
    }

    if (endTopic == "STATUS") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject statusObject = jsonDoc.object().value("Status").toObject();
        deviceInfo.name = statusObject.value("DeviceName").toString();
        deviceInfo.friendlyName = statusObject.value("FriendlyName").toArray()[0].toString();

        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS1") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject status1Object = jsonDoc.object().value("StatusPRM").toObject();
        deviceInfo.OTAUrl = QUrl(status1Object.value("OtaUrl").toString());
        deviceInfo.bootCount = status1Object.value("BootCount").toInt();

        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS2") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject status2Object = jsonDoc.object().value("StatusFWR").toObject();
        deviceInfo.firmwareVersion = status2Object.value("Version").toString();
        deviceInfo.buildDateAndTime = status2Object.value("BuildDateTime").toString();
        deviceInfo.cpuFreq = status2Object.value("CpuFrequency").toInt();
        deviceInfo.hardware = status2Object.value("Hardware").toString();

        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS5") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject status5Object = jsonDoc.object().value("StatusNET").toObject();
        deviceInfo.hostName = status5Object.value("Hostname").toString();
        deviceInfo.ipAddress = QHostAddress(status5Object.value("IPAddress").toString());
        deviceInfo.gateway = QHostAddress(status5Object.value("Gateway").toString());
        deviceInfo.dnsServer = QHostAddress(status5Object.value("DNSServer").toString());
        deviceInfo.macAddress = status5Object.value("Mac").toString();

        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS11") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject wifiObject = jsonDoc.object().value("StatusSTS").toObject().value("Wifi").toObject();
        deviceInfo.wifiSSID = wifiObject.value("SSId").toString();
        deviceInfo.wifiBSSID = wifiObject.value("BSSId").toString();
        deviceInfo.wifiChannel = wifiObject.value("Channel").toInt();
        deviceInfo.wifiRSSI = wifiObject.value("RSSI").toInt();
        deviceInfo.wifiSignal = wifiObject.value("Signal").toInt();

        emit deviceManager->device_InfoUpdate(deviceInfo);
    } else if (endTopic == "LOGGING") {

        emit recievedLogMessage(message.payload());
    } else if (endTopic == "RESULT") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject rootObject = jsonDoc.object();

        // Here is where we identify a device's capabilities (color, power, etc...) and set the respective values.

        if (!rootObject.value("POWER").isUndefined()) {
            deviceInfo.capabilities.power[0] = true;
            deviceInfo.power[0] = onOffToBool(rootObject.value("POWER").toString());
        }

        // Go through 4 power swiches
        for (int i = 0; i < 4; i++) {
            if (!rootObject.value("POWER" + QString::number(i + 1)).isUndefined()) {
                bool value = onOffToBool(rootObject.value("POWER" + QString::number(i + 1)).toString());
                deviceInfo.capabilities.power[i] = true;
                deviceInfo.power[i] = value;
            }
        }

        if (!rootObject.value("Dimmer").isUndefined()) {
            deviceInfo.capabilities.dimmer = true;
            deviceInfo.dimmer = rootObject.value("Dimmer").toInt();
        }

        // Go through 5 light channels
        for (int i = 0; i < 5; i++) {
            if (!rootObject.value("Channel" + QString::number(i + 1)).isUndefined()) {
                int value = rootObject.value("Channel" + QString::number(i + 1)).toInt();
                deviceInfo.capabilities.channels[i] = true;
                deviceInfo.channels[i] = map(value, 0, 100, 0, 255);
            }
        }

        if (!rootObject.value("HSBColor").isUndefined()) {
            deviceInfo.capabilities.color = true;
            QStringList colorValues = rootObject.value("HSBColor").toString().split(",");
            int h = colorValues[0].toInt();
            int s = map(colorValues[1].toInt(), 0, 100, 0, 255);
            int v = map(colorValues[2].toInt(), 0, 100, 0, 255);
            deviceInfo.color = QColor::fromHsv(h, s, v);
        }

        if (!rootObject.value("CT").isUndefined()) {
            deviceInfo.capabilities.colorTemp = true;
            deviceInfo.colorTemp = rootObject.value("CT").toInt();
        }

        if (!rootObject.value("Restart").isUndefined()) {
            deviceInfo.status = Restarting;
            emit deviceManager->device_InfoUpdate(deviceInfo);
        }

        emit recievedStateUpdate();

    }
}
