#include "terminaldialog.h"
#include "ui_terminaldialog.h"

TerminalDialog::TerminalDialog(QWidget *parent, Device *_device) :
    QDialog(parent),
    ui(new Ui::TerminalDialog)
{
    ui->setupUi(this);
    device = _device;
    this->setWindowTitle(device->deviceInfo.name + " - Terminal");
    device->setLogLevel(4);
    ui->terminalText->setText("");
    connect(device, &Device::recievedLogMessage, this, [=] (QString logMessage) {
        if (logMessage != "") {
            ui->terminalText->setText(ui->terminalText->toPlainText() + "\n" + logMessage);
            ui->terminalText->verticalScrollBar()->setValue(ui->terminalText->verticalScrollBar()->maximum());
        }
    });

    connect(ui->terminalText->verticalScrollBar(), &QScrollBar::sliderMoved, this, [=] () {
        if (ui->terminalText->verticalScrollBar()->value() == ui->terminalText->verticalScrollBar()->maximum()) {
            autoScroll = true;
        } else {
            autoScroll = false;
        }
    });
}

TerminalDialog::~TerminalDialog()
{
    delete ui;
}

void TerminalDialog::closeEvent(QCloseEvent *closeEvent) {
    Q_UNUSED(closeEvent);
    device->setLogLevel(0);
}


void TerminalDialog::on_sendButton_clicked()
{
    device->sendCommand(ui->terminalCommand->text());
    ui->terminalCommand->clear();
}
