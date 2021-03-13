#include "generalsettingswidget.h"
#include "ui_generalsettingswidget.h"

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettingsWidget)
{
    ui->setupUi(this);
}

GeneralSettingsWidget::~GeneralSettingsWidget()
{
    delete ui;
}
