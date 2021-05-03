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
            pushButton->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(pushButton, &QPushButton::toggled, this, [=](bool toggled) {
                if (updatingUI) {
                    return;
                }
                device->setPower(pushButton->property("powerNumber").toInt(), toggled);
            });

            connect(pushButton, &QPushButton::customContextMenuRequested, this, [=]() {
                QInputDialog *inputDialog = new QInputDialog(this);
                inputDialog->setWindowModality(Qt::WindowModal);
                inputDialog->setInputMode(QInputDialog::TextInput);
                inputDialog->setWindowTitle("Edit WebUI Button Name");
                inputDialog->setLabelText("Enter a new button name:");
                inputDialog->setTextValue(pushButton->text());
                inputDialog->setBaseSize(300, 150);

                // this bit of code allows be to change the color of the buttons and that's all.
                // THANKS Qt!!!!!!!!!
                QList<QDialogButtonBox *> objectList = inputDialog
                                                           ->findChildren<QDialogButtonBox *>();
                if (!objectList.isEmpty()) {
                    objectList.first()->button(QDialogButtonBox::Ok)->setObjectName("okButton");
                }

                if (inputDialog->exec() == 1) {
                    QString newName = inputDialog->textValue();
                    device->setWebUIButtonName(pushButton->property("powerNumber").toInt(), newName);
                }
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
