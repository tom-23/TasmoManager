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


class PreferencesManager : public QObject
{
    Q_OBJECT
public:
    PreferencesManager(QObject *parent = nullptr);
    virtual ~PreferencesManager() {};

    // Preference Values
    QString backupLocation = "";
    bool useAutomaticDeviceScanning = true;
    QHostAddress firstScanIP;
    QHostAddress lastScanIP;
    int versionChannel = 0;

    void loadPreferences();
    void savePreferences();

private:
    QString preferencesJSONLocation;
};

#endif // PREFERENCESMANAGER_H
