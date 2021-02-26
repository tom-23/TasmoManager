#include "deviceoptionswidget.h"
#include "ui_deviceoptionswidget.h"

DeviceOptionsWidget::DeviceOptionsWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceOptionsWidget)
{
    ui->setupUi(this);
}

DeviceOptionsWidget::~DeviceOptionsWidget()
{
    delete ui;
}
