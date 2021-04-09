#include "editserverdialog.h"
#include "ui_editserverdialog.h"

EditServerDialog::EditServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditServerDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    MQTTServerInfo *_serverInfo = new MQTTServerInfo();
    _serverInfo->port = 1883;
    setMQTTServer(_serverInfo);
}

EditServerDialog::~EditServerDialog()
{
    delete ui;
}

void EditServerDialog::setMQTTServer(MQTTServerInfo *_serverInfo) {
    qDebug() << _serverInfo->currentlyConnected;
    if (_serverInfo->currentlyConnected) {
        qDebug() << "Server already connected";
        auto m = new QMessageBox(this);
        m->setText("Cannot edit a currently connected server.");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        this->reject();
    }
    serverInfo = _serverInfo;
    ui->name->setText(serverInfo->name);
    if (!serverInfo->ipAddress.isNull()) {
        ui->host->setText(serverInfo->ipAddress.toString());
    } else {
        ui->host->setText(serverInfo->host);
    }
    ui->port->setValue(serverInfo->port);
    ui->username->setText(serverInfo->username);
    ui->password->setText(serverInfo->password);
}


void EditServerDialog::on_saveChangesButton_clicked()
{
    if (ui->name->text() == ""
            || ui->host->text() == "") {
        auto m = new QMessageBox(this);
        m->setText("Please fill out all required parameters.");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }

    if (ui->host->text() == "") {
        auto m = new QMessageBox(this);
        m->setText("IP Address / Host is empty.");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }

    serverInfo->name = ui->name->text();
    if (!QHostAddress(ui->host->text()).isNull()) {
        serverInfo->ipAddress = QHostAddress(ui->host->text());
    } else {
        serverInfo->host = ui->host->text();
    }
    serverInfo->port = ui->port->value();
    serverInfo->username = ui->username->text();
    serverInfo->password = ui->password->text().toUtf8();

    this->accept();
}

void EditServerDialog::on_cancelButton_clicked()
{
    this->reject();
}
