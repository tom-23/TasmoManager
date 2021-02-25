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

    updateUI();

    connect(device, &Device::recievedStateUpdate, this, [=] () {
        delayTimer->start();
    });

}

void DevicePowerWidget::updateUI() {
    updatingUI = true;

    ui->power1->setVisible(device->deviceInfo.capabilities.power[0]);
    ui->power1->setChecked(device->deviceInfo.power[0]);

    ui->power2->setVisible(device->deviceInfo.capabilities.power[1]);
    ui->power2->setChecked(device->deviceInfo.power[1]);

    ui->power3->setVisible(device->deviceInfo.capabilities.power[2]);
    ui->power3->setChecked(device->deviceInfo.power[2]);

    ui->power4->setVisible(device->deviceInfo.capabilities.power[3]);
    ui->power4->setChecked(device->deviceInfo.power[3]);

    bool noPower = device->deviceInfo.capabilities.power[0]
            || device->deviceInfo.capabilities.power[1]
            || device->deviceInfo.capabilities.power[2]
            || device->deviceInfo.capabilities.power[3];

    this->setVisible(noPower);

    updatingUI = false;
}

void DevicePowerWidget::on_power1_toggled(bool checked)
{
    device->setPower(1, checked);
}

void DevicePowerWidget::on_power2_toggled(bool checked)
{
    device->setPower(2, checked);
}

void DevicePowerWidget::on_power3_toggled(bool checked)
{
    device->setPower(3, checked);
}

void DevicePowerWidget::on_power4_toggled(bool checked)
{
    device->setPower(4, checked);
}
