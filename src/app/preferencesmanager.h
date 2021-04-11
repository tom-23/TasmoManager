#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QObject>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QHostAddress>

struct VisibleColumns {
    bool DeviceName = true;
    bool FriendlyName = false;
    bool IPAddress = true;
    bool WIFIStrength = true;
    bool MACAddress = true;
    bool FirmwareVersion = true;
    bool Module = false;
    bool Status = true;
};


class PreferencesManager : public QObject
{
    Q_OBJECT
public:
    PreferencesManager(QObject *parent = nullptr);
    virtual ~PreferencesManager() {};

    // Preference Values
    QString backupLocation = "";
    bool useAutomaticDeviceScanning = true;
    bool showOfflineDevices = true;
    QHostAddress firstScanIP;
    QHostAddress lastScanIP;
    int versionChannel = 0;

    VisibleColumns *visibleColumns = new VisibleColumns();

    void loadPreferences();
    void savePreferences();

private:
    QString preferencesJSONLocation;
};

#endif // PREFERENCESMANAGER_H
