#ifndef WIFINETWORKSWIDGET_H
#define WIFINETWORKSWIDGET_H

#include <QWidget>
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

private:
    Ui::WifiNetworksWidget *ui;
    Device *device;
};

#endif // WIFINETWORKSWIDGET_H
