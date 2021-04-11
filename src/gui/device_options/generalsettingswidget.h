#ifndef GENERALSETTINGSWIDGET_H
#define GENERALSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class GeneralSettingsWidget;
}

class GeneralSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsWidget(QWidget *parent = nullptr);
    ~GeneralSettingsWidget();

private:
    Ui::GeneralSettingsWidget *ui;
};

#endif // GENERALSETTINGSWIDGET_H
