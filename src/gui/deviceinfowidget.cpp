#include "deviceinfowidget.h"
#include "ui_deviceinfowidget.h"

DeviceInfoWidget::DeviceInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceInfoWidget)
{
    ui->setupUi(this);
    this->setVisible(false);
}

DeviceInfoWidget::~DeviceInfoWidget()
{
    delete ui;
}

void DeviceInfoWidget::setDevice(Device *_device) {

    device = _device;
    ui->deviceName->setText(device->deviceInfo.name);
    ui->deviceFriendlyName->setText(device->deviceInfo.friendlyName);
    ui->ipAddress->setText(device->deviceInfo.ipAddress.toString());
    ui->macAddress->setText(device->deviceInfo.macAddress);
    this->setVisible(true);
}

void DeviceInfoWidget::on_saveChangesButton_clicked()
{
    device->setDeviceNames(ui->deviceName->text(), ui->deviceFriendlyName->text());
}
