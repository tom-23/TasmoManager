#include "fullinfowidget.h"
#include "ui_fullinfowidget.h"

FullInfoWidget::FullInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FullInfoWidget)
{
    ui->setupUi(this);

}

FullInfoWidget::~FullInfoWidget()
{
    delete ui;
}

void FullInfoWidget::updateInfo() {
    ui->firmwareVersion->setText(device->deviceInfo.firmwareVersion);
    ui->buildDateTime->setText(device->deviceInfo.buildDateAndTime);
    ui->coreSDKVersion->setText(device->deviceInfo.coreSDKVersion);
    ui->deviceName->setText(device->deviceInfo.name);
    ui->friendlyName->setText(device->deviceInfo.friendlyName);
    ui->chipName->setText(device->deviceInfo.hardware);
    ui->clockSpeed->setText(QString::number(device->deviceInfo.cpuFreq) + "hZ");
    ui->ipAddress->setText(device->deviceInfo.ipAddress.toString());
    ui->subnetMask->setText(device->deviceInfo.subnetMask.toString());
    ui->macAddress->setText(device->deviceInfo.macAddress);
    ui->dnsServer->setText(device->deviceInfo.dnsServer.toString());
    ui->gateWay->setText(device->deviceInfo.gateway.toString());
}
