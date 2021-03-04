#include "editserverdialog.h"
#include "ui_editserverdialog.h"

EditServerDialog::EditServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditServerDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

EditServerDialog::~EditServerDialog()
{
    delete ui;
}

void EditServerDialog::setMQTTServer(MQTTServerInfo _serverInfo) {
    serverInfo = _serverInfo;
    ui->name->setText(serverInfo.name);
    ui->host->setText(serverInfo.ipAddress.toString());
    ui->port->setValue(serverInfo.port);
    ui->username->setText(serverInfo.username);
    ui->password->setText(serverInfo.password);
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

    if (QHostAddress(ui->host->text()).isNull()) {
        auto m = new QMessageBox(this);
        m->setText("IP Address is invalid.");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }

    serverInfo.name = ui->name->text();
    serverInfo.ipAddress = QHostAddress(ui->host->text());
    serverInfo.port = ui->port->value();
    serverInfo.username = ui->username->text();
    serverInfo.password = ui->password->text().toUtf8();

    this->accept();
}

void EditServerDialog::on_cancelButton_clicked()
{
    this->reject();
}
