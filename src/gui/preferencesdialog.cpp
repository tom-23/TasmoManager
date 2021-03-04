#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    ui->version->setText(TASMOMANAGER_VERSION);
    ui->listWidget->setCurrentRow(0);
    ui->saveIPSettingsButton->setVisible(false);
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

void PreferencesDialog::setPreferencesManager(PreferencesManager *_preferencesManager) {
    preferencesManager = _preferencesManager;
    ui->firstIP->setText(preferencesManager->firstScanIP.toString());
    ui->lastIP->setText(preferencesManager->lastScanIP.toString());
    ui->automaticButton->setChecked(preferencesManager->useAutomaticDeviceScanning);
    ui->manualButton->setChecked(!preferencesManager->useAutomaticDeviceScanning);
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
        MQTTServerInfo *info = serverManager->serverList->at(i);
        QString user = "";
        if (info->username != "") {
            user =  info->username + "@";
        }
        QListWidgetItem *itemWidget = new QListWidgetItem(ui->mqttServersList);
        itemWidget->setText(info->name + " (" + user + info->ipAddress.toString() + ")");
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

void PreferencesDialog::on_closeButton_clicked()
{
    this->accept();
}

void PreferencesDialog::on_openIssueButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/tom-23/TasmoManager/issues"));
}

void PreferencesDialog::on_saveIPSettingsButton_clicked()
{
    QHostAddress firstHostAddress = QHostAddress(ui->firstIP->text());
    QHostAddress lastHostAddress = QHostAddress(ui->lastIP->text());
    if (firstHostAddress.isNull() || lastHostAddress.isNull()) {
        auto m = new QMessageBox(this);
        m->setText("One or both IP addresses are invalid");
        m->setIcon(QMessageBox::Warning);
        m->setWindowModality(Qt::WindowModal);
        m->setStandardButtons(QMessageBox::Ok);
        m->exec();
        return;
    }
    preferencesManager->firstScanIP = firstHostAddress;
    preferencesManager->lastScanIP = lastHostAddress;
    preferencesManager->savePreferences();
    ui->saveIPSettingsButton->setVisible(false);
}

void PreferencesDialog::on_firstIP_textChanged(const QString &arg1)
{
    firstIPChanged = (arg1 != preferencesManager->firstScanIP.toString());
    updateIPsChanged();
}

void PreferencesDialog::on_lastIP_textChanged(const QString &arg1)
{
    lastIPChanged = (arg1 != preferencesManager->lastScanIP.toString());
    updateIPsChanged();
}

void PreferencesDialog::updateIPsChanged() {
    ui->saveIPSettingsButton->setVisible(lastIPChanged || firstIPChanged);
}

void PreferencesDialog::on_automaticButton_toggled(bool checked)
{
    preferencesManager->useAutomaticDeviceScanning = checked;
    if (checked) {
        preferencesManager->savePreferences();
    }
}

void PreferencesDialog::on_manualButton_toggled(bool checked)
{
    preferencesManager->useAutomaticDeviceScanning = !checked;
    if (checked) {
        preferencesManager->savePreferences();
    }
}
