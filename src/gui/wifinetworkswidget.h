#ifndef WIFINETWORKSWIDGET_H
#define WIFINETWORKSWIDGET_H

#include <QWidget>
#include <QMovie>
#include <QHostAddress>
#include "app/device.h"

namespace Ui {
class WifiNetworksWidget;
}

class WifiNetworksWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WifiNetworksWidget(QWidget *parent = nullptr);
    ~WifiNetworksWidget();

    void setDevice(Device *device);
    void refreshInfo();

private slots:

    void on_staticIP_currentIndexChanged(int index);

    void on_currentAP_currentIndexChanged(int index);

    void on_revertButton_clicked();

    void on_ap1SSID_textChanged(const QString &arg1);

    void on_ap1Password_textChanged(const QString &arg1);

    void on_ap2SSID_textChanged(const QString &arg1);

    void on_ap2Password_textChanged(const QString &arg1);

    void on_ipAddress_textChanged(const QString &arg1);

    void on_gateway_textChanged(const QString &arg1);

    void on_subnetMask_textChanged(const QString &arg1);

    void on_dnsServer_textChanged(const QString &arg1);

    void on_saveButton_clicked();

private:
    Ui::WifiNetworksWidget *ui;
    Device *device;

    int commandCount;

    void valueChanged();
};

#endif // WIFINETWORKSWIDGET_H
