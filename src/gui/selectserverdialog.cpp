#include "selectserverdialog.h"
#include "ui_selectserverdialog.h"

SelectServerDialog::SelectServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectServerDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

SelectServerDialog::~SelectServerDialog()
{
    delete ui;
}

void SelectServerDialog::setMQTTServerManager(MQTTServerManager *_serverManager) {
    serverManager = _serverManager;
    for (int i = 0; i < serverManager->serverList->size(); ++i) {
        MQTTServerInfo *info = serverManager->serverList->at(i);
        QString host = "";
        QString user = "";
        if (!info->ipAddress.isNull()) {
            host = info->ipAddress.toString();
        } else {
            host = info->host;
        }
        if (info->username != "") {
            user = info->username + "@";
        }
        ui->serverComboBox->addItem(info->name + " (" + user + host + ":" + QString::number(info->port) + ")");
    }
    ui->serverComboBox->setCurrentIndex(0);
}

void SelectServerDialog::on_connectButton_clicked()
{
    selectedServer = serverManager->serverList->at(ui->serverComboBox->currentIndex());
    this->accept();
}

void SelectServerDialog::on_cancelButton_clicked()
{
    this->reject();
}
