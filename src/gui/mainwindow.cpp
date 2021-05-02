#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    deviceManager = new DeviceManager(this);
    deviceManager->loadSetOptionsSchema();
    serverManager = new MQTTServerManager(this);
    serverManager->loadServerList();
    preferencesManager = new PreferencesManager(this);
    preferencesManager->loadPreferences();

    ui->actionDeviceName->setChecked(preferencesManager->visibleColumns->DeviceName);
    ui->actionFriendlyName->setChecked(preferencesManager->visibleColumns->FriendlyName);
    ui->actionIP_Address->setChecked(preferencesManager->visibleColumns->IPAddress);
    ui->actionWIFI_Strengt->setChecked(preferencesManager->visibleColumns->WIFIStrength);
    ui->actionMAC_Address->setChecked(preferencesManager->visibleColumns->MACAddress);
    ui->actionFirmware_Version->setChecked(preferencesManager->visibleColumns->FirmwareVersion);
    ui->actionModule->setChecked(preferencesManager->visibleColumns->Module);
    ui->actionStatus->setChecked(preferencesManager->visibleColumns->Status);

    ui->newUpdateBar->setVisible(false);

    ui->deviceList->setColumnWidth(0, 200);
    ui->deviceList->setColumnWidth(1, 200);
    ui->deviceList->setColumnWidth(2, 125);
    ui->deviceList->setColumnWidth(4, 125);
    ui->deviceList->setColumnWidth(5, 125);

    connect(deviceManager, &DeviceManager::device_Discovered, this, &MainWindow::on_deviceDiscovered);
    connect(deviceManager, &DeviceManager::device_InfoUpdate, this, &MainWindow::on_deviceInfoUpdate);

    connect(deviceManager, &DeviceManager::mqtt_onConnect, this, [=] () {
        deviceManager->subscribeToDiscovery();
        updateInfoText();
    });

    connect(deviceManager, &DeviceManager::schemeLoadFailed, this, [this] () {
        auto m = new QMessageBox(this);
        m->setText("Fatal Error");
        m->setInformativeText("Unable to load setoptions.schema.json!");
        m->setIcon(QMessageBox::Critical);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        this->close();
    });
    QTimer *errorCollectionTimer = new QTimer(this);
    errorCollectionTimer->setSingleShot(true);

    QList<QMQTT::ClientError> *errorList = new QList<QMQTT::ClientError>;

    connect(deviceManager, &DeviceManager::mqtt_onError, this, [=] (QMQTT::ClientError error) {

        // Only show message box is we are sure MQTT is disconnected.

            if (!errorCollectionTimer->isActive()) {
                errorList->clear();
                errorCollectionTimer->start(500);
            }
            errorList->append(error);
    });

    connect(errorCollectionTimer, &QTimer::timeout, this, [=] () {
        if (deviceOptionsWidget != nullptr) {
            deviceOptionsWidget->reject();
        }
        auto m = new QMessageBox(this);
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->setText("Cannot connect to server");

        for (int i = 0; i < errorList->size(); i++) {
            QMQTT::ClientError error = errorList->at(i);
            if (error == QMQTT::MqttBadUserNameOrPasswordError || error == QMQTT::MqttNotAuthorizedError) {
                m->setText("Incorrect / Bad Username or Password");
            }
            m->setInformativeText(m->informativeText() + "Error: " + QVariant::fromValue(error).toString() + "\n");
        }
        if (m->informativeText().endsWith("\n")) {
            m->setInformativeText(m->informativeText().left(m->informativeText().lastIndexOf("\n")));
        }
        m->show();
        updateInfoText();
    });

    updateInfoText();

    ui->backupButton->setVisible(false);

    connect(softwareUpdate, &SoftwareUpdate::on_getUpdatesFinised, this, [=] () {
        if (softwareUpdate->latestUpdate != nullptr) {
            ui->newUpdateLabel->setText("A new version of TasmoManager can be installed (" +
                                        softwareUpdate->latestUpdate->version +
                                        "). Go to Settings > Software Update to install.");
            ui->newUpdateBar->setVisible(true);
        } else {
            ui->newUpdateBar->setVisible(false);
        }
    });

    checkForUpdates();
    this->showMaximized();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // We only want to show a warning dialog if the user is still connected.
    if (deviceManager->mqttClient != nullptr) {
        if (deviceManager->connectionStatus == Connected || deviceManager->connectionStatus == Connecting) {
            auto m = new QMessageBox(this);
            m->setText("Are you sure you want to quit?");
            m->setInformativeText("You are currently connected / connecting to '" + deviceManager->mqttServerInfo->name + "'.");
            m->setIcon(QMessageBox::Warning);
            m->setWindowModality(Qt::WindowModal);
            m->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            if (m->exec() == QMessageBox::Yes) {
                deviceManager->disconnect();
                event->accept();
            } else {
                event->ignore();
            }
        }
    }
    preferencesManager->savePreferences();
}

