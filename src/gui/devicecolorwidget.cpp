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

void DeviceColorWidget::setDevice(Device *_device) {

    device = _device;

    if (device != nullptr) {
        previousChannels.clear();
        previousChannels.append(device->deviceInfo.channels);

        updateUI();

        connect(device, &Device::recievedStateUpdate, this, &DeviceColorWidget::updateUI);
    } else {
        this->setVisible(false);
    }


}

void DeviceColorWidget::updateColor() {

    if (device != nullptr) {
        QColor color = QColor::fromHsv(ui->hue->value(), ui->saturation->value(), ui->brightness->value());

        if (device->deviceInfo.capabilities.color == false && device->deviceInfo.capabilities.dimmer == true) {
            previousDimmer = ui->brightness->value();
            device->setDimmer(ui->brightness->value());
        }

        if (previousColor != color) {
            previousColor = color;
            device->setColor(color);
        }

        if (previousCT != ui->colorTemp->value()) {
            previousCT = ui->colorTemp->value();
            device->setColorTemp(ui->colorTemp->value());
        }

        bool noChannelsChanged = true;

        for (int i = 0; i < device->deviceInfo.capabilities.channels.size(); i++) {
            QSlider *slider = this->findChild<QSlider*>("channel" + QString::number(i + 1));
            if (slider) {
                if (previousChannels[i] != slider->value()) {
                    noChannelsChanged = false;
                    previousChannels[i] = slider->value();
                    device->setChannel(i + 1, slider->value());
                }
            }
        }

        if ((previousCT == ui->colorTemp->value()) && (previousColor == color) && noChannelsChanged) {
            updateTimer->stop();
        }

    } else {
        updateTimer->stop();
    }
}

void DeviceColorWidget::updateUI() {

    if (!device) {
        return;
    }
    // Only show ui objects which can be controlled.

    bool displayWidget = device->deviceInfo.capabilities.colorTemp
            || device->deviceInfo.capabilities.color
            || device->deviceInfo.capabilities.dimmer
            || device->deviceInfo.capabilities.channels[0];


    ui->brightness->setVisible(device->deviceInfo.capabilities.dimmer);

    ui->hue->setVisible(device->deviceInfo.capabilities.color);
    ui->saturation->setVisible(device->deviceInfo.capabilities.color);

    ui->colorTemp->setVisible(device->deviceInfo.capabilities.colorTemp);

    ui->channel1->setVisible(device->deviceInfo.capabilities.channels[0]);
    ui->channel2->setVisible(device->deviceInfo.capabilities.channels[1]);
    ui->channel3->setVisible(device->deviceInfo.capabilities.channels[2]);
    ui->channel4->setVisible(device->deviceInfo.capabilities.channels[3]);
    ui->channel5->setVisible(device->deviceInfo.capabilities.channels[4]);


    if (displayWidget) {
        this->setVisible(true);
    } else {
        this->setVisible(false);
    }

    if (!updateTimer->isActive()) {
        updatingUI = true;
        int h = device->deviceInfo.color.hue();
        int s = device->deviceInfo.color.saturation();
        int v = device->deviceInfo.color.value();

        ui->hue->setValue(h);
        ui->saturation->setValue(s);
        ui->brightness->setValue(v);

        ui->colorTemp->setValue(device->deviceInfo.colorTemp);

        for (int i = 0; i < device->deviceInfo.capabilities.channels.size(); i++) {
            QSlider *slider = this->findChild<QSlider*>("channel" + QString::number(i + 1));
            if (slider) {
                slider->setValue(device->deviceInfo.channels[i]);
            }
        }
        updatingUI = false;
    }
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

void DeviceColorWidget::on_channel1_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Dimmers] Dimmer CH1:" << value;
    }
}

void DeviceColorWidget::on_channel2_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Dimmers] Dimmer CH2:" << value;
    }
}

void DeviceColorWidget::on_channel3_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Dimmers] Dimmer CH3:" << value;
    }
}

void DeviceColorWidget::on_channel4_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Dimmers] Dimmer CH4:" << value;
    }
}

void DeviceColorWidget::on_channel5_valueChanged(int value)
{
    if (!updateTimer->isActive() && !updatingUI) {
        updateTimer->start();
        qDebug() << "[Dimmers] Dimmer CH5:" << value;
    }
}
