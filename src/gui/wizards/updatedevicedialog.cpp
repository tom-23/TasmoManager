#include "updatedevicedialog.h"
#include "ui_updatedevicedialog.h"

UpdateDeviceDialog::UpdateDeviceDialog(QWidget *parent, DeviceManager *_deviceManager) :
    QDialog(parent),
    ui(new Ui::UpdateDeviceDialog)
{
    ui->setupUi(this);
    deviceManager = _deviceManager;
}

UpdateDeviceDialog::~UpdateDeviceDialog()
{
    delete ui;
}
