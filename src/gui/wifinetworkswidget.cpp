#include "wifinetworkswidget.h"
#include "ui_wifinetworkswidget.h"

WifiNetworksWidget::WifiNetworksWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WifiNetworksWidget)
{
    ui->setupUi(this);
    ui->ipSettingsWidget->setVisible(false);
}

WifiNetworksWidget::~WifiNetworksWidget()
{
    delete ui;
}

void WifiNetworksWidget::on_staticIP_currentIndexChanged(int index)
{
    ui->ipSettingsWidget->setVisible(index == 1);
}

void WifiNetworksWidget::on_currentAP_currentIndexChanged(int index)
{
    ui->apTabs->setCurrentIndex(index);
}

void WifiNetworksWidget::setDevice(Device *_device) {
    device = _device;
}

void WifiNetworksWidget::refreshInfo() {
    ui->ap1SSID->setText(device->deviceInfo.wifiSSID);
    ui->ap1Password->setText("");
}
