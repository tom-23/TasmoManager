#include "devicemqttserverswidget.h"
#include "ui_devicemqttserverswidget.h"

DeviceMQTTServersWidget::DeviceMQTTServersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceMQTTServersWidget)
{
    ui->setupUi(this);
    ui->revertButton->setVisible(false);
    ui->saveButton->setVisible(false);
    QMovie *movie = new QMovie(":/gif/assets/light_loader.gif");
    movie->setScaledSize(QSize(16, 16));
    ui->spinner->setMovie(movie);
    movie->start();
    ui->spinner->setVisible(false);
    ui->fullTopic->addAction(ui->actionInsert_prefix);
    ui->fullTopic->addAction(ui->actionInsert_Topic);
}

DeviceMQTTServersWidget::~DeviceMQTTServersWidget()
{
    delete ui;
}

void DeviceMQTTServersWidget::setDevice(Device *_device) {
    device = _device;
    connect(device, &Device::recievedMQTTInfoUpdate, this, [=] () {
        commandCount = commandCount + 1;
        refreshInfo();
        if (commandCount == 7) {
            ui->spinner->setVisible(false);
            this->setEnabled(true);
        }
    });
    commandCount = 0;
    device->getMQTTSettings();
    this->setEnabled(false);
    ui->spinner->setVisible(true);
    refreshInfo();
}

void DeviceMQTTServersWidget::refreshInfo() {
    if (this->isEnabled() == false) {
        ui->topic->setText(device->deviceInfo.mqttTopic);
        ui->fullTopic->setText(device->deviceInfo.mqttFullTopic);
        ui->clientName->setText(device->deviceInfo.mqttClient);

        ui->serverList->clear();
        mqttSeverInfo = *device->deviceInfo.mqttServer;
        QString ip;
        if (!device->deviceInfo.mqttServer->ipAddress.isNull()) {
            ip = device->deviceInfo.mqttServer->ipAddress.toString();
        } else {
            ip = device->deviceInfo.mqttServer->host;
        }
        ui->serverList->addItem(device->deviceInfo.mqttServer->name + " (" + device->deviceInfo.mqttServer->username + "@" +
                                ip + ":" +
                                QString::number(device->deviceInfo.mqttServer->port) + ")");
        ui->saveButton->setVisible(false);
        ui->revertButton->setVisible(false);
    }
}

void DeviceMQTTServersWidget::on_editButton_clicked()
{
    EditServerDialog *editServerDialog = new EditServerDialog(this);
    editServerDialog->setWindowModality(Qt::WindowModal);
    editServerDialog->setMQTTServer(&mqttSeverInfo);
    if (editServerDialog->exec() == 1) {
        ui->serverList->clear();
        QString ip;
        if (!mqttSeverInfo.ipAddress.isNull()) {
            ip = mqttSeverInfo.ipAddress.toString();
        } else {
            ip = mqttSeverInfo.host;
        }
        ui->serverList->addItem(mqttSeverInfo.name + " (" + mqttSeverInfo.username + "@" +
                                ip + ":" +
                                QString::number(mqttSeverInfo.port) + ")");
    };
}


void DeviceMQTTServersWidget::on_actionInsert_prefix_triggered()
{
    ui->fullTopic->insert("%prefix%");
}

void DeviceMQTTServersWidget::on_actionInsert_Topic_triggered()
{
    ui->fullTopic->insert("%topic%");
}

void DeviceMQTTServersWidget::valueChanged() {
    ui->saveButton->setVisible(true);
    ui->revertButton->setVisible(true);
}

void DeviceMQTTServersWidget::on_revertButton_clicked()
{
    this->setEnabled(false);
    refreshInfo();
    this->setEnabled(true);
}

void DeviceMQTTServersWidget::on_serverList_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void DeviceMQTTServersWidget::on_clientName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void DeviceMQTTServersWidget::on_topic_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void DeviceMQTTServersWidget::on_fullTopic_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void DeviceMQTTServersWidget::on_saveButton_clicked()
{
    device->deviceInfo.mqttTopic = ui->topic->text();
    device->deviceInfo.mqttFullTopic = ui->fullTopic->text();
    device->deviceInfo.mqttClient = ui->clientName->text();
    device->deviceInfo.mqttServer = new MQTTServerInfo(mqttSeverInfo);
    ui->revertButton->setVisible(false);
    ui->saveButton->setVisible(false);
    device->setMQTTSettings();
}
