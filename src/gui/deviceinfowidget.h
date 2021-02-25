#ifndef DEVICEINFOWIDGET_H
#define DEVICEINFOWIDGET_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class DeviceInfoWidget;
}

class DeviceInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceInfoWidget(QWidget *parent = nullptr);
    ~DeviceInfoWidget();

    void setDevice(Device *device);

private slots:
    void on_saveChangesButton_clicked();

private:
    Ui::DeviceInfoWidget *ui;

    Device *device;
};

#endif // DEVICEINFOWIDGET_H
