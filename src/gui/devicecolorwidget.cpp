#include "devicecolorwidget.h"
#include "ui_devicecolorwidget.h"

DeviceColorWidget::DeviceColorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceColorWidget)
{
    ui->setupUi(this);
    this->setVisible(false);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(500);
    connect(updateTimer, &QTimer::timeout, this, &DeviceColorWidget::updateColor);
}

DeviceColorWidget::~DeviceColorWidget()
{
    delete ui;
}

void DeviceColorWidget::on_hue_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Color] Hue:" << value;
    }
}

void DeviceColorWidget::on_saturation_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Color] Saturation:" << value;
    }
}

void DeviceColorWidget::on_brightness_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Color] Brightness:" << value;
    }
}

void DeviceColorWidget::on_colorTemp_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Color] Color Temp:" << value;
    }
}

void DeviceColorWidget::updateColor() {
    if (device != nullptr) {
        QColor color = QColor::fromHsv(ui->hue->value(), ui->saturation->value(), ui->brightness->value());

        if (previousColor != color) {
            previousColor = color;
            device->setColor(color);
        }

        if (previousCT != ui->colorTemp->value()) {
            previousCT = ui->colorTemp->value();
            device->setColorTemp(ui->colorTemp->value());
        }

        if ((previousCT == ui->colorTemp->value()) && (previousColor == color)) {
            updateTimer->stop();
        }
    } else {
        updateTimer->stop();
    }
}

void DeviceColorWidget::updateUI() {

    if (!updateTimer->isActive()) {
        updatingUI = true;
        int h = device->deviceInfo.color.hue();
        int s = device->deviceInfo.color.saturation();
        int v = device->deviceInfo.color.value();

        ui->hue->setValue(h);
        ui->saturation->setValue(s);
        ui->brightness->setValue(v);

        ui->colorTemp->setValue(device->deviceInfo.colorTemp);

        updatingUI = false;
    }
}

void DeviceColorWidget::setDevice(Device *_device) {

    device = _device;

    ui->brightness->setVisible(device->deviceInfo.capabilities.dimmer);

    ui->hue->setVisible(device->deviceInfo.capabilities.color);
    ui->saturation->setVisible(device->deviceInfo.capabilities.color);

    ui->colorTemp->setVisible(device->deviceInfo.capabilities.colorTemp);

    if (device->deviceInfo.capabilities.colorTemp || device->deviceInfo.capabilities.color || device->deviceInfo.capabilities.dimmer) {
        this->setVisible(true);
    } else {
        this->setVisible(false);
    }

    updateUI();

    connect(device, &Device::recievedStateUpdate, this, &DeviceColorWidget::updateUI);
}
