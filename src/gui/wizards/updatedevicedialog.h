#ifndef UPDATEDEVICEDIALOG_H
#define UPDATEDEVICEDIALOG_H

#include <QDialog>
#include "app/devicemanager.h"

namespace Ui {
class UpdateDeviceDialog;
}

class UpdateDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDeviceDialog(QWidget *parent = nullptr, DeviceManager *_deviceManager = nullptr);
    ~UpdateDeviceDialog();

private:
    Ui::UpdateDeviceDialog *ui;
    DeviceManager *deviceManager;
};

#endif // UPDATEDEVICEDIALOG_H
