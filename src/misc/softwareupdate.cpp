#include "softwareupdate.h"

SoftwareUpdate::SoftwareUpdate(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager();
}

void SoftwareUpdate::getSoftwareUpdates() {
    latestUpdate = nullptr;
    networkManager = new QNetworkAccessManager();
    connect(networkManager, &QNetworkAccessManager::finished, this, &SoftwareUpdate::netManagerFinished);
    QNetworkRequest request;
    request.setUrl(updateURL);
    networkReply = networkManager->get(request);
}

void SoftwareUpdate::beginSoftwareUpdate(Update *update) {
    if (update == nullptr) {
        return;
    }
    networkManager = new QNetworkAccessManager();
    connect(networkManager, &QNetworkAccessManager::finished, this, [=] () {});
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setUrl(update->downloadPath);
    networkReply = networkManager->get(request);
    connect(networkReply, &QNetworkReply::downloadProgress, this, [=] (qint64 ist, qint64 max) {
        emit on_softwareUpdateProgress(0, max / 10000, ist / 10000, "Downloading Software Update File...");
    });
    connect(networkManager, &QNetworkAccessManager::finished, this, [=] (QNetworkReply *reply) {
        QString savelocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + update->downloadFileName;
        QFile file(savelocation);
        if (file.open(QIODevice::ReadWrite)) {
            file.resize(0);
            file.write(reply->readAll());
            file.close();
        } else {
            emit on_softwareUpdateProgress(0, 0, 0, "ERROR WRITING TO FILE");
            emit on_softwareUpdateError();
            return;
        }
        emit on_softwareUpdateProgress(0, 0, 0, "Downloaded Software Update!");
        installPackage(savelocation);
    });
}

void SoftwareUpdate::netManagerFinished(QNetworkReply *reply) {
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    semver_t current = {};
    QString currentVersion = TASMOMANAGER_VERSION;
    if (currentVersion.startsWith("v")) {
        currentVersion.remove(0, 1);
    }
    if (semver_parse(currentVersion.toStdString().c_str(), &current)) {
        qDebug("[S/W Update] Invalid semver string");
    }


    for (int i = 0; i < document.array().size(); i++) {
        QJsonObject updateObject = document.array().at(i).toObject();
        QString version = updateObject.value("tag_name").toString();
        if (version.startsWith("v")) {
            version.remove(0, 1);
        }
        semver_t compare = {};
        if (semver_parse(version.toStdString().c_str(), &compare)) {
            qDebug("[S/W Update] Invalid semver string");
        }

        bool betaAllowed = (versionChannel == Alpha || versionChannel == Beta);
        bool alphaAllowed = (versionChannel == Alpha);


        int resolution = semver_compare(compare, current);

        if (resolution == 0) {
        } else if (resolution == -1) {
        } else {

              Update *update = new Update;
              update->changelog = updateObject.value("body").toString();
              update->version = version;
              update->releasePath = QUrl(updateObject.value("html_url").toString());
              update->author = updateObject.value("author").toObject().value("login").toString();
              update->authorProfile = QUrl(updateObject.value("author").toObject().value("html_url").toString());
              QJsonArray assets = updateObject.value("assets").toArray();

              QUrl windowsInstaller;
              QString windowsInstallerName;

              QUrl macOSInstaller;
              QString macOSInstallerName;

              QUrl debInstaller;
              QString debInstallerName;
              QUrl rpmInstaller;
              QString rpmInstallerName;


              for (int a = 0; a < assets.size(); a++) {
                  QJsonObject asset = assets.at(a).toObject();
                  QString assetName = asset.value("name").toString();

                  if (assetName.endsWith(".exe")) {
                      windowsInstaller = QUrl(asset.value("browser_download_url").toString());
                      windowsInstallerName = asset.value("name").toString();
                  }
                  if (assetName.endsWith(".pkg")) {
                      macOSInstaller = QUrl(asset.value("browser_download_url").toString());
                      macOSInstallerName = asset.value("name").toString();
                  }
                  if (assetName.endsWith(".deb")) {
                      debInstaller = QUrl(asset.value("browser_download_url").toString());
                      debInstallerName = asset.value("name").toString();
                  }
                  if (assetName.endsWith(".deb")) {
                      rpmInstaller = QUrl(asset.value("browser_download_url").toString());
                      rpmInstallerName = asset.value("name").toString();
                  }
              }

              #ifdef __APPLE__
                update->downloadPath = macOSInstaller;
                update->downloadFileName = macOSInstallerName;
              #endif

              #ifdef _WIN32
                update->downloadPath = windowsInstaller;
                update->downloadFileName = windowsInstallerName;
              #endif

              #ifdef __linux__

                LinuxPackageManager packageManager = getLinuxPackageManager();

                if (packageManager == LinuxPackageManager::apt) {
                    update->downloadPath = debInstaller;
                    update->downloadFileName = debInstallerName;
                } else if (packageManager == LinuxPackageManager::rpm) {
                    update->downloadPath = rpmInstaller;
                    update->downloadFileName = rpmInstallerName;
                }
              #endif

              if (!updateObject.value("draft").toBool()) {
                  if (QString::fromUtf8(compare.prerelease).toLower().contains("alpha") && alphaAllowed) {
                      latestUpdate = update;
                      qDebug("[S/W Update] Version %s is higher than: %s so a new ALPHA update is avalible!", version.toStdString().c_str(), TASMOMANAGER_VERSION);
                      emit on_getUpdatesFinised();
                      return;
                  }

                  if (QString::fromUtf8(compare.prerelease).toLower().contains("beta") && betaAllowed) {
                      latestUpdate = update;
                      qDebug("[S/W Update] Version %s is higher than: %s so a new BETA update is avalible!", version.toStdString().c_str(), TASMOMANAGER_VERSION);
                      emit on_getUpdatesFinised();
                      return;
                  }

                  if (QString::fromUtf8(compare.prerelease).toLower() == "") {
                      latestUpdate = update;
                      qDebug("[S/W Update] Version %s is higher than: %s so a new STABLE update is avalible!", version.toStdString().c_str(), TASMOMANAGER_VERSION);
                      emit on_getUpdatesFinised();
                      return;
                  }
              }
        }
    }
    qDebug() << "[S/W Update] You are running the latest version of TasmoManager!";
    emit on_getUpdatesFinised();
}

