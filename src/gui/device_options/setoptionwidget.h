#ifndef SETOPTIONWIDGET_H
#define SETOPTIONWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDesktopServices>
#include "app/device.h"

namespace Ui {
class SetOptionWidget;
}

class SetOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SetOptionWidget(QWidget *parent = nullptr, SetOption *setOption = nullptr);
    ~SetOptionWidget();

    void refreshValue();

private:
    bool uiUpdating = false;
    Ui::SetOptionWidget *ui;
    SetOption *setOption;

    QList<SetOption *> *generalSetOptionList;

    int oldValue;
    QString oldStringValue;

    QWidget *inputBox;

    QPushButton *revertButton;
    QPushButton *saveButton;

};

#endif // SETOPTIONWIDGET_H
