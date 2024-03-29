#ifndef SOFTWAREUPDATE_H
#define SOFTWAREUPDATE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QApplication>
#include <QProcess>
#include <QInputDialog>
#include "version.h"

#include <semver.h>

enum LinuxPackageManager {
    apt,
    rpm

};

enum VersionType {
    Stable,
    Beta,
    Alpha
};

struct Update {
    QString version;
    QString changelog;
    QUrl downloadPath;
    QString downloadFileName;
    QUrl releasePath;
    QString type;
    QString author;
    QUrl authorProfile;

};

class SoftwareUpdate : public QObject
{
    Q_OBJECT
public:
    explicit SoftwareUpdate(QObject *parent = nullptr);
    void getSoftwareUpdates();
    Update *latestUpdate = nullptr;

    void beginSoftwareUpdate(Update *update);

    bool isSudoPasswordCorrect(QString password);

    VersionType versionChannel = Stable;

    QString sudoPassword;

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *networkReply;
    QUrl updateURL = QUrl("https://api.github.com/repos/tom-23/tasmomanager/releases");
    void netManagerFinished(QNetworkReply *reply);
    void installPackage(QString packagePath);

    LinuxPackageManager getLinuxPackageManager();


signals:

    void on_getUpdatesFinised();
    void on_softwareUpdateProgress(int min, int max, int value, QString status);
    void on_softwareUpdateError();
};

#endif // SOFTWAREUPDATE_H
