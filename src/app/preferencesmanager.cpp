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
        useAutomaticDeviceScanning = root.value("useAutomaticDeviceScannig").toBool();
        firstScanIP = QHostAddress(root.value("lastScanIP").toString());
        lastScanIP = QHostAddress(root.value("firstScanIP").toString());
        versionChannel = root.value("versionChannel").toInt();
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
        jsonDoc.setObject(root);

        file.resize(0);
        file.write(jsonDoc.toJson());
        qDebug() << "[Prefs Man] Saved preferences file!";

    } else {
        qDebug() << "[Prefs Man] Could not save server file.";
        qDebug() << "[Prefs Man] Error: " << file.errorString();
    }
}
