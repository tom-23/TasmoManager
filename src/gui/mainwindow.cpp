#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    deviceManager = new DeviceManager(this);
    serverManager = new MQTTServerManager(this);



    serverManager->loadServerList();

    connect(deviceManager, &DeviceManager::device_Discovered, this, &MainWindow::on_deviceDiscovered);
    connect(deviceManager, &DeviceManager::device_InfoUpdate, this, &MainWindow::on_deviceInfoUpdate);

    connect(deviceManager, &DeviceManager::mqtt_onConnect, this, [=] () {
        deviceManager->subscribeToDiscovery();
        updateInfoText();
    });

    QTimer *errorCollectionTimer = new QTimer(this);
    errorCollectionTimer->setSingleShot(true);

    QList<QMQTT::ClientError> *errorList = new QList<QMQTT::ClientError>;

    connect(deviceManager, &DeviceManager::mqtt_onError, this, [=] (QMQTT::ClientError error) {

        // Only show message box is we are sure MQTT is disconnected.
        if (deviceManager->mqttClient->connectionState() != QMQTT::ConnectionState::STATE_CONNECTED) {

            errorList->append(error);
            if (!errorCollectionTimer->isActive()) {
                errorCollectionTimer->start(500);
            }
        }
    });

    connect(errorCollectionTimer, &QTimer::timeout, this, [=] () {
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

    ui->deviceButton->setVisible(false);
    ui->mqttButton->setVisible(false);
    ui->firmwareButton->setVisible(false);
    ui->backupButton->setVisible(false);
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
}

void MainWindow::on_actiontest_triggered()
{
    SetupDeviceDialog *setupDeviceDialog = new SetupDeviceDialog(this, deviceManager, serverManager);
    setupDeviceDialog->show();
}

void MainWindow::on_actionPreferences_triggered()
{
    preferencesDialog = new PreferencesDialog(this);
    preferencesDialog->setMQTTManager(serverManager);
    preferencesDialog->show();
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

            deviceManager->connect(&selectServerDialog->selectedServer);
            updateInfoText();
        }
    } else {
        deviceManager->disconnect();
        updateInfoText();
    }
}

void MainWindow::on_deviceDiscovered(DeviceInfo deviceInfo) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, deviceInfo.name);
    item->setText(1, deviceInfo.ipAddress.toString());
    item->setText(2, "");
    item->setText(3, deviceInfo.macAddress);
    item->setText(4, "Offline");
    ui->deviceList->addTopLevelItem(item);
    if (ui->deviceList->selectedItems().size() == 0) {
        ui->deviceList->topLevelItem(0)->setSelected(true);
    }
}

void MainWindow::on_deviceInfoUpdate(DeviceInfo deviceInfo) {
    for (int i = 0; i < ui->deviceList->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->deviceList->topLevelItem(i);
        QString itemMac = item->text(3);
        itemMac.replace(":", "");

        QString deviceInfoMac = deviceInfo.macAddress;
        deviceInfoMac.replace(":", "");
        if (deviceInfoMac == itemMac) {
            item->setText(0, deviceInfo.name);
            item->setText(1, deviceInfo.ipAddress.toString());

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
                item->setIcon(2, icon);
                item->setText(2, QString::number(deviceInfo.wifiRSSI) + "% (" + QString::number(deviceInfo.wifiSignal) + " dBm)");
            } else {
                item->setText(2, "n/a");
            }

            item->setText(3, deviceInfo.macAddress);
            item->setText(4, deviceInfo.firmwareVersion);

            if (deviceInfo.status == DeviceStatus::Online) {
                QIcon onlineIcon;
                onlineIcon.addFile(":/16/assets/16_online.svg");
                item->setIcon(5, onlineIcon);
                item->setText(5, "Online");
            } else if (deviceInfo.status == DeviceStatus::Offline) {
                QIcon offlineIcon;
                offlineIcon.addFile(":/16/assets/16_offline.svg");
                item->setIcon(5, offlineIcon);
                item->setText(5, "Offline");
            } else if (deviceInfo.status == DeviceStatus::Restarting) {
                QIcon restartingIcon;
                restartingIcon.addFile(":/16/assets/16_restarting.svg");
                item->setIcon(5, restartingIcon);
                item->setText(5, "Restarting...");
            } else if (deviceInfo.status == DeviceStatus::Updating) {
                QIcon updatingIcon;
                updatingIcon.addFile(":/16/assets/16_updating.svg");
                item->setIcon(5, updatingIcon);
                item->setText(5, "Updating...");
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
    if (current) {
        selectedDevice = deviceManager->getDeviceByMAC(current->text(3));
        ui->deviceInfoWidget->setDevice(selectedDevice);
        ui->devicePowerWidget->setDevice(selectedDevice);
        ui->deviceColorWidget->setDevice(selectedDevice);
         ui->deviceActionsContainer->setEnabled(true);
    } else {
        ui->deviceActionsContainer->setEnabled(false);
    }
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
        ui->userLabel->setText("User: " + deviceManager->mqttServerInfo->username);
        ui->serverLabel->setText("Server: " + deviceManager->mqttServerInfo->name + " (" + deviceManager->mqttServerInfo->ipAddress.toString() + ")");
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

        ui->actiontest->setEnabled(true);
        ui->refreshButton->setEnabled(true);

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

        ui->actiontest->setEnabled(false);

        ui->deviceActionsContainer->setEnabled(false);
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
    DeviceOptionsWidget *deviceOptionsWidget = new DeviceOptionsWidget(this);
    deviceOptionsWidget->setWindowModality(Qt::WindowModality::WindowModal);
    deviceOptionsWidget->exec();
}

void MainWindow::on_restartButton_clicked()
{
    selectedDevice->restart();
}
