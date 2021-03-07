#ifndef DEVICEMQTTSERVERSWIDGET_H
#define DEVICEMQTTSERVERSWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QDebug>
#include <QMovie>

#include "app/device.h"
#include "editserverdialog.h"

namespace Ui {
class DeviceMQTTServersWidget;
}

class DeviceMQTTServersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceMQTTServersWidget(QWidget *parent = nullptr);
    ~DeviceMQTTServersWidget();

    void setDevice(Device *device);

private slots:
    void on_editButton_clicked();


    void on_actionInsert_prefix_triggered();

    void on_actionInsert_Topic_triggered();

    void on_revertButton_clicked();

    void on_serverList_currentTextChanged(const QString &arg1);

    void on_clientName_textChanged(const QString &arg1);

    void on_topic_textChanged(const QString &arg1);

    void on_fullTopic_textChanged(const QString &arg1);

    void on_saveButton_clicked();

private:
    Ui::DeviceMQTTServersWidget *ui;

    Device *device;

    void refreshInfo();

    int commandCount;

    void valueChanged();

    MQTTServerInfo mqttSeverInfo;
};

#endif // DEVICEMQTTSERVERSWIDGET_H
