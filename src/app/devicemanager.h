#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFile>
#include <QMessageBox>

#include "version.h"

#include <qmqtt.h>

#include "mqttserver.h"

#include "device.h"
class Device;
struct DeviceInfo;
struct SetOption;
#include "networkscanthread.h"

enum ConnectionStatus {
    Connected,
    Connecting,
    Disconnected
};

struct MQTTServerInfo;

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceManager(QObject *parent = nullptr);

    QMQTT::Client *mqttClient;
    MQTTServerInfo *mqttServerInfo;

    void connect(MQTTServerInfo *serverInfo);
    void disconnect();

    void subscribeToDiscovery();

    QList<Device *> *deviceList;
    QList<SetOption> *setOptionsList;

    void scanNewDevices();
    void setupNewDevice(DeviceInfo baseInfo, DeviceInfo newInfo);


    void refreshDevices();

    QString discoveryTopic = "tasmota/discovery";

    Device* getDeviceByMAC(QString mac);
    bool compaireMAC(QString firstMAC, QString secondMAC);

    ConnectionStatus connectionStatus;

    void loadSetOptionsSchema();

    const QUrl mainStableOTA = QUrl("http://ota.tasmota.com/tasmota/release/tasmota.bin");
    const QUrl mainDevOTA = QUrl("http://ota.tasmota.com/tasmota/tasmota.bin");

private:

    QList<QHostAddress> getIPAddresses(QNetworkAddressEntry networkAddress);
    QList<QNetworkAddressEntry> getActiveAddresses();

    QList<QHostAddress> addressesToScan;
    int addressesScanned;

    int stepIndex;
    QString setupNewDeviceMac = "";

    QNetworkAccessManager *networkManager;

    void on_mqttMessage(QMQTT::Message message);

    int getDeviceDiscoveryInfoDelay = 0;
    int getDeviceInfoDelay = 0;

    int refreshDelay = 200;
signals:

    void scanNewDevices_Progress(int progress, int amount);
    void scanNewDevices_NewDeviceFound(DeviceInfo deviceInfo);

    void setupNewDevices_Progress(int progress, int amount, QString currentStatus);

    void mqtt_onConnect();
    void mqtt_onDisconnect();
    void mqtt_onError(QMQTT::ClientError clientError);

    void device_Discovered(DeviceInfo deviceInfo);
    void device_InfoUpdate(DeviceInfo deviceInfo);

    void schemeLoadFailed();
};

#endif // DEVICEMANAGER_H
