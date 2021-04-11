#ifndef UPDATEDEVICEDIALOG_H
#define UPDATEDEVICEDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMovie>
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

private slots:
    void on_cancelButton_clicked();

    void on_nextButton_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_deviceList_itemChanged(QListWidgetItem *item);

    void on_deviceUpdate(DeviceInfo deviceInfo);



    void on_doneButton_clicked();

    void on_actionSelect_Deselect_All_toggled(bool arg1);

private:
    Ui::UpdateDeviceDialog *ui;
    DeviceManager *deviceManager;

    int updatePos;
    int updatePosMax;

    QUrl OTAUrl;

    QList<Device *> *updateDeviceList = new QList<Device *>;

    void updateProgressBar();

    bool errorsEncountered = false;

};

#endif // UPDATEDEVICEDIALOG_H
