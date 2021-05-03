#ifndef DEVICEPOWERWIDGET_H
#define DEVICEPOWERWIDGET_H

#include <QWidget>

#include "../mainwindow.h"

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

private:
    Ui::DevicePowerWidget *ui;

    Device *device;

    void updateUI();

    bool updatingUI = false;

    QTimer *delayTimer;
};

#endif // DEVICEPOWERWIDGET_H
