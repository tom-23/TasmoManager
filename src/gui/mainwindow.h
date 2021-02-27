#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QTreeWidgetItem>
#include <QProgressDialog>
#include <QCloseEvent>

#include "setupdevicedialog.h"
#include "preferencesdialog.h"
#include "terminaldialog.h"
#include "selectserverdialog.h"

#include "../app/devicemanager.h"
#include "../app/mqttservermanager.h"

class DeviceColorWidget;
class PreferencesDialog;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    DeviceManager *deviceManager;
    MQTTServerManager *serverManager;

private slots:

    void on_actiontest_triggered();

    void on_actionPreferences_triggered();

    void on_connectButton_clicked();

    void on_deviceDiscovered(DeviceInfo deviceInfo);

    void on_deviceInfoUpdate(DeviceInfo deviceInfo);

    void on_terminalButton_clicked();

    void on_deviceList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_webUIButton_clicked();

    void on_settingsButton_clicked();

    void on_actionAbout_triggered();

    void on_refreshButton_clicked();

    void on_deviceButton_clicked();

    void on_restartButton_clicked();

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    void updateInfoText();

    PreferencesDialog *preferencesDialog;

    Device *selectedDevice;

};
#endif // MAINWINDOW_H
