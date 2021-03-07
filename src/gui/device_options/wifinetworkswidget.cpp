#include "wifinetworkswidget.h"
#include "ui_wifinetworkswidget.h"

WifiNetworksWidget::WifiNetworksWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WifiNetworksWidget)
{
    ui->setupUi(this);
    ui->ipSettingsWidget->setVisible(false);
    QMovie *movie = new QMovie(":/gif/assets/light_loader.gif");
    movie->setScaledSize(QSize(16, 16));
    ui->spinner->setMovie(movie);
    movie->start();
    ui->spinner->setVisible(false);
}

WifiNetworksWidget::~WifiNetworksWidget()
{
    delete ui;
}

void WifiNetworksWidget::on_staticIP_currentIndexChanged(int index)
{
    ui->ipSettingsWidget->setVisible(index == 1);
    valueChanged();
}

void WifiNetworksWidget::on_currentAP_currentIndexChanged(int index)
{
    ui->apTabs->setCurrentIndex(index);
    valueChanged();
}

void WifiNetworksWidget::setDevice(Device *_device) {
    device = _device;
    connect(device, &Device::recievedWifiInfoUpdate, this, [=] () {
        commandCount = commandCount + 1;
        refreshInfo();
        if (commandCount == 5) {
            ui->spinner->setVisible(false);
            this->setEnabled(true);
        }
    });
    commandCount = 0;
    device->getWifiNetworkSettings();
    this->setEnabled(false);
    ui->spinner->setVisible(true);
    refreshInfo();
}

void WifiNetworksWidget::refreshInfo() {
    if (this->isEnabled() == false) {
        ui->currentAP->setCurrentIndex(device->deviceInfo.activeAP - 1);
        ui->ap1SSID->setText(device->deviceInfo.ap1SSID);
        ui->ap1Password->setText("");
        ui->ap2SSID->setText(device->deviceInfo.ap2SSID);
        ui->ap2Password->setText("");
        ui->ipAddress->setText(device->deviceInfo.ipAddress.toString());
        ui->gateway->setText(device->deviceInfo.gateway.toString());
        ui->dnsServer->setText(device->deviceInfo.dnsServer.toString());
        ui->subnetMask->setText(device->deviceInfo.subnetMask.toString());
        if (device->deviceInfo.useStaticIP) {
            ui->staticIP->setCurrentIndex(1);
        } else {
            ui->staticIP->setCurrentIndex(0);
        }
        ui->saveButton->setVisible(false);
        ui->revertButton->setVisible(false);
    }
}

void WifiNetworksWidget::valueChanged() {
    ui->saveButton->setVisible(true);
    ui->revertButton->setVisible(true);
}

void WifiNetworksWidget::on_revertButton_clicked()
{
    this->setEnabled(false);
    refreshInfo();
    this->setEnabled(true);
}

void WifiNetworksWidget::on_ap1SSID_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_ap1Password_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_ap2SSID_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_ap2Password_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_ipAddress_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_gateway_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_subnetMask_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_dnsServer_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    valueChanged();
}

void WifiNetworksWidget::on_saveButton_clicked()
{
    if (QHostAddress(ui->ipAddress->text()).isNull()
            || QHostAddress(ui->gateway->text()).isNull()
            || QHostAddress(ui->subnetMask->text()).isNull()
            || QHostAddress(ui->dnsServer->text()).isNull()) {
        auto m = new QMessageBox(this);
        m->setText("One or more IP addresses are invalid");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }
    device->deviceInfo.activeAP = ui->currentAP->currentIndex() + 1;
    device->deviceInfo.ap1SSID = ui->ap1SSID->text();
    device->deviceInfo.ap1Password = ui->ap1Password->text();
    device->deviceInfo.ap2SSID = ui->ap2SSID->text();
    device->deviceInfo.useStaticIP = (ui->staticIP->currentIndex() == 1);
    if (device->deviceInfo.useStaticIP) {
        device->deviceInfo.ipAddress = QHostAddress(ui->ipAddress->text());
        device->deviceInfo.gateway = QHostAddress(ui->gateway->text());
        device->deviceInfo.subnetMask = QHostAddress(ui->subnetMask->text());
        device->deviceInfo.dnsServer = QHostAddress(ui->dnsServer->text());
    }
    ui->revertButton->setVisible(false);
    ui->saveButton->setVisible(false);
    device->setWifiNetworkSettings();
}