void MainWindow::on_actiontest_triggered()
{
    SetupDeviceDialog *setupDeviceDialog = new SetupDeviceDialog(this, deviceManager, serverManager);
    setupDeviceDialog->exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    preferencesDialog = new PreferencesDialog(this);
    preferencesDialog->setWindowModality(Qt::WindowModal);
    preferencesDialog->setMQTTManager(serverManager);
    preferencesDialog->setPreferencesManager(preferencesManager);
    preferencesDialog->exec();
    updatePrefs();
}

void MainWindow::on_connectButton_clicked()
{
    if (ui->connectButton->text() == "Connect") {
        if (serverManager->serverList->size() == 0) {
            on_actionPreferences_triggered();
            return;
        }
        SelectServerDialog *selectServerDialog = new SelectServerDialog(this);
        selectServerDialog->setMQTTServerManager(serverManager);
        selectServerDialog->setWindowModality(Qt::WindowModal);
        if (selectServerDialog->exec() == 1) {
            deviceManager->connect(selectServerDialog->selectedServer);
            updateInfoText();
        }

    } else {
        deviceManager->disconnect();
        updateInfoText();
    }
    qApp->processEvents(QEventLoop::AllEvents);
}

void MainWindow::on_deviceDiscovered(DeviceInfo deviceInfo) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, deviceInfo.name);
    item->setText(1, deviceInfo.friendlyName);
    item->setText(2, deviceInfo.ipAddress.toString());
    item->setText(3, "");
    item->setText(4, deviceInfo.macAddress);
    item->setText(5, "");
    item->setText(6, deviceInfo.module);
    item->setText(7, "Offline");
    item->setHidden(!preferencesManager->showOfflineDevices);
    ui->deviceList->addTopLevelItem(item);
    if (ui->deviceList->selectedItems().size() == 0) {
        ui->deviceList->topLevelItem(0)->setSelected(true);
    }
    updateStatusBar();
}

void MainWindow::on_deviceInfoUpdate(DeviceInfo deviceInfo) {
    updateStatusBar();
    for (int i = 0; i < ui->deviceList->topLevelItemCount(); i++) {

        QTreeWidgetItem *item = ui->deviceList->topLevelItem(i);
        QString itemMac = item->text(4);
        QString deviceInfoMac = deviceInfo.macAddress;

        if (deviceManager->compaireMAC(deviceInfoMac, itemMac)) {
            item->setText(0, deviceInfo.name);
            item->setText(1, deviceInfo.friendlyName);
            item->setText(2, deviceInfo.ipAddress.toString());

            QIcon icon;
            int rssi = deviceInfo.wifiRSSI;

            if (rssi >= 80) {
                icon.addFile(":/16/assets/16_great_signal.svg");
            } else if (rssi < 80 && rssi >= 50){
                icon.addFile(":/16/assets/16_good_signal.svg");
            } else if (rssi < 50 && rssi > 0){
                icon.addFile(":/16/assets/16_weak_signal.svg");
            }

            if (rssi != 0) {
                item->setIcon(3, icon);
                item->setText(3, QString::number(deviceInfo.wifiRSSI) + "% (" + QString::number(deviceInfo.wifiSignal) + " dBm)");
            } else {
                item->setText(3, "n/a");
            }

            item->setText(4, deviceInfo.macAddress);
            item->setText(5, deviceInfo.firmwareVersion);
            item->setText(6, deviceInfo.module);

            item->setHidden(false);

            if (deviceInfo.status == DeviceStatus::Online) {
                QIcon onlineIcon;
                onlineIcon.addFile(":/16/assets/16_online.svg");
                item->setIcon(7, onlineIcon);
                item->setText(7, "Online");

            } else if (deviceInfo.status == DeviceStatus::Offline) {
                QIcon offlineIcon;
                offlineIcon.addFile(":/16/assets/16_offline.svg");
                item->setIcon(7, offlineIcon);
                item->setText(7, "Offline");
                item->setHidden(!preferencesManager->showOfflineDevices);
            } else if (deviceInfo.status == DeviceStatus::Restarting) {
                QIcon restartingIcon;
                restartingIcon.addFile(":/16/assets/16_restarting.svg");
                item->setIcon(7, restartingIcon);
                item->setText(7, "Restarting...");
            } else if (deviceInfo.status == DeviceStatus::Updating) {
                QIcon updatingIcon;
                updatingIcon.addFile(":/16/assets/16_updating.svg");
                item->setIcon(7, updatingIcon);
                item->setText(7, "Updating...");
            }

            if (selectedDevice != nullptr) {
                if (selectedDevice->deviceInfo.status == DeviceStatus::Online) {
                    ui->deviceInfoWidget->setDevice(selectedDevice);
                    ui->devicePowerWidget->setDevice(selectedDevice);
                    ui->deviceColorWidget->setDevice(selectedDevice);
                    ui->deviceActionsContainer->setEnabled(true);
                } else {
                    ui->deviceInfoWidget->setDevice(nullptr);
                    ui->devicePowerWidget->setDevice(nullptr);
                    ui->deviceColorWidget->setDevice(nullptr);
                    ui->deviceActionsContainer->setEnabled(false);
                }
            }

            if (!(deviceInfo.status == DeviceStatus::Online)) {
                if (deviceOptionsWidget != NULL) {
                    if (deviceOptionsWidget->isVisible()) {
                        if (deviceOptionsWidget->deviceInfoMac == deviceInfoMac) {
                            deviceOptionsWidget->close();
                            delete deviceOptionsWidget;
                            deviceOptionsWidget = NULL;
                            auto m = new QMessageBox(this);
                            m->setText("Device is offline");
                            m->setInformativeText("'" + deviceInfo.name + "' is restarting. Please wait...");
                            m->setIcon(QMessageBox::Information);
                            m->setWindowModality(Qt::WindowModal);
                            m->setStandardButtons(QMessageBox::Ok);
                            m->show();
                        }
                    }
                }
            }

            return;
        }
    }
}

