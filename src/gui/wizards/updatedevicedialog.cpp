#include "updatedevicedialog.h"
#include "ui_updatedevicedialog.h"

UpdateDeviceDialog::UpdateDeviceDialog(QWidget *parent, DeviceManager *_deviceManager) :
    QDialog(parent),
    ui(new Ui::UpdateDeviceDialog)
{
    ui->setupUi(this);
    deviceManager = _deviceManager;

    QMovie *movie = new QMovie(":/gif/assets/loader.gif");
    movie->setScaledSize(QSize(42, 42));
    ui->spinner->setMovie(movie);
    movie->start();

    ui->stackedWidget->setCurrentIndex(0);
    ui->doneButton->setVisible(false);

    ui->deviceList->addAction(ui->actionSelect_Deselect_All);

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    for (int i = 0; i < deviceManager->deviceList->size(); i++) {
        Device *device = deviceManager->deviceList->at(i);
        if (device->deviceInfo.status == DeviceStatus::Online) {
            QListWidgetItem *item = new QListWidgetItem(ui->deviceList);
            item->setCheckState(Qt::Unchecked);
            item->setData(1, device->deviceInfo.macAddress);
            item->setText(device->deviceInfo.name + " - " + device->deviceInfo.firmwareVersion);
        }
    }
    QFile releasesFile(":/data/tasmota-releases.json");
    if (!releasesFile.open(QIODevice::ReadOnly)) {
        this->reject();
    }
    QJsonDocument doc = QJsonDocument::fromJson(releasesFile.readAll());

    for (int i = 0; i < doc.array().size(); i++) {
        ui->prodOTAFlavour->addItem(doc.array().at(i).toString());
    }
    ui->prodOTAFlavour->setCurrentIndex(0);
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
            if (ui->customOTARadio->isChecked()) {
                OTAUrl = QUrl(ui->customOTAUrl->text());
            } else {
                QString base;
                if (ui->prodOTAVer->currentIndex() == 0) {
                    base = "http://ota.tasmota.com/tasmota/release/";
                } else {
                    base = "http://ota.tasmota.com/tasmota/";
                }
                base = base + ui->prodOTAFlavour->currentText();
                if (ui->gZIPButton->isChecked()) {
                    base = base + ".gz";
                }
                OTAUrl = QUrl(base);
            }
        break;
        case 3:

            ui->nextButton->setText("Start Upgrade");
        break;
        case 4:
            updatePos = 0;
            updatePosMax = 0;
            ui->updateList->clear();
            for (int i = 0; i < ui->deviceList->count(); i++) {
                QListWidgetItem *item = ui->deviceList->item(i);
                if (item->checkState() == Qt::Checked) {
                    qDebug() << item->data(1).toString();
                    Device *device = deviceManager->getDeviceByMAC(item->data(1).toString());
                    updatePosMax = updatePosMax + 3;
                    device->deviceInfo.oldFirmwareVerson = device->deviceInfo.firmwareVersion;

                    if (device->deviceInfo.minimalFirmware) {
                        device->deviceInfo.updateStep = 1;
                    } else {
                        device->deviceInfo.updateStep = 0;
                    }

                    QListWidgetItem *newItem = new QListWidgetItem(ui->updateList);
                    newItem->setText(device->deviceInfo.name);

                    newItem->setData(5, device->deviceInfo.macAddress);
                    ui->updateList->addItem(newItem);


                    device->deviceInfo.OTAUrl = OTAUrl;
                    device->setOTAUrlAndUpgrade();
                    on_deviceUpdate(device->deviceInfo);

                }
            }
            ui->progressBar->setValue(0);
            ui->progressBar->setMaximum(updatePosMax);
            connect(deviceManager, &DeviceManager::device_InfoUpdate, this, &UpdateDeviceDialog::on_deviceUpdate);
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

void UpdateDeviceDialog::on_deviceUpdate(DeviceInfo deviceInfo) {
    for (int i = 0; i < ui->updateList->count(); i++) {
        QListWidgetItem *item = ui->updateList->item(i);
        QString itemMac = item->data(5).toString();
        itemMac.replace(":", "");

        QString deviceInfoMac = deviceInfo.macAddress;
        deviceInfoMac.replace(":", "");
        if (deviceInfoMac == itemMac) {
            Device *device = deviceManager->getDeviceByMAC(itemMac);

            if (deviceInfo.status == DeviceStatus::Online && !deviceInfo.minimalFirmware) {
                QIcon onlineIcon;
                onlineIcon.addFile(":/16/assets/16_online.svg");
                item->setIcon(onlineIcon);
            if (deviceInfo.status == DeviceStatus::Online) {
                if (device->deviceInfo.updateStep == 2) {
                    device->deviceInfo.updateStep = 3;
                }
            }

            } else if (deviceInfo.status == DeviceStatus::Offline && !deviceInfo.minimalFirmware) {
                QIcon offlineIcon;
                offlineIcon.addFile(":/16/assets/16_offline.svg");
                item->setIcon(offlineIcon);
            } else if (deviceInfo.minimalFirmware) {
                QIcon updatingIcon;
                updatingIcon.addFile(":/16/assets/16_updating.svg");
                item->setIcon(updatingIcon);
            }

            if (device->deviceInfo.updateStep != 3) {
                item->setText(device->deviceInfo.name + " - Step " + QString::number(device->deviceInfo.updateStep + 1) + " of 4");
            } else {
                item->setText(device->deviceInfo.name + " - Finished updating.");
            }

            if (device->deviceInfo.updateError != "") {
                QIcon offlineIcon;
                offlineIcon.addFile(":/16/assets/16_offline.svg");
                item->setIcon(offlineIcon);
                errorsEncountered = true;
                item->setText(device->deviceInfo.name + " - " + device->deviceInfo.updateError);
            }

            updateProgressBar();
            return;
        }
    }
}


void UpdateDeviceDialog::updateProgressBar() {
    updatePos = 0;
    updatePosMax = 0;
    for (int i = 0; i < ui->updateList->count(); i++) {
        QListWidgetItem *item = ui->updateList->item(i);
        QString itemMac = item->data(5).toString();
        itemMac.replace(":", "");
        Device *device = deviceManager->getDeviceByMAC(itemMac);
        updatePos = updatePos + device->deviceInfo.updateStep;
        if (device->deviceInfo.updateError == "") {
            updatePosMax = updatePosMax + 3;
        }
    }
    ui->progressBar->setMaximum(updatePosMax);
    ui->progressBar->setValue(updatePos);
    if (updatePos == updatePosMax) {
        if (errorsEncountered) {
            ui->stackedWidget->setCurrentIndex(6);
            ui->cancelButton->setVisible(false);
            ui->doneButton->setVisible(true);
        } else {
            ui->stackedWidget->setCurrentIndex(5);
            ui->cancelButton->setVisible(false);
            ui->doneButton->setVisible(true);
        }
    }
}

void UpdateDeviceDialog::on_doneButton_clicked()
{
    this->accept();
}

void UpdateDeviceDialog::on_actionSelect_Deselect_All_toggled(bool arg1)
{
    for (int i = 0; i < ui->deviceList->count(); i++) {
        if (arg1) {
            ui->deviceList->item(i)->setCheckState(Qt::Checked);
        } else {
            ui->deviceList->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}
