#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "mainwindow.h"
#include "editserverdialog.h"
#include "version.h"


namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    void setMQTTManager(MQTTServerManager *serverManager);

    void goToAboutPage();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_mqttServersList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_saveChangesButton_clicked();

    void on_githubButton_clicked();

private:
    Ui::PreferencesDialog *ui;

    MQTTServerManager *serverManager;

    void updateMQTTServerList();

    bool updatingServerList = false;
};

#endif // PREFERENCESDIALOG_H
