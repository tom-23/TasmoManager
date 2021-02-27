#ifndef DEVICEPOWERWIDGET_H
#define DEVICEPOWERWIDGET_H

#include <QWidget>

#include "mainwindow.h"

namespace Ui {
class DevicePowerWidget;
}

class DevicePowerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DevicePowerWidget(QWidget *parent = nullptr);
    ~DevicePowerWidget();

    void setDevice(Device *device);

private slots:
    void on_power1_toggled(bool checked);

    void on_power2_toggled(bool checked);

    void on_power3_toggled(bool checked);

    void on_power4_toggled(bool checked);

    void on_power5_toggled(bool checked);

    void on_power6_toggled(bool checked);

private:
    Ui::DevicePowerWidget *ui;

    Device *device;

    void updateUI();

    bool updatingUI = false;

    QTimer *delayTimer;
};

#endif // DEVICEPOWERWIDGET_H
