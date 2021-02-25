#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>
#include <QDesktopServices>
#include <math.h>

#include "mqttserver.h"
#include <qmqtt.h>

#include "devicemanager.h"
class DeviceManager;
struct MQTTServerInfo;

enum DeviceStatus {
    Online,
    Offline,
    Restarting,
    Updating
};

struct DeviceCapabilities {
    QList<bool> power = {false, false, false, false};
    bool color = false;
    bool colorTemp = false;
    bool dimmer = false;
};

struct DeviceInfo {
    DeviceStatus status = Offline;
    QString name;
    QString friendlyName;

    QString moduleTemplate;

    MQTTServerInfo mqttServer;
    QString mqttTopic;
    QString mqttFullTopic;

    QUrl OTAUrl;
    int bootCount;

    QString firmwareVersion;
    QString buildDateAndTime;
    int cpuFreq;
    QString hardware;

    QString hostName;
    QHostAddress ipAddress;
    QHostAddress gateway;
    QHostAddress dnsServer;
    QString macAddress;

    QString wifiSSID;
    QString wifiBSSID;
    int wifiChannel;
    int wifiRSSI = 0;
    int wifiSignal;

    DeviceCapabilities capabilities;

    QList<bool> power = {false, false, false, false};

    int dimmer;

    QColor color;
    int colorTemp;
};

Q_DECLARE_METATYPE(DeviceInfo)
Q_DECLARE_METATYPE(DeviceInfo*)

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);

    DeviceManager *deviceManager;
    DeviceInfo deviceInfo;

    void getDeviceInfo(QString fullTopic = "", int delay = 500);
    void setDeviceNames(QString name, QString fullName);

    void openWebUI();

    void setLogLevel(int level);

    void getState();

    void restart();

    void setPower(int powerID, bool on);

    void setColor(QColor color);
    void setColorTemp(int colorTemp);


    void on_Message(QMQTT::Message message);

    void sendCommand(QString command);

    QString statTopic;
    QString cmndTopic;
    QString teleTopic;

private:

    bool isSubscribedToStat = false;

    QString boolToOnOff(bool value) {
        if (value) {
            return "ON";
        } else {
            return "OFF";
        }
    }

    bool onOffToBool(QString value) {
        if (value.toUpper() == "ON") {
            return true;
        } else {
            return false;
        }
    }

    int map(float value, float istart, float istop, float ostart, float ostop) {
        return round(ostart + (ostop - ostart) * ((value - istart) / (istop - istart)));
    }


signals:

    void recievedLogMessage(QString message);
    void recievedStateUpdate();
};

#endif // DEVICE_H
