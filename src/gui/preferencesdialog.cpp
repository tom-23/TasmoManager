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
    Q_UNUSED(current);
    Q_UNUSED(previous);
    ui->stackedWidget->setCurrentIndex(ui->listWidget->currentIndex().row());
}

void PreferencesDialog::setMQTTManager(MQTTServerManager *_serverManager) {
    serverManager = _serverManager;
    updateMQTTServerList();
}

void PreferencesDialog::on_addButton_clicked()
{
    EditServerDialog *editServerDialog = new EditServerDialog(this);
    editServerDialog->setWindowModality(Qt::WindowModal);
    if (editServerDialog->exec() == 1) {
        serverManager->serverList->append(editServerDialog->serverInfo);
        serverManager->saveServerList();
        updateMQTTServerList();
    };
}

void PreferencesDialog::updateMQTTServerList() {

    int lastSelectedIndex = 0;
    if (ui->mqttServersList->selectedItems().size() != 0) {
        lastSelectedIndex = ui->mqttServersList->currentRow();
    }
    ui->mqttServersList->clear();
    for (int i = 0; i < serverManager->serverList->size(); i++) {
        MQTTServerInfo info = serverManager->serverList->at(i);
        QString user = "";
        if (info.username != "") {
            user =  info.username + "@";
        }
        QListWidgetItem *itemWidget = new QListWidgetItem(ui->mqttServersList);
        itemWidget->setText(info.name + " (" + user + info.ipAddress.toString() + ")");
        itemWidget->setSelected(false);
        ui->mqttServersList->addItem(itemWidget);
    }
    if(ui->mqttServersList->count() != 0) {
        if (ui->mqttServersList->count() > lastSelectedIndex) {
            ui->mqttServersList->setCurrentRow(lastSelectedIndex);
        } else {
            ui->mqttServersList->setCurrentRow(ui->mqttServersList->count() - 1);
        }

    }
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

void PreferencesDialog::goToAboutPage() {
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    Q_UNUSED(current);
    Q_UNUSED(previous);
}

void PreferencesDialog::on_githubButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/tom-23/TasmoManager"));
}

void PreferencesDialog::on_editButton_clicked()
{
    if (ui->mqttServersList->selectedItems().size() != 0) {
        int itemIndex = ui->mqttServersList->currentRow();
        EditServerDialog *editServerDialog = new EditServerDialog(this);
        editServerDialog->setWindowModality(Qt::WindowModal);
        editServerDialog->setMQTTServer(serverManager->serverList->at(itemIndex));
        if (editServerDialog->exec() == 1) {
            serverManager->serverList->removeAt(itemIndex);
            serverManager->serverList->insert(itemIndex, editServerDialog->serverInfo);
            serverManager->saveServerList();
            updateMQTTServerList();
        };
    }
}

void PreferencesDialog::on_mqttServersList_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    on_editButton_clicked();
}