void MainWindow::on_terminalButton_clicked()
{
    TerminalDialog *terminalDialog = new TerminalDialog(this, selectedDevice);
    terminalDialog->show();
}

void MainWindow::on_deviceList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (current == nullptr) {
        updateWidgetDevices(nullptr);
        return;
    }
    selectedDevice = deviceManager->getDeviceByMAC(current->text(4));
    if (selectedDevice == nullptr) {
        updateWidgetDevices(nullptr);
        return;
    }

    if (selectedDevice->deviceInfo.status == DeviceStatus::Online) {
        updateWidgetDevices(selectedDevice);
    } else {
        updateWidgetDevices(nullptr);
    }
}

void MainWindow::updateWidgetDevices(Device *device) {
    ui->deviceInfoWidget->setDevice(device);
    ui->devicePowerWidget->setDevice(device);
    ui->deviceColorWidget->setDevice(device);
    ui->deviceActionsContainer->setEnabled((device != nullptr));
}

void MainWindow::on_webUIButton_clicked()
{
    selectedDevice->openWebUI();
}

void MainWindow::on_settingsButton_clicked()
{
    on_actionPreferences_triggered();
}

void MainWindow::updateInfoText() {

    if (deviceManager->connectionStatus == Connected || deviceManager->connectionStatus == Connecting) {
        if (deviceManager->mqttServerInfo->username != "") {
            ui->userLabel->setText("User: " + deviceManager->mqttServerInfo->username);
        } else {
            ui->userLabel->setText("User: none");
        }
        QString host;
        if (!deviceManager->mqttServerInfo->ipAddress.isNull()) {
            host = deviceManager->mqttServerInfo->ipAddress.toString();
        } else {
            host = deviceManager->mqttServerInfo->host;
        }
        ui->serverLabel->setText("Server: " + deviceManager->mqttServerInfo->name + " (" +  host + ":" + QString::number(deviceManager->mqttServerInfo->port) + ")");
    } else {
        ui->userLabel->setText("User: -");
        ui->serverLabel->setText("Server: -");
    }

    QIcon icon;

    if (deviceManager->connectionStatus == Connected) {

        icon.addFile(":/32/assets/32_disconnect.svg");
        ui->connectButton->setIcon(icon);
        ui->connectButton->setText("Disconnect");
        ui->statusLabel->setText("Status: Connected");

        ui->menuDevices->setEnabled(true);
        ui->refreshButton->setEnabled(true);
        ui->deviceToolsContainer->setEnabled(true);

    } else if (deviceManager->connectionStatus == Connecting) {

        icon.addFile(":/32/assets/32_connecting.svg");
        ui->connectButton->setIcon(icon);
        ui->connectButton->setText("Connecting");
        ui->statusLabel->setText("Status: Connecting...");

    } else if (deviceManager->connectionStatus == Disconnected) {

        icon.addFile(":/32/assets/32_connect.svg");
        ui->connectButton->setIcon(icon);
        ui->connectButton->setText("Connect");
        ui->statusLabel->setText("Status: Disconnect");


        ui->deviceList->clear();
        ui->deviceColorWidget->setVisible(false);
        ui->deviceInfoWidget->setVisible(false);
        ui->devicePowerWidget->setVisible(false);

        ui->menuDevices->setEnabled(false);

        ui->deviceActionsContainer->setEnabled(false);
        ui->deviceToolsContainer->setEnabled(false);
        ui->refreshButton->setEnabled(false);
    }


}

