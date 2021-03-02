#ifndef DEVICEOPTIONSWIDGET_H
#define DEVICEOPTIONSWIDGET_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class DeviceOptionsWidget;
}

class DeviceOptionsWidget : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceOptionsWidget(QWidget *parent = nullptr);
    ~DeviceOptionsWidget();

private slots:
    void on_cancelButton_clicked();

private:
    Ui::DeviceOptionsWidget *ui;
};

#endif // DEVICEOPTIONSWIDGET_H