bool SoftwareUpdate::isSudoPasswordCorrect(QString password) {
    QProcess sudoProcess(this);
    sudoProcess.setProcessChannelMode(QProcess::MergedChannels);
    QStringList processArguments;
    processArguments << "-c" << ("echo '" + password + "' | sudo -S echo correct");
    sudoProcess.start("/bin/sh", processArguments);
    sudoProcess.waitForStarted();
    sudoProcess.waitForReadyRead();
    sudoProcess.kill();
    sudoProcess.waitForFinished();
    if (sudoProcess.readAll().endsWith("correct\n")) {
        return true;
    } else {
        return false;
    }
}

LinuxPackageManager SoftwareUpdate::getLinuxPackageManager() {
    QProcess detectionProcess(this);
    detectionProcess.setProcessChannelMode(QProcess::MergedChannels);
    detectionProcess.start("apt", {"-v"});
    detectionProcess.waitForStarted();
    detectionProcess.waitForBytesWritten();
    if (detectionProcess.readAll().startsWith("apt")) {
        return LinuxPackageManager::apt;
    }

    detectionProcess.start("rpm", {""});
    detectionProcess.waitForStarted();
    detectionProcess.waitForBytesWritten();
    if (detectionProcess.readAll().startsWith("RPM")) {
        return LinuxPackageManager::rpm;
    }
}

void SoftwareUpdate::installPackage(QString packagePath) {
    qDebug() << "[S/W Update] Attempting to run install package...";
    emit on_softwareUpdateProgress(0, 0, 0, "Installing update...");
    QProcess *process = new QProcess(this);
    if (packagePath.endsWith(".pkg")) {
        QString appleScript = "do shell script \"/usr/sbin/installer -pkg " + packagePath + " -target /\" with administrator privileges";
        QString osascript = "/usr/bin/osascript";
        QStringList processArguments;
        processArguments << "-e" << appleScript;
        process->start(osascript, processArguments);
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
            Q_UNUSED(exitStatus);
            qDebug() << "[S/W Update] Finished install with exit code" << exitCode;
            if (exitCode == 0) {
                qDebug() << "[S/W Update] Restarting app...";
                qApp->quit();
                QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            } else {
                qDebug() << "[S/W Update] An error occoured whilst trying to install the app package.";
                emit on_softwareUpdateError();
            }
        });
    } else if (packagePath.endsWith(".exe")) {
        QStringList processArguments;
        processArguments << "/NOCANCEL" << "/NORESTART" << "/FORCECLOSEAPPLICATIONS" << "/RESTARTAPPLICATIONS" << "/SUPPRESSMSGBOXES" << "/SILENT";
        //process->startDetached(packagePath, processArguments);
        process->start(packagePath, processArguments);
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
            Q_UNUSED(exitStatus);
            qDebug() << "[S/W Update] Finished install with exit code" << exitCode;
            if (exitCode != 0) {
                qDebug() << "[S/W Update] An error occoured whilst trying to install the app package.";
                emit on_softwareUpdateError();
            }
        });
    } else if (packagePath.endsWith(".deb")) {
        QStringList processArguments;
        processArguments << "-c" << ("echo '" + sudoPassword + "' | sudo -S dpkg -i '" + packagePath + "'");
        process->start("/bin/sh", processArguments);
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
            Q_UNUSED(exitStatus);
            qDebug() << "[S/W Update] Finished install with exit code" << exitCode;
            qDebug() << process->readAllStandardError();
            if (exitCode == 0) {
                qDebug() << "[S/W Update] Restarting app...";
                qApp->quit();
                QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            } else {
                qDebug() << "[S/W Update] An error occoured whilst trying to install the app package.";
                emit on_softwareUpdateError();
            }
        });
    } else if (packagePath.endsWith(".rpm")) {
        QStringList processArguments;
        processArguments << "echo" << sudoPassword << "| " <<"sudo" << "rpm" << "-i" << packagePath << "--nodeps" << "--force";
        process->start("/bin/sh", processArguments);
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
            Q_UNUSED(exitStatus);
            qDebug() << "[S/W Update] Finished install with exit code" << exitCode;
            if (exitCode == 0) {
                qDebug() << "[S/W Update] Restarting app...";
                qApp->quit();
                QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            } else {
                qDebug() << "[S/W Update] An error occoured whilst trying to install the app package.";
                emit on_softwareUpdateError();
            }
        });
    } else {
        qDebug() << "[S/W Update] Looks like the app package isn't an installer";
        delete process;
        emit on_softwareUpdateError();
    }
}
