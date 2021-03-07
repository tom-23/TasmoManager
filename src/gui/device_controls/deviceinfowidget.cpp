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
    if (device != nullptr) {
        lastDeviceName = device->deviceInfo.name;
        ui->deviceName->setText(device->deviceInfo.name);
        lastDeviceFriendlyName = device->deviceInfo.friendlyName;
        ui->deviceFriendlyName->setText(device->deviceInfo.friendlyName);
        ui->ipAddress->setText(device->deviceInfo.ipAddress.toString());
        ui->macAddress->setText(device->deviceInfo.macAddress);

        ui->saveButton->setVisible(false);
        ui->revertButton->setVisible(false);
        this->setVisible(true);
    } else {
        this->setVisible(false);
    }
}

void DeviceInfoWidget::on_deviceName_textChanged(const QString &arg1)
{
    qDebug() << lastDeviceName;
    qDebug() << arg1;
    deviceNameChanged = (lastDeviceName != arg1);
    updateButtonVisibility();
}

void DeviceInfoWidget::on_deviceFriendlyName_textChanged(const QString &arg1)
{
    deviceFriendlyNameChanged = (lastDeviceFriendlyName != arg1);
    updateButtonVisibility();
}

void DeviceInfoWidget::updateButtonVisibility() {
    ui->saveButton->setVisible(deviceNameChanged | deviceFriendlyNameChanged);
    ui->revertButton->setVisible(deviceNameChanged | deviceFriendlyNameChanged);
}

void DeviceInfoWidget::on_saveButton_clicked()
{
    device->setDeviceNames(ui->deviceName->text(), ui->deviceFriendlyName->text());
}

void DeviceInfoWidget::on_revertButton_clicked()
{
    ui->deviceName->setText(lastDeviceName);
    ui->deviceFriendlyName->setText(lastDeviceFriendlyName);
}
