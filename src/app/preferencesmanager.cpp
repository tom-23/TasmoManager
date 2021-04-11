#include "preferencesmanager.h"

PreferencesManager::PreferencesManager(QObject *parent) : QObject(parent)
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!dir.exists())
        dir.mkpath(".");
    preferencesJSONLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/.tasmoManagerPreferences.json";
    qDebug() << "[Prefs Man] JSON Location:" << preferencesJSONLocation;
}

void PreferencesManager::loadPreferences() {
    QFile file(preferencesJSONLocation);

    if (!file.exists()) {
        savePreferences();
    }

    if (file.open(QFile::ReadOnly)) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = jsonDoc.object();
        backupLocation = root.value("backupLocation").toString();
        useAutomaticDeviceScanning = root.value("useAutomaticDeviceScanning").toBool();
        firstScanIP = QHostAddress(root.value("lastScanIP").toString());
        lastScanIP = QHostAddress(root.value("firstScanIP").toString());
        versionChannel = root.value("versionChannel").toInt();
        showOfflineDevices = root.value("showOfflineDevices").toBool();

        QJsonObject visibleColumnsObj = root.value("visibleColumns").toObject();
        visibleColumns->DeviceName = visibleColumnsObj.value("deviceName").toBool(true);
        visibleColumns->FriendlyName = visibleColumnsObj.value("friendlyName").toBool();
        visibleColumns->IPAddress = visibleColumnsObj.value("ipAddress").toBool(true);
        visibleColumns->WIFIStrength = visibleColumnsObj.value("wifiStrength").toBool(true);
        visibleColumns->MACAddress = visibleColumnsObj.value("macAddress").toBool(true);
        visibleColumns->FirmwareVersion = visibleColumnsObj.value("firmwareVersion").toBool(true);
        visibleColumns->Module = visibleColumnsObj.value("module").toBool();
        visibleColumns->Status = visibleColumnsObj.value("status").toBool(true);

    } else {
        qDebug() << "[Prefs Man] Could not load preferences file.";
    }

}

void PreferencesManager::savePreferences() {
    QFile file(preferencesJSONLocation);
    if (file.open(QFile::ReadWrite)) {
        QJsonDocument jsonDoc;

        QJsonObject root;
        root.insert("backupLocation", backupLocation);
        root.insert("useAutomaticDeviceScannig", useAutomaticDeviceScanning);
        root.insert("firstScanIP", firstScanIP.toString());
        root.insert("lastScanIP", lastScanIP.toString());
        root.insert("versionChannel", versionChannel);
        root.insert("showOfflineDevices", showOfflineDevices);

        QJsonObject visibleColumnsObj;
        visibleColumnsObj.insert("deviceName", visibleColumns->DeviceName);
        visibleColumnsObj.insert("friendlyName", visibleColumns->FriendlyName);
        visibleColumnsObj.insert("ipAddress", visibleColumns->IPAddress);
        visibleColumnsObj.insert("wifiStrength", visibleColumns->WIFIStrength);
        visibleColumnsObj.insert("macAddress", visibleColumns->MACAddress);
        visibleColumnsObj.insert("firmwareVersion", visibleColumns->FirmwareVersion);
        visibleColumnsObj.insert("module", visibleColumns->Module);
        visibleColumnsObj.insert("status", visibleColumns->Status);

        root.insert("visibleColumns", visibleColumnsObj);

        jsonDoc.setObject(root);

        file.resize(0);
        file.write(jsonDoc.toJson());
        qDebug() << "[Prefs Man] Saved preferences file!";

    } else {
        qDebug() << "[Prefs Man] Could not save server file.";
        qDebug() << "[Prefs Man] Error: " << file.errorString();
    }
}
