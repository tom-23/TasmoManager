#ifndef DEVICEMQTTSERVERSWIDGET_H
#define DEVICEMQTTSERVERSWIDGET_H

#include <QWidget>

namespace Ui {
class DeviceMQTTServersWidget;
}

class DeviceMQTTServersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceMQTTServersWidget(QWidget *parent = nullptr);
    ~DeviceMQTTServersWidget();

private:
    Ui::DeviceMQTTServersWidget *ui;
    QString lastTopic;
    QString lastFullTopic;
    int lastMQTTServer;
};

#endif // DEVICEMQTTSERVERSWIDGET_H
