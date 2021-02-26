#ifndef DEVICEOPTIONSWIDGET_H
#define DEVICEOPTIONSWIDGET_H

#include <QDialog>

namespace Ui {
class DeviceOptionsWidget;
}

class DeviceOptionsWidget : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceOptionsWidget(QWidget *parent = nullptr);
    ~DeviceOptionsWidget();

private:
    Ui::DeviceOptionsWidget *ui;
};

#endif // DEVICEOPTIONSWIDGET_H
