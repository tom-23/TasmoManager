#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    ui->version->setText(TASMOMANAGER_VERSION);
    ui->listWidget->setCurrentRow(0);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->stackedWidget->setCurrentIndex(ui->listWidget->currentIndex().row());
}

void PreferencesDialog::setMQTTManager(MQTTServerManager *_serverManager) {
    serverManager = _serverManager;
    updateMQTTServerList();
}

void PreferencesDialog::on_addButton_clicked()
{
    MQTTServerInfo serverInfo;
    EditServerDialog *editServerDialog = new EditServerDialog(this);
    editServerDialog->setWindowModality(Qt::WindowModal);
    editServerDialog->setMQTTServer(serverInfo);
    if (editServerDialog->exec() == 1) {
        serverManager->serverList->append(editServerDialog->serverInfo);
        serverManager->saveServerList();
        updateMQTTServerList();
    };
}

void PreferencesDialog::updateMQTTServerList() {
    int currentIndex = ui->mqttServersList->currentRow();
    bool currentItemPresent = ui->mqttServersList->currentItem();
    updatingServerList = true;
    ui->mqttServersList->clear();
    for (int i = 0; i < serverManager->serverList->size(); i++) {
        MQTTServerInfo info = serverManager->serverList->at(i);
        ui->mqttServersList->addItem(info.name + " (" + info.ipAddress.toString() + ")");
    }
    if(ui->mqttServersList->count() != 0) {
        if (currentItemPresent) {
            if (ui->mqttServersList->count() < currentIndex) {
                ui->mqttServersList->item(ui->mqttServersList->count())->setSelected(true);
            } else {
                ui->mqttServersList->item(currentIndex)->setSelected(true);
            }
        }
    }
    updatingServerList = false;
}

void PreferencesDialog::on_removeButton_clicked()
{
    if (ui->mqttServersList->selectedItems().size() != 0) {
        int itemIndex = ui->mqttServersList->currentRow();

        serverManager->serverList->removeAt(itemIndex);
        serverManager->saveServerList();
        updateMQTTServerList();
    }
}

void PreferencesDialog::on_mqttServersList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!updatingServerList) {
        if (serverManager->serverList->size() == ui->mqttServersList->count()) {
            MQTTServerInfo info = serverManager->serverList->at(ui->mqttServersList->currentRow());
            ui->serverName->setText(info.name);
            ui->serverIP->setText(info.ipAddress.toString());
            ui->serverPort->setValue(info.port);
            ui->serverUsername->setText(info.username);
            ui->serverPassword->setText(QString::fromUtf8(info.password));
        }
    }
}

void PreferencesDialog::on_saveChangesButton_clicked()
{
    if (ui->serverName->text() == ""
            || ui->serverIP->text() == ""
            || ui->serverUsername->text() == ""
            || ui->serverPassword->text() == "") {
        auto m = new QMessageBox(this);
        m->setText("Please fill out all parameters.");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }

    if (QHostAddress(ui->serverIP->text()).isNull()) {
        auto m = new QMessageBox(this);
        m->setText("IP Address is invalid.");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }

    int currentIndex = ui->mqttServersList->currentRow();

    MQTTServerInfo serverInfo;

    serverInfo.name = ui->serverName->text();
    serverInfo.ipAddress = QHostAddress(ui->serverIP->text());
    serverInfo.port = ui->serverPort->value();
    serverInfo.username = ui->serverUsername->text();
    serverInfo.password = ui->serverPassword->text().toUtf8();

    serverManager->serverList->removeAt(currentIndex);
    serverManager->serverList->insert(currentIndex, serverInfo);

    serverManager->saveServerList();
    qDebug() << "Updated Server";
    updateMQTTServerList();

}

void PreferencesDialog::goToAboutPage() {
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

void PreferencesDialog::on_githubButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/tom-23/TasmoManager"));
}
