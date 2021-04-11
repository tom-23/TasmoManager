#ifndef SETUPDEVICEDIALOG_H
#define SETUPDEVICEDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QMovie>

#include "app/devicemanager.h"
#include "app/mqttservermanager.h"

namespace Ui {
class SetupDeviceDialog;
}

class SetupDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDeviceDialog(QWidget *parent = nullptr, DeviceManager *_deviceManager = nullptr, MQTTServerManager *_serverManager = nullptr);
    ~SetupDeviceDialog();

private slots:
    void on_cancelButton_clicked();

    void on_nextButton_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_discoveredDeviceList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_topic_textChanged(const QString &arg1);

    void on_fulltopic_textChanged(const QString &arg1);

    void on_mqttServerBox_currentIndexChanged(const QString &arg1);

    void on_doneButton_clicked();

private:
    Ui::SetupDeviceDialog *ui;

    DeviceManager *deviceManager;
    MQTTServerManager *serverManager;

    QList<DeviceInfo> *discoveredDeviceList;
    DeviceInfo currentDevice;
    DeviceInfo deviceSettings;

    void mqttDetailsFilled();

    int countDown = 20;

    QTimer *timeoutTimer;
};

#endif // SETUPDEVICEDIALOG_H
