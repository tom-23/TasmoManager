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
#include <QMetaType>

#include "mqttserver.h"
#include <qmqtt.h>

#include "devicemanager.h"
class DeviceManager;
struct MQTTServerInfo;

enum SetOptionCategory {
    General,
    Buttons,
    Lighting,
    Temperature,
    MQTT,
    WIFI,
    Misc,
    IrRf,
    null,
};

class Device;

struct SetOption {
    int number;
    bool restartRequired = false;
    bool valueChanged;
    QString name;
    QString typeName;
    QString info;
    QString info1;
    QString info2;
    QString info3;
    QString info4;
    QString info5;
    QString info6;
    QString warning;
    QUrl link;
    QUrl link1;
    QList<QString> values;
    SetOptionCategory category;
    int value;
    int valueMin;
    int valueMax;
    QString valueString;
    Device *device;
};

enum DeviceStatus {
    Online,
    Offline,
    Restarting,
    Updating,
};

struct DeviceCapabilities {
    QList<bool> power = {false, false, false, false, false, false};
    QList<bool> channels = {false, false, false, false, false};
    bool color = false;
    bool colorTemp = false;
    bool dimmer = false;
};

struct DeviceInfo {
    DeviceStatus status = Offline;
    QString name;
    QString friendlyName;

    QString moduleTemplate;

    MQTTServerInfo *mqttServer;
    QString mqttTopic;
    QString mqttFullTopic;

    QUrl OTAUrl;
    int bootCount;

    QString firmwareVersion;
    QString coreSDKVersion;
    QString buildDateAndTime;
    int cpuFreq;
    QString hardware;

    QString hostName;
    QHostAddress ipAddress;
    QHostAddress subnetMask;
    QHostAddress gateway;
    QHostAddress dnsServer;
    QString macAddress;

    QString wifiSSID;
    QString wifiBSSID;
    int wifiChannel;
    int wifiRSSI = 0;
    int wifiSignal;

    DeviceCapabilities capabilities = DeviceCapabilities();

    QList<bool> power = {false, false, false, false, false, false};

    int dimmer;
    QList<int> channels = {0, 0, 0, 0, 0};

    QColor color;
    int colorTemp;

    QList<SetOption*> *setOptions;
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
    void setChannel(int channelID, int value);

    void setDimmer(int value);

    void setColor(QColor color);
    void setColorTemp(int colorTemp);


    void on_Message(QMQTT::Message message);

    void sendCommand(QString command);

    void getSetOptions(QList<SetOption *> *setOptionList);
    void saveSetOptions(QList<SetOption *> *setOptionList);

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

    int getSetOptionsAmount;
    int getSetOptionsProgress;


signals:

    void recievedLogMessage(QString message);
    void recievedInfoUpdate();
    void recievedStateUpdate();

    void setOptionValueUpdate(SetOption *setOption);
    void getSetOptionsDone();
};

#endif // DEVICE_H
