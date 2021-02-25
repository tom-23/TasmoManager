#ifndef TERMINALDIALOG_H
#define TERMINALDIALOG_H

#include <QDialog>
#include <QScrollBar>
#include "../app/device.h"

namespace Ui {
class TerminalDialog;
}

class TerminalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TerminalDialog(QWidget *parent = nullptr, Device *device = nullptr);
    ~TerminalDialog();

private slots:

    void on_sendButton_clicked();

private:
    Ui::TerminalDialog *ui;
    Device *device;

    void closeEvent(QCloseEvent *closeEvent);

    bool autoScroll;
};

#endif // TERMINALDIALOG_H
