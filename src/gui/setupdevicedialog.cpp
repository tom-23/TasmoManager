#include "setupdevicedialog.h"
#include "ui_setupdevicedialog.h"

SetupDeviceDialog::SetupDeviceDialog(QWidget *parent, DeviceManager *_deviceManager, MQTTServerManager *_serverManager) :
    QDialog(parent),
    ui(new Ui::SetupDeviceDialog)
{
    ui->setupUi(this);

    QMovie *movie = new QMovie(":/gif/assets/loader.gif");
    movie->setScaledSize(QSize(42, 42));
    ui->spinner->setMovie(movie);
    movie->start();

    deviceManager = _deviceManager;
    serverManager = _serverManager;

    for (int i = 0; i < serverManager->serverList->size(); i++) {
        ui->mqttServerBox->addItem(serverManager->serverList->at(i).name);
    }
    ui->mqttServerBox->setCurrentIndex(0);

    ui->stackedWidget->setCurrentIndex(0);
    ui->doneButton->setVisible(false);

    discoveredDeviceList = new QList<DeviceInfo>;

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(1000);

    connect(timeoutTimer, &QTimer::timeout, this, [=] () {
        ui->timeoutLabel->setText("Timeout in: " + QString::number(countDown) + "s...");
        countDown = countDown - 1;
        if (countDown == 0) {
            timeoutTimer->stop();
            ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count() - 1);
            ui->doneButton->setVisible(true);
            ui->cancelButton->setVisible(false);
            ui->nextButton->setVisible(false);

        }
    });

    connect(deviceManager, &DeviceManager::scanNewDevices_Progress, this, [=] (int progress, int max) {
        ui->scanStatusLabel->setText("Scanning Address " + QString::number(progress) + " of " + QString::number(max) + "...");
    });

    connect(deviceManager, &DeviceManager::scanNewDevices_NewDeviceFound, this, [=] (DeviceInfo deviceInfo) {
        QListWidgetItem *item = new QListWidgetItem(ui->discoveredDeviceList);
        item->setText("Tasmota (" + deviceInfo.name + ") MAC: " + deviceInfo.macAddress);
        ui->discoveredDeviceList->addItem(item);
        discoveredDeviceList->append(deviceInfo);
    });

    connect(deviceManager, &DeviceManager::setupNewDevices_Progress, this, [=] (int progress, int amount, QString status) {
        ui->statusLabel->setText("(Step " + QString::number(progress) + " of " + QString::number(amount) + ") " + status);
        if (progress == amount) {
            timeoutTimer->stop();
            ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count() - 2);
            ui->doneButton->setVisible(true);
            ui->cancelButton->setVisible(false);
            ui->nextButton->setVisible(false);
        }
    });
}

SetupDeviceDialog::~SetupDeviceDialog()
{
    delete ui;
}

void SetupDeviceDialog::on_cancelButton_clicked()
{
    auto m = new QMessageBox(this);
    m->setText("Are you sure you want to cancel setup?");
    m->setInformativeText("You will loose changes.");
    m->setIcon(QMessageBox::Warning);
    m->setWindowModality(Qt::WindowModal);
    m->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (m->exec() == QMessageBox::Yes) {
        this->close();
    };
}

void SetupDeviceDialog::on_nextButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
}

void SetupDeviceDialog::on_stackedWidget_currentChanged(int arg1)
{
    switch (arg1) {
        case 1:
            ui->nextButton->setEnabled(false);
            ui->discoveredDeviceList->clear();
            deviceManager->scanNewDevices();
        break;
        case 2:
            currentDevice = discoveredDeviceList->at(ui->discoveredDeviceList->currentRow());
        break;
        case 3:


            deviceSettings.name = ui->deviceName->text();
            deviceSettings.friendlyName = ui->deviceFriendlyName->text();
            deviceSettings.moduleTemplate = ui->deviceTemplate->text();

            ui->nextButton->setEnabled(false);
        break;
        case 4:

            deviceSettings.mqttServer = serverManager->serverList->at(ui->mqttServerBox->currentIndex());
            deviceSettings.mqttTopic = ui->topic->text();
            deviceSettings.mqttFullTopic = ui->fulltopic->text();

            deviceManager->setupNewDevice(currentDevice, deviceSettings);
            countDown = 20;
            timeoutTimer->start();

            ui->nextButton->setVisible(false);
        break;
    }
}

void SetupDeviceDialog::on_discoveredDeviceList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current != nullptr) {
        ui->nextButton->setEnabled(true);
    }
}

void SetupDeviceDialog::on_topic_textChanged(const QString &arg1)
{
    mqttDetailsFilled();
}

void SetupDeviceDialog::on_fulltopic_textChanged(const QString &arg1)
{
    mqttDetailsFilled();
}

void SetupDeviceDialog::on_mqttServerBox_currentIndexChanged(const QString &arg1)
{
    mqttDetailsFilled();
}

void SetupDeviceDialog::mqttDetailsFilled() {
    ui->nextButton->setEnabled((ui->topic->text() != "") && (ui->fulltopic->text() != "") && (ui->mqttServerBox->currentText() != ""));
}

void SetupDeviceDialog::on_doneButton_clicked()
{
    this->accept();
}
