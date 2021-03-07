#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{
    deviceInfo.mqttServer->currentlyConnected = false;
    deviceInfo.mqttServer->name = "device";
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

void Device::getSetOptions(QList<SetOption *> *setOptionList) {
    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");
    QString payload = "";
    for (int i = 0; i < setOptionList->size(); i++) {
        SetOption *setOption = setOptionList->at(i);
        payload = payload + "SetOption" + QString::number(setOption->number);
        if (i != setOptionList->size()) {
            payload = payload + "; ";
        }
    }
    getSetOptionsAmount = setOptionList->size() - 1;
    getSetOptionsProgress = 0;
    message.setPayload(payload.toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::saveSetOptions(QList<SetOption *> *setOptionList) {
    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");
    QString payload = "";
    for (int i = 0; i < setOptionList->size(); i++) {
        SetOption *setOption = setOptionList->at(i);

        if (setOption->typeName == "ENUM" || setOption->typeName == "INTEGER") {
            payload = payload + "SetOption" + QString::number(setOption->number) + " "
            + QString::number(setOption->value);
        } else {
            payload = payload + "SetOption" + QString::number(setOption->number) + " "
            + setOption->valueString;
        }

        if (i != setOptionList->size()) {
            payload = payload + "; ";
        }
    }
    getSetOptionsAmount = setOptionList->size() - 1;
    getSetOptionsProgress = 0;
    message.setPayload(payload.toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::getWifiNetworkSettings() {
    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");
    QString backlog = "AP; Ssid; IPAddress1; IPAddress2; IPAddress3; IPAddress4";
    message.setPayload(backlog.toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::setWifiNetworkSettings() {

    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");

    QString backlog = "";
    backlog = backlog + "AP " + QString::number(deviceInfo.activeAP) + "; ";
    backlog = backlog + "Ssid1 " + deviceInfo.ap1SSID + "; ";
    backlog = backlog + "Ssid2 " + deviceInfo.ap2SSID + "; ";

    if (deviceInfo.ap1Password != "") {
        backlog = backlog + "Password1 " + deviceInfo.ap1Password + "; ";

    }

    if (deviceInfo.ap2Password != "") {
        backlog = backlog + "Password2 " + deviceInfo.ap2Password + "; ";
    }

    if (deviceInfo.useStaticIP) {
        backlog = backlog + "IPAddress1 " + deviceInfo.ipAddress.toString() + "; ";
        backlog = backlog + "IPAddress2 " + deviceInfo.gateway.toString() + "; ";
        backlog = backlog + "IPAddress3 " + deviceInfo.subnetMask.toString() + "; ";
        backlog = backlog + "IPAddress4 " + deviceInfo.dnsServer.toString() + "; ";
    } else {
        backlog = backlog + "IPAddress1 0.0.0.0; ";
    }
    backlog = backlog + "restart 1";

    message.setPayload(backlog.toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::getMQTTSettings() {
    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");
    QString backlog = "FullTopic; MqttClient; MqttHost; MqttUser; MqttPort; MqttPassword; Topic";
    message.setPayload(backlog.toUtf8());
    deviceManager->mqttClient->publish(message);
}

void Device::setMQTTSettings() {
    QMQTT::Message message;
    message.setTopic(cmndTopic + "Backlog");

    QString backlog = "";
    backlog = backlog + "FullTopic " + deviceInfo.mqttFullTopic + "; ";
    backlog = backlog + "Topic " + deviceInfo.mqttTopic + "; ";
    backlog = backlog + "MqttClient " + deviceInfo.mqttClient + "; ";
    QString host;
    if (!deviceInfo.mqttServer->ipAddress.isNull()) {
        host = deviceInfo.mqttServer->ipAddress.toString();
    } else {
        host = deviceInfo.mqttServer->host;
    }
    backlog = backlog + "MqttHost " + host + "; ";
    backlog = backlog + "MqttPort " + QString::number(deviceInfo.mqttServer->port) + "; ";
    backlog = backlog + "MqttUser " + deviceInfo.mqttServer->username + "; ";
    if (!(deviceInfo.mqttServer->password == "****")) {
        backlog = backlog + "MqttPassword " + deviceInfo.mqttServer->password + "; ";
    }

    message.setPayload(backlog.toUtf8());
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
        emit recievedInfoUpdate();
        emit deviceManager->device_InfoUpdate(deviceInfo);
    }

    if (endTopic == "STATUS") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject statusObject = jsonDoc.object().value("Status").toObject();
        deviceInfo.name = statusObject.value("DeviceName").toString();
        deviceInfo.friendlyName = statusObject.value("FriendlyName").toArray()[0].toString();

        emit recievedInfoUpdate();
        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS1") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject status1Object = jsonDoc.object().value("StatusPRM").toObject();
        deviceInfo.OTAUrl = QUrl(status1Object.value("OtaUrl").toString());
        deviceInfo.bootCount = status1Object.value("BootCount").toInt();

        emit recievedInfoUpdate();
        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS2") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject status2Object = jsonDoc.object().value("StatusFWR").toObject();
        deviceInfo.firmwareVersion = status2Object.value("Version").toString();
        deviceInfo.coreSDKVersion = status2Object.value("Core").toString() + " / " + status2Object.value("SDK").toString();
        deviceInfo.buildDateAndTime = status2Object.value("BuildDateTime").toString();
        deviceInfo.cpuFreq = status2Object.value("CpuFrequency").toInt();
        deviceInfo.hardware = status2Object.value("Hardware").toString();

        emit recievedInfoUpdate();
        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS5") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject status5Object = jsonDoc.object().value("StatusNET").toObject();
        deviceInfo.hostName = status5Object.value("Hostname").toString();
        deviceInfo.ipAddress = QHostAddress(status5Object.value("IPAddress").toString());
        deviceInfo.subnetMask = QHostAddress(status5Object.value("Subnetmask").toString());
        deviceInfo.gateway = QHostAddress(status5Object.value("Gateway").toString());
        deviceInfo.dnsServer = QHostAddress(status5Object.value("DNSServer").toString());
        deviceInfo.macAddress = status5Object.value("Mac").toString();

        emit recievedInfoUpdate();
        emit deviceManager->device_InfoUpdate(deviceInfo);

    } else if (endTopic == "STATUS11") {

        jsonDoc = QJsonDocument::fromJson(message.payload());
        QJsonObject wifiObject = jsonDoc.object().value("StatusSTS").toObject().value("Wifi").toObject();
        deviceInfo.wifiSSID = wifiObject.value("SSId").toString();
        deviceInfo.wifiBSSID = wifiObject.value("BSSId").toString();
        deviceInfo.wifiChannel = wifiObject.value("Channel").toInt();
        deviceInfo.wifiRSSI = wifiObject.value("RSSI").toInt();
        deviceInfo.wifiSignal = wifiObject.value("Signal").toInt();

        emit recievedInfoUpdate();
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
        for (int i = 0; i < deviceInfo.capabilities.power.size(); i++) {
            if (!rootObject.value("POWER" + QString::number(i + 1)).isUndefined()) {
                bool value = onOffToBool(rootObject.value("POWER" + QString::number(i + 1)).toString());
                deviceInfo.capabilities.power[i] = true;
                deviceInfo.power[i] = value;
            }
        }

        // Go through 5 light channels
        for (int i = 0; i < deviceInfo.capabilities.channels.size(); i++) {
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

        if (!rootObject.value("Dimmer").isUndefined()) {
            deviceInfo.capabilities.dimmer = true;
            deviceInfo.dimmer = rootObject.value("Dimmer").toInt();

            if (deviceInfo.capabilities.colorTemp) {
                deviceInfo.color.setHsv(deviceInfo.color.hue(), deviceInfo.color.saturation(), map(deviceInfo.dimmer, 0, 100, 0, 255));
            }
        }

        if (!rootObject.value("Restart").isUndefined()) {
            deviceInfo.status = Restarting;
            emit deviceManager->device_InfoUpdate(deviceInfo);
        }

        if (!rootObject.value("Ap").isUndefined()) {
            QJsonObject ap = rootObject.value("Ap").toObject();
            int activeAP = 0;
            if (!ap.value("1").isUndefined()) {
                activeAP = 1;
            }
            if (!ap.value("2").isUndefined()) {
                activeAP = 2;
            }
            deviceInfo.activeAP = activeAP;
            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("SSId1").isUndefined()) {
            deviceInfo.ap1SSID = rootObject.value("SSId1").toString();
            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("SSId2").isUndefined()) {
            deviceInfo.ap2SSID = rootObject.value("SSId2").toString();
            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("IPAddress1").isUndefined()) {
            QString ipAddress = rootObject.value("IPAddress1").toString().toUpper();
            deviceInfo.useStaticIP = !ipAddress.contains("(IP UNSET)");
            QStringList ipAddressSplit = ipAddress.replace("(IP UNSET)", "").split(" ");
            if (deviceInfo.useStaticIP) {
                deviceInfo.ipAddress = QHostAddress(ipAddressSplit[1]);
            } else {
                deviceInfo.ipAddress = QHostAddress(ipAddress);
            }

            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("IPAddress2").isUndefined()) {
            QString gatewayAddress = rootObject.value("IPAddress2").toString();
            deviceInfo.gateway = QHostAddress(gatewayAddress);
            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("IPAddress3").isUndefined()) {
            QString subnetAddress = rootObject.value("IPAddress3").toString();
            deviceInfo.subnetMask = QHostAddress(subnetAddress);
            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("IPAddress4").isUndefined()) {
            QString dnsAddress = rootObject.value("IPAddress4").toString();
            deviceInfo.dnsServer = QHostAddress(dnsAddress);
            emit recievedWifiInfoUpdate();
        }

        if (!rootObject.value("MqttClient").isUndefined()) {
            QString mqttClient = rootObject.value("MqttClient").toString();
            deviceInfo.mqttClient = mqttClient;
            emit recievedMQTTInfoUpdate();
        }

        if (!rootObject.value("MqttHost").isUndefined()) {
            QString mqttHost = rootObject.value("MqttHost").toString();
            if (QHostAddress(mqttHost).isNull()) {
                deviceInfo.mqttServer->host = mqttHost;
            } else {
                deviceInfo.mqttServer->ipAddress = QHostAddress(mqttHost);
            }
            emit recievedMQTTInfoUpdate();
        }

        if (!rootObject.value("MqttUser").isUndefined()) {
            QString mqttUser = rootObject.value("MqttUser").toString();
            deviceInfo.mqttServer->username = mqttUser;
            emit recievedMQTTInfoUpdate();
        }

        if (!rootObject.value("MqttPort").isUndefined()) {
            int mqttPort = rootObject.value("MqttPort").toInt();
            deviceInfo.mqttServer->port = mqttPort;
            emit recievedMQTTInfoUpdate();
        }

        if (!rootObject.value("MqttPassword").isUndefined()) {
            QString mqttPassword = rootObject.value("MqttPassword").toString();
            deviceInfo.mqttServer->password = mqttPassword.toUtf8();
            emit recievedMQTTInfoUpdate();
        }

        if (!rootObject.value("Topic").isUndefined()) {
            QString mqttTopic = rootObject.value("Topic").toString();
            deviceInfo.mqttTopic = mqttTopic;
            emit recievedMQTTInfoUpdate();
        }

        if (!rootObject.value("FullTopic").isUndefined()) {
            QString mqttFullTopic = rootObject.value("FullTopic").toString();
            deviceInfo.mqttFullTopic = mqttFullTopic;
            emit recievedMQTTInfoUpdate();
        }

        for (int i = 0; i < deviceInfo.setOptions->size(); i++) {
            SetOption *setOption = deviceInfo.setOptions->at(i);
            if (!rootObject.value("SetOption" + QString::number(setOption->number)).isUndefined()) {
                QJsonValue value = rootObject.value("SetOption" + QString::number(setOption->number));
                if (value.type() == QJsonValue::String) {
                    if (value.toString() == "OFF") {
                        setOption->value = 0;

                    } else if (value.toString() == "ON") {
                        setOption->value = 1;
                    } else {
                        setOption->valueString = value.toString();
                    }
                } else {
                    setOption->value = value.toInt();
                }
                emit setOptionValueUpdate(setOption);
                getSetOptionsProgress = getSetOptionsProgress + 1;
                if (getSetOptionsProgress - 1 == getSetOptionsAmount) {
                    emit getSetOptionsDone();
                }
            }

        }

        emit recievedStateUpdate();
    }
}
