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

    void on_deviceName_textChanged(const QString &arg1);

    void on_deviceFriendlyName_textChanged(const QString &arg1);

    void on_saveButton_clicked();

    void on_revertButton_clicked();

private:
    Ui::DeviceInfoWidget *ui;

    Device *device;
    QString lastDeviceName;
    bool deviceNameChanged = false;
    QString lastDeviceFriendlyName;
    bool deviceFriendlyNameChanged = false;

    void updateButtonVisibility();
};

#endif // DEVICEINFOWIDGET_H
