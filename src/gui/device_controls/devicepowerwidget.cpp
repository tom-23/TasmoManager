#include "devicepowerwidget.h"
#include "ui_devicepowerwidget.h"

DevicePowerWidget::DevicePowerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevicePowerWidget)
{
    ui->setupUi(this);

    this->setVisible(false);

    delayTimer = new QTimer(this);
    delayTimer->setSingleShot(true);
    delayTimer->setInterval(500);

    connect(delayTimer, &QTimer::timeout, this, [=] () {
        updateUI();
    });
}

DevicePowerWidget::~DevicePowerWidget()
{
    delete ui;
}

void DevicePowerWidget::setDevice(Device *_device) {

    device = _device;

    if (device != nullptr) {
        updateUI();

        connect(device, &Device::recievedStateUpdate, this, [=] () {
            delayTimer->start();
        });
    } else {
        this->setVisible(false);
    }
}

void DevicePowerWidget::updateUI() {
    qDebug() << "updating power";
    if (!device) {
        return;
    }
    updatingUI = true;

    for (int i = 0; i < device->deviceInfo.capabilities.power.size(); i++) {
        QPushButton *pushButton = this->findChild<QPushButton*>("power" + QString::number(i + 1));
        if (pushButton) {
            pushButton->setVisible(device->deviceInfo.capabilities.power[i]);
            pushButton->setChecked(device->deviceInfo.power[i]);
        }
    }

    this->setVisible(device->deviceInfo.capabilities.power[0]);

    updatingUI = false;
}

void DevicePowerWidget::on_power1_toggled(bool checked)
{
    if (updatingUI) {
        return;
    }
    device->setPower(1, checked);
}

void DevicePowerWidget::on_power2_toggled(bool checked)
{
    if (updatingUI) {
        return;
    }
    device->setPower(2, checked);
}

void DevicePowerWidget::on_power3_toggled(bool checked)
{
    if (updatingUI) {
        return;
    }
    device->setPower(3, checked);
}

void DevicePowerWidget::on_power4_toggled(bool checked)
{
    if (updatingUI) {
        return;
    }
    device->setPower(4, checked);
}

void DevicePowerWidget::on_power5_toggled(bool checked)
{
    if (updatingUI) {
        return;
    }
    device->setPower(5, checked);
}

void DevicePowerWidget::on_power6_toggled(bool checked)
{
    if (updatingUI) {
        return;
    }
    device->setPower(6, checked);
}