void MainWindow::on_actionAbout_triggered()
{
    on_actionPreferences_triggered();
    preferencesDialog->goToAboutPage();
}

void MainWindow::on_refreshButton_clicked()
{
    deviceManager->refreshDevices();
}

void MainWindow::on_deviceButton_clicked()
{
    deviceOptionsWidget = new DeviceOptionsWidget(this);
    deviceOptionsWidget->setWindowModality(Qt::WindowModality::WindowModal);
    deviceOptionsWidget->setDevice(selectedDevice);
    deviceOptionsWidget->exec();
}

void MainWindow::on_restartButton_clicked()
{
    selectedDevice->restart();
}

void MainWindow::on_mqttButton_clicked()
{
    deviceOptionsWidget = new DeviceOptionsWidget(this);
    deviceOptionsWidget->setWindowModality(Qt::WindowModality::WindowModal);
    deviceOptionsWidget->setDevice(selectedDevice);
    deviceOptionsWidget->showMQTTPage();
    deviceOptionsWidget->exec();
}

void MainWindow::on_wifiButton_clicked()
{
    deviceOptionsWidget = new DeviceOptionsWidget(this);
    deviceOptionsWidget->setWindowModality(Qt::WindowModality::WindowModal);
    deviceOptionsWidget->setDevice(selectedDevice);
    deviceOptionsWidget->showWIFIPage();
    deviceOptionsWidget->exec();
}

void MainWindow::on_deviceList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item);
    Q_UNUSED(column);
    if (selectedDevice->deviceInfo.status == DeviceStatus::Online) {
        on_deviceButton_clicked();
    }
}

void MainWindow::on_firmwareButton_clicked()
{
    UpdateDeviceDialog *updateDeviceDialog = new UpdateDeviceDialog(this, deviceManager);
    updateDeviceDialog->exec();
}

void MainWindow::updatePrefs() {
    for (int i = 0; i < ui->deviceList->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->deviceList->topLevelItem(i);
        if (item->text(5) == "Offline") {
            item->setHidden(!preferencesManager->showOfflineDevices);
        }
    }
}

void MainWindow::on_actionDeviceName_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(0, !arg1);
    preferencesManager->visibleColumns->DeviceName = arg1;
}

void MainWindow::on_actionFriendlyName_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(1, !arg1);
    preferencesManager->visibleColumns->FriendlyName = arg1;
}

void MainWindow::on_actionIP_Address_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(2, !arg1);
    preferencesManager->visibleColumns->IPAddress = arg1;
}

void MainWindow::on_actionWIFI_Strengt_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(3, !arg1);
    preferencesManager->visibleColumns->WIFIStrength = arg1;
}

void MainWindow::on_actionMAC_Address_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(4, !arg1);
    preferencesManager->visibleColumns->MACAddress = arg1;
}

void MainWindow::on_actionFirmware_Version_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(5, !arg1);
    preferencesManager->visibleColumns->FirmwareVersion = arg1;
}

void MainWindow::on_actionModule_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(6, !arg1);
    preferencesManager->visibleColumns->Module = arg1;
}

void MainWindow::on_actionStatus_toggled(bool arg1)
{
    ui->deviceList->setColumnHidden(7, !arg1);
    preferencesManager->visibleColumns->Status = arg1;
}

void MainWindow::checkForUpdates() {
    if (preferencesManager->versionChannel == 0) {
        softwareUpdate->versionChannel = Stable;
    }
    if (preferencesManager->versionChannel == 1) {
        softwareUpdate->versionChannel = Beta;
    }
    if (preferencesManager->versionChannel == 2) {
        softwareUpdate->versionChannel = Alpha;
    }
    softwareUpdate->getSoftwareUpdates();
}

void MainWindow::on_actionDocumentation_triggered()
{
    QDesktopServices::openUrl(QUrl("https://tom-23.github.io/tasmomanager-docs"));
}

void MainWindow::updateStatusBar()
{
    QString deviceTotalCountString = QString::number(deviceManager->deviceList->size());
    int deviceOnlineCount = 0;
    for (int i = 0; i < deviceManager->deviceList->size(); i++) {
        Device *device = deviceManager->deviceList->at(i);
        if (device->deviceInfo.status == DeviceStatus::Online) {
            deviceOnlineCount += 1;
        }
    }
    QString deviceOnlineCountString = QString::number(deviceOnlineCount);

    ui->statusBarText->setText(deviceTotalCountString + " total device(s), "
                               + deviceOnlineCountString + " online device(s).");
}
