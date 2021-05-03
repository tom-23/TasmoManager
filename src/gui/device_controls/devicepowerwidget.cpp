#include "devicepowerwidget.h"
#include "ui_devicepowerwidget.h"

DevicePowerWidget::DevicePowerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevicePowerWidget)
{
    ui->setupUi(this);

    this->setVisible(false);

    delayTimer = new QTimer(this);
    delayTimer->setSingleShot(true);
    delayTimer->setInterval(500);

    connect(delayTimer, &QTimer::timeout, this, [=] () {
        updateUI();
    });
}

DevicePowerWidget::~DevicePowerWidget()
{
    delete ui;
}

void DevicePowerWidget::setDevice(Device *_device) {

    device = _device;

    if (device != nullptr) {
        updateUI();

        connect(device, &Device::recievedStateUpdate, this, [=] () {
            delayTimer->start();
        });
    } else {
        this->setVisible(false);
    }
}

void DevicePowerWidget::updateUI() {
    qDebug() << "[Power Widget] Updating info...";
    if (!device) {
        return;
    }
    updatingUI = true;

    for (int i = 0; i < device->deviceInfo.power.size(); i++) {
        QPushButton *pushButton = this->findChild<QPushButton *>("power" + QString::number(i + 1));
        if (!pushButton) {
            pushButton = new QPushButton(ui->centralWidget);
            pushButton->setObjectName("power" + QString::number(i + 1));
            pushButton->setProperty("powerNumber", i + 1);
            pushButton->setCheckable(true);
            connect(pushButton, &QPushButton::toggled, this, [=](bool toggled) {
                if (updatingUI) {
                    return;
                }
                device->setPower(pushButton->property("powerNumber").toInt(), toggled);
            });

            QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->centralWidget->layout());
            int col = 0;
            if (gridLayout->count() % 2 != 0) {
                col = 1;
            }
            gridLayout->addWidget(pushButton, gridLayout->count() / 2, col);
        }
        if (pushButton) {
            pushButton->setVisible(device->deviceInfo.power[i]->enabled);
            pushButton->setChecked(device->deviceInfo.power[i]->power);
            QString webUiName = device->deviceInfo.power[i]->webUiName;
            if (webUiName != "") {
                pushButton->setText(webUiName);
            } else {
                pushButton->setText("POWER " + QString::number(i + 1));
            }
        }
    }

    this->setVisible(device->deviceInfo.power[0]->enabled);

    updatingUI = false;
}
