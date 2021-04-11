#include "deviceoptionswidget.h"
#include "ui_deviceoptionswidget.h"

DeviceOptionsWidget::DeviceOptionsWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceOptionsWidget)
{
    ui->setupUi(this);
    QMovie *movie = new QMovie(":/gif/assets/light_loader.gif");
    movie->setScaledSize(QSize(16, 16));
    ui->spinner->setMovie(movie);
    movie->start();
    ui->generalSetOptions->setVisible(false);
    ui->buttonsSetOptions->setVisible(false);
    ui->lightsSetOptions->setVisible(false);
    ui->temperatureSetOptions->setVisible(true);
    ui->wifiSetOptions->setVisible(false);
    ui->mqttSetOptions->setVisible(false);
    ui->irRfSetOptions->setVisible(true);
    ui->miscSetOptions->setVisible(false);
}

DeviceOptionsWidget::~DeviceOptionsWidget()
{
    delete ui;
}

bool DeviceOptionsWidget::updateCurrentInfo() {

    switch (ui->stackedWidget->currentIndex()) {
    case 0:
    break;
    case 1:
        ui->buttonsWidget->setDevice(device);
    break;
    case 2:
        ui->lightsWidget->setDevice(device);
    break;
    case 3:
        ui->spinner->setVisible(true);
        device->getSetOptions(temperatureSetOptions);
    break;
    case 4:
        ui->wifiNetworksWidget->setDevice(device);
    break;
    case 5:
        ui->mqttWidget->setDevice(device);
    break;
    case 6:
        ui->spinner->setVisible(true);
        device->getSetOptions(irRfSetOptions);
    break;
    case 7:
    break;
    case 8:
        ui->infoWidget->setDevice(device);
    break;
    case 9:
        ui->fullInfoWidget->setDevice(device);
    break;
    }
    return updateCurrentSetOptions();
}

bool DeviceOptionsWidget::updateCurrentSetOptions() {
    if (ui->showSetOptionsButton->isChecked()) {

        bool optionsEdited = false;

        for (int i = 0; i < device->deviceInfo.setOptions->size(); i++) {
            SetOption *setOption = device->deviceInfo.setOptions->at(i);
            if (setOption->valueChanged) {
                optionsEdited = true;
            }
        }

        if (optionsEdited) {

        }
        ui->spinner->setVisible(true);
        switch (ui->stackedWidget->currentIndex()) {
        case 0:
            device->getSetOptions(generalSetOptions);
        break;
        case 1:
            device->getSetOptions(buttonsSetOptions);
        break;
        case 2:
            device->getSetOptions(lightingSetOptions);
        break;
        case 3:

        break;
        case 4:
            device->getSetOptions(wifiSetOptions);
        break;
        case 5:
            device->getSetOptions(mqttSetOptions);
        break;
        case 6:

        break;

        case 7:
        break;
        case 8:
            device->getSetOptions(miscSetOptions);
        break;
        case 9:
            ui->spinner->setVisible(false);
        break;
        }
    }
    return true;
}

void DeviceOptionsWidget::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    ui->stackedWidget->setCurrentIndex(ui->listWidget->currentIndex().row());
    updateCurrentInfo();
}

void DeviceOptionsWidget::on_showSetOptionsButton_toggled(bool checked)
{
    Q_UNUSED(checked);
    if (checked) {
        updateCurrentSetOptions();
    }
    ui->generalSetOptions->setVisible(checked);
    ui->buttonsSetOptions->setVisible(checked);
    ui->lightsSetOptions->setVisible(checked);

    ui->wifiSetOptions->setVisible(checked);
    ui->mqttSetOptions->setVisible(checked);
    ui->irRfSetOptions->setVisible(checked);
    ui->miscSetOptions->setVisible(checked);
}

void DeviceOptionsWidget::showMQTTPage() {
    ui->listWidget->setCurrentRow(5);
}

void DeviceOptionsWidget::showWIFIPage() {
    ui->listWidget->setCurrentRow(4);
}

void DeviceOptionsWidget::initalizeUI() {

    connect(device, &Device::getSetOptionsDone, this, [=] () {
        ui->spinner->setVisible(false);
    });

    connect(device, &Device::setOptionValueUpdate, this, [=] (SetOption *setOption) {
        for (int i = 0; i < setOptionWidgetList->size(); i++) {
            SetOptionWidget *setOptionWidget = setOptionWidgetList->at(i);
            if (setOptionWidget->objectName() == "SetOption" + QString::number(setOption->number)) {
                setOptionWidget->refreshValue();
            }
        }
    });

    ui->spinner->setVisible(false);

    for (int i = 0; i < device->deviceInfo.setOptions->size(); i++) {

        SetOption *setOption = device->deviceInfo.setOptions->at(i);
        SetOptionWidget *setOptionWidget = new SetOptionWidget(ui->generalPage, setOption);

        if (setOption->category == SetOptionCategory::General) {
            ui->generalSetOptions->layout()->addWidget(setOptionWidget);
            generalSetOptions->append(setOption);
        } else if (setOption->category == SetOptionCategory::Buttons) {
            ui->buttonsSetOptions->layout()->addWidget(setOptionWidget);
            buttonsSetOptions->append(setOption);
        } else if (setOption->category == SetOptionCategory::Lighting) {
            ui->lightsSetOptions->layout()->addWidget(setOptionWidget);
            lightingSetOptions->append(setOption);
        } else if (setOption->category == SetOptionCategory::Temperature) {
            ui->temperatureSetOptions->layout()->addWidget(setOptionWidget);
            temperatureSetOptions->append(setOption);
        } else if (setOption->category == SetOptionCategory::WIFI) {
            ui->wifiSetOptions->layout()->addWidget(setOptionWidget);
            wifiSetOptions->append(setOption);
        } else if (setOption->category == SetOptionCategory::MQTT) {
            ui->mqttSetOptions->layout()->addWidget(setOptionWidget);
            mqttSetOptions->append(setOption);
        } else if (setOption->category == SetOptionCategory::IrRf) {
            ui->irRfSetOptions->layout()->addWidget(setOptionWidget);
            irRfSetOptions->append(setOption);
        } else {
            ui->miscSetOptions->layout()->addWidget(setOptionWidget);
            miscSetOptions->append(setOption);
        }
        setOptionWidgetList->append(setOptionWidget);
    }
    ui->generalSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->buttonsSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->lightsSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->temperatureSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->wifiSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->mqttSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->irRfSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->miscSetOptions->layout()->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));

    ui->listWidget->setCurrentRow(0);
    updateCurrentSetOptions();
}

void DeviceOptionsWidget::on_refreshUIButton_clicked()
{
    updateCurrentInfo();
}

bool DeviceOptionsWidget::askToDiscard() {
    auto m = new QMessageBox(this);
    m->setText("Unsaved Changes");
    m->setInformativeText("Are you sure you want to refresh? This will result in changes being lost.");
    m->setIcon(QMessageBox::Warning);
    m->setWindowModality(Qt::WindowModal);
    m->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (m->exec() == QMessageBox::No) {
        return false;
    } else {
        return true;
    }
}

void DeviceOptionsWidget::on_closeButton_clicked()
{
    this->accept();
}
