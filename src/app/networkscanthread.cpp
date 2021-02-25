#include "networkscanthread.h"

NetworkScanThread::NetworkScanThread()
{
    timeoutTimer = new QTimer();
    timeoutTimer->setSingleShot(true);
    QObject::connect(timeoutTimer, &QTimer::timeout, this, &NetworkScanThread::cancel);
}

void NetworkScanThread::go() {
    qDebug() << "[Scan Thread] running. IP" << scanIP.toString();
    manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished, this, &NetworkScanThread::netManagerFinished);

    QNetworkRequest request;
    request.setUrl(QUrl("http://" + scanIP.toString() + "/cm?cmnd=Status%200"));
    netreply = manager->get(request);
    timeoutTimer->start(5000); // 5 sec timeout.
}

void NetworkScanThread::setIP(QHostAddress ipAddress) {
    scanIP = ipAddress;
}

void NetworkScanThread::netManagerFinished(QNetworkReply *reply) {
        if (reply->error()) {
            qDebug() << "[Scan Thread] IP: " << scanIP.toString() << " doesn't appear to be a tasmota device.";
            emit resultReady(DeviceInfo());
            return;
        }

        QString answer = reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());

        QJsonObject status = jsonResponse.object().value("Status").toObject();
        QJsonObject statusFWR = jsonResponse.object().value("StatusFWR").toObject();
        QJsonObject statusNET = jsonResponse.object().value("StatusNET").toObject();

        DeviceInfo deviceInfo;
        deviceInfo.name = status.value("DeviceName").toString();
        deviceInfo.friendlyName = status.value("FriendlyName").toArray()[0].toString();
        deviceInfo.macAddress = statusNET.value("Mac").toString();
        deviceInfo.ipAddress = QHostAddress(statusNET.value("IPAddress").toString());
        deviceInfo.firmwareVersion = statusFWR.value("Version").toString();

        qDebug() << "[Scan Thread] IP: " << scanIP.toString() << " is a tasmota device!";

        if (deviceInfo.macAddress == "") {
            qDebug() << "[Scan Thread] Didn't get MAC address from device. Moving on...";
            emit resultReady(DeviceInfo());
        } else {
            emit resultReady(deviceInfo);
        }

}

void NetworkScanThread::cancel() {
    netreply->abort();
}
