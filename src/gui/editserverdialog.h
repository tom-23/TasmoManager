#ifndef EDITSERVERDIALOG_H
#define EDITSERVERDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "../app/mqttserver.h"

namespace Ui {
class EditServerDialog;
}

class EditServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditServerDialog(QWidget *parent = nullptr);
    ~EditServerDialog();

    void setMQTTServer(MQTTServerInfo *serverInfo);
    MQTTServerInfo *serverInfo;

private slots:
    void on_saveChangesButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::EditServerDialog *ui;


};

#endif // EDITSERVERDIALOG_H
