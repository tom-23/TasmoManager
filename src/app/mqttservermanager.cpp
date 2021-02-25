#include "mqttservermanager.h"

MQTTServerManager::MQTTServerManager(QObject *parent) : QObject(parent)
{
    serverListJSONLocation = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/.tasmoManagerServers.json";
    serverList = new QList<MQTTServerInfo>;
}

void MQTTServerManager::loadServerList() {

    QFile file(serverListJSONLocation);

    if (!file.exists()) {
        saveServerList();
    }

    if (file.open(QFile::ReadOnly)) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        QJsonArray serversArray = jsonDoc.array();

        for (int i = 0; i < serversArray.size(); i++ ) {
            QJsonObject server = serversArray.at(i).toObject();
            MQTTServerInfo serverInf;
            serverInf.ipAddress = QHostAddress(server.value("ipAddress").toString());
            serverInf.name = server.value("name").toString();
            serverInf.password = server.value("password").toString().toUtf8();
            serverInf.username = server.value("username").toString();
            serverInf.port = server.value("port").toInt();
            serverList->append(serverInf);
        }
    } else {
        qDebug() << "[Server Man] Could not load server file.";
    }
}

void MQTTServerManager::saveServerList() {
    QFile file(serverListJSONLocation);
    if (file.open(QFile::ReadWrite)) {
        QJsonDocument jsonDoc;
        QJsonArray serversArray;

        for (int i = 0; i < serverList->size(); i++ ) {
            QJsonObject server;
            MQTTServerInfo serverInfo = serverList->at(i);
            server.insert("ipAddress", serverInfo.ipAddress.toString());
            server.insert("name", serverInfo.name);
            server.insert("username", serverInfo.username);
            server.insert("password", QString::fromUtf8(serverInfo.password));
            server.insert("port", serverInfo.port);

            serversArray.append(server);

        }

        jsonDoc.setArray(serversArray);

        file.resize(0);
        file.write(jsonDoc.toJson());

    } else {
        qDebug() << "[Server Man] Could not load server file.";
    }
}
