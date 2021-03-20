#include "updatedevicedialog.h"
#include "ui_updatedevicedialog.h"

UpdateDeviceDialog::UpdateDeviceDialog(QWidget *parent, DeviceManager *_deviceManager) :
    QDialog(parent),
    ui(new Ui::UpdateDeviceDialog)
{
    ui->setupUi(this);
    deviceManager = _deviceManager;

    ui->stackedWidget->setCurrentIndex(0);
    ui->doneButton->setVisible(false);

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    for (int i = 0; i < deviceManager->deviceList->size(); i++) {
        Device *device = deviceManager->deviceList->at(i);
        if (device->deviceInfo.status == DeviceStatus::Online) {
            QListWidgetItem *item = new QListWidgetItem(ui->deviceList);
            item->setCheckState(Qt::Unchecked);
            item->setData(0, QVariant::fromValue(device->deviceInfo.macAddress));
            item->setText(device->deviceInfo.name + " - " + device->deviceInfo.firmwareVersion);
        }
    }

}

UpdateDeviceDialog::~UpdateDeviceDialog()
{
    delete ui;
}

void UpdateDeviceDialog::on_cancelButton_clicked()
{
    auto m = new QMessageBox(this);
    m->setText("Are you sure you want to cancel firmware update?");
    m->setInformativeText("You will loose changes.");
    m->setIcon(QMessageBox::Warning);
    m->setWindowModality(Qt::WindowModal);
    m->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (m->exec() == QMessageBox::Yes) {
        this->close();
    };
}

void UpdateDeviceDialog::on_nextButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
}

void UpdateDeviceDialog::on_stackedWidget_currentChanged(int arg1)
{
    switch (arg1) {
        case 1:
            ui->nextButton->setEnabled(false);
        break;
        case 2:
            updateDeviceList->clear();
            for (int i = 0; i < ui->deviceList->count(); i++) {
                QListWidgetItem *item = ui->deviceList->item(i);
                if (item->checkState() == Qt::Checked) {
                    Device *device = deviceManager->getDeviceByMAC(item->data(0).toString());
                    updateDeviceList->append(device);
                }
            }
        break;
        case 3:

            ui->nextButton->setText("Start Upgrade");
        break;
        case 4:
            ui->nextButton->setText("Next");
            ui->nextButton->setVisible(false);
        break;
    }
}

void UpdateDeviceDialog::on_deviceList_itemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item);
    bool itemIsChecked = false;
    for (int i = 0; i < ui->deviceList->count(); i++) {
        if (ui->deviceList->item(i)->checkState() == Qt::Checked) {
            itemIsChecked = true;
        }
    }
    if (ui->stackedWidget->currentIndex() == 1) {
        ui->nextButton->setEnabled(itemIsChecked);
    }
}
