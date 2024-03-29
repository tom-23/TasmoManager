#ifndef MQTTSERVERMANAGER_H
#define MQTTSERVERMANAGER_H

#include <QObject>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "mqttserver.h"
struct DeviceInfo;

class MQTTServerManager : public QObject
{
    Q_OBJECT
public:
    explicit MQTTServerManager(QObject *parent = nullptr);

    void loadServerList();
    void saveServerList();

    QList<MQTTServerInfo*> *serverList = new QList<MQTTServerInfo*>;


private:
    QString serverListJSONLocation;
signals:

};

#endif // MQTTSERVERMANAGER_H
