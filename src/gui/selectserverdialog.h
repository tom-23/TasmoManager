#ifndef SELECTSERVERDIALOG_H
#define SELECTSERVERDIALOG_H

#include <QDialog>

#include "../app/mqttservermanager.h"

namespace Ui {
class SelectServerDialog;
}

class SelectServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectServerDialog(QWidget *parent = nullptr);
    ~SelectServerDialog();

    void setMQTTServerManager(MQTTServerManager *serverManager);

    MQTTServerInfo selectedServer;
private slots:
    void on_connectButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::SelectServerDialog *ui;

    MQTTServerManager *serverManager;
};

#endif // SELECTSERVERDIALOG_H
