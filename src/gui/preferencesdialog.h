#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "mainwindow.h"
#include "editserverdialog.h"
#include "version.h"
#include "app/preferencesmanager.h"

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

    void setPreferencesManager(PreferencesManager *preferencesManager);

    void goToAboutPage();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_addButton_clicked();

    void on_editButton_clicked();

    void on_removeButton_clicked();

    void on_githubButton_clicked();

    void on_mqttServersList_itemDoubleClicked(QListWidgetItem *item);

    void on_closeButton_clicked();

    void on_openIssueButton_clicked();

    void on_saveIPSettingsButton_clicked();

    void on_firstIP_textChanged(const QString &arg1);

    void on_lastIP_textChanged(const QString &arg1);

    void on_automaticButton_toggled(bool checked);

    void on_manualButton_toggled(bool checked);

private:
    Ui::PreferencesDialog *ui;

    MQTTServerManager *serverManager;
    PreferencesManager *preferencesManager;

    void updateMQTTServerList();

    bool firstIPChanged;
    bool lastIPChanged;

    void updateIPsChanged();
};

#endif // PREFERENCESDIALOG_H
