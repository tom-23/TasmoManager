#include "deviceoptionswidget.h"
#include "ui_deviceoptionswidget.h"

DeviceOptionsWidget::DeviceOptionsWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceOptionsWidget)
{
    ui->setupUi(this);
    QMovie *movie = new QMovie(":/gif/assets/light_loader.gif");
    movie->setScaledSize(QSize(24, 24));
    ui->spinner->setMovie(movie);
    movie->start();
}

DeviceOptionsWidget::~DeviceOptionsWidget()
{
    delete ui;
}

void DeviceOptionsWidget::on_cancelButton_clicked()
{
    this->reject();
}
