#ifndef NETWORKSCANTHREAD_H
#define NETWORKSCANTHREAD_H

#include <QThread>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include "devicemanager.h"

class NetworkScanThread : public QObject
{
    Q_OBJECT
public:
    NetworkScanThread();
    void go();
    void setIP(QHostAddress ipAddress);
private:
    void netManagerFinished(QNetworkReply *reply);
    QHostAddress scanIP;
    QNetworkAccessManager * manager;

    QNetworkReply *netreply;

    void cancel();

    QTimer *timeoutTimer;
signals:
    void resultReady(DeviceInfo deviceInfo);
};

#endif // NETWORKSCANTHREAD_H
