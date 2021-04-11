#ifndef MQTTSERVER_H
#define MQTTSERVER_H

#include <QObject>
#include <QHostAddress>

struct MQTTServerInfo {
    QHostAddress ipAddress;
    QString host;
    int port;
    QString username;
    QByteArray password;
    QString clientID;
    QString name;
    bool currentlyConnected;
};

Q_DECLARE_METATYPE(MQTTServerInfo);
Q_DECLARE_METATYPE(MQTTServerInfo*);
Q_DECLARE_METATYPE(const MQTTServerInfo *);

#endif // MQTTSERVER_H
