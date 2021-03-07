#ifndef FULLINFOWIDGET_H
#define FULLINFOWIDGET_H

#include <QWidget>
#include "app/device.h"

namespace Ui {
class FullInfoWidget;
}

class FullInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FullInfoWidget(QWidget *parent = nullptr);
    ~FullInfoWidget();

    void setDevice(Device *_device) {
        device = _device;
        connect(device, &Device::recievedInfoUpdate, this, [=] () {
            updateInfo();
        });
        updateInfo();
    };

    void updateInfo();
private:
    Ui::FullInfoWidget *ui;
    Device *device;

};

#endif // FULLINFOWIDGET_H
