#ifndef DEVICECOLORWIDGET_H
#define DEVICECOLORWIDGET_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class DeviceColorWidget;
}

class DeviceColorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceColorWidget(QWidget *parent = nullptr);
    ~DeviceColorWidget();

    void setDevice(Device *device);

private slots:
    void on_hue_valueChanged(int value);

    void on_saturation_valueChanged(int value);

    void on_brightness_valueChanged(int value);

    void on_colorTemp_valueChanged(int value);

private:
    Ui::DeviceColorWidget *ui;

    Device *device;

    void updateColor();

    void updateUI();

    QTimer *updateTimer;

    QColor previousColor;
    int previousCT;

    bool updatingUI = false;
};

#endif // DEVICECOLORWIDGET_H
