#ifndef DEVICEOPTIONSWIDGET_H
#define DEVICEOPTIONSWIDGET_H

#include <QDialog>
#include <QMovie>
#include <QListWidgetItem>
#include <QSpacerItem>
#include "app/devicemanager.h"
#include "setoptionwidget.h"
#include "wifinetworkswidget.h"

namespace Ui {
class DeviceOptionsWidget;
}

class DeviceOptionsWidget : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceOptionsWidget(QWidget *parent = nullptr);
    ~DeviceOptionsWidget();
    void showMQTTPage();
    void showWIFIPage();

    QString deviceInfoMac;

    void setDevice(Device *_device) {
        device = _device;
        deviceInfoMac = device->deviceInfo.macAddress;
        deviceInfoMac.replace(":", "");
        initalizeUI();
    };


private slots:
    void on_cancelButton_clicked();
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_showSetOptionsButton_toggled(bool checked);

    void on_saveChangesButton_clicked();

    void on_refreshUIButton_clicked();

private:
    Ui::DeviceOptionsWidget *ui;


    QList<SetOption*> *generalSetOptions = new QList<SetOption*>;
    QList<SetOption*> *buttonsSetOptions = new QList<SetOption*>;
    QList<SetOption*> *lightingSetOptions = new QList<SetOption*>;
    QList<SetOption*> *temperatureSetOptions = new QList<SetOption*>;
    QList<SetOption*> *wifiSetOptions = new QList<SetOption*>;
    QList<SetOption*> *mqttSetOptions = new QList<SetOption*>;
    QList<SetOption*> *irRfSetOptions = new QList<SetOption*>;
    QList<SetOption*> *miscSetOptions = new QList<SetOption*>;

    QList<SetOptionWidget*> *setOptionWidgetList = new QList<SetOptionWidget*>;

    void initalizeUI();

    bool updateCurrentSetOptions();
    bool updateCurrentInfo();

    QListWidgetItem *previousItem;

    bool dontSelect = false;

    bool askToDiscard();

    Device *device;
};

#endif // DEVICEOPTIONSWIDGET_H
