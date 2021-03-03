#include "devicemqttserverswidget.h"
#include "ui_devicemqttserverswidget.h"

DeviceMQTTServersWidget::DeviceMQTTServersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceMQTTServersWidget)
{
    ui->setupUi(this);
}

DeviceMQTTServersWidget::~DeviceMQTTServersWidget()
{
    delete ui;
}
