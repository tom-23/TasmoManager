#include "setoptionwidget.h"
#include "ui_setoptionwidget.h"

SetOptionWidget::SetOptionWidget(QWidget *parent, SetOption *_setOption) :
    QWidget(parent),
    ui(new Ui::SetOptionWidget)
{
    ui->setupUi(this);
    ui->warningLabel->setVisible(false);
    setOption = _setOption;
    this->setObjectName("SetOption" + QString::number(setOption->number));
    ui->nameLabel->setText("(SetOption" + QString::number(setOption->number) + ") " + setOption->name + ":");
    if (setOption->warning != "") {
        ui->warningLabel->setVisible(true);
        ui->warningLabel->setText(setOption->warning);
    }
    ui->iconWidget->setVisible(setOption->restartRequired);
    if (setOption->typeName == "INTEGER") {
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        spinBox->setMinimum(setOption->valueMin);
        spinBox->setMaximum(setOption->valueMax);
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=] (int value) {
            if (!uiUpdating){
                setOption->value = value;
                setOption->valueChanged = (oldValue != value);
                revertButton->setVisible(setOption->valueChanged);
                saveButton->setVisible(setOption->valueChanged);
            }
        });
        inputBox = spinBox;
        ui->controlsLayout->addWidget(spinBox);
    }
    if (setOption->typeName == "ENUM") {
        QComboBox *comboBox = new QComboBox(this);
        comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        comboBox->addItems(setOption->values);
        comboBox->setCurrentText(0);
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=] (int value) {
            if (!uiUpdating){
                setOption->value = value;
                setOption->valueChanged = (oldValue != value);
                revertButton->setVisible(setOption->valueChanged);
                saveButton->setVisible(setOption->valueChanged);
            }
        });
        inputBox = comboBox;
        ui->controlsLayout->addWidget(comboBox);
    }
    if (setOption->typeName == "STRING") {
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        connect(lineEdit, &QLineEdit::textChanged, this, [=] (QString value) {
            if (!uiUpdating){
                setOption->valueString = value;
                setOption->valueChanged = (oldStringValue != value);
                revertButton->setVisible(setOption->valueChanged);
                saveButton->setVisible(setOption->valueChanged);
            }
        });
        inputBox = lineEdit;
        ui->controlsLayout->addWidget(lineEdit);
    }

    saveButton = new QPushButton(this);
    saveButton->setObjectName("saveButton");
    saveButton->setText("");
    saveButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    saveButton->setVisible(false);

    connect(saveButton, &QPushButton::clicked, this, [=] () {
        QList<SetOption *> *setOptionList = new QList<SetOption *>;
        setOptionList->append(setOption);
        setOption->device->saveSetOptions(setOptionList);
        revertButton->setVisible(false);
        saveButton->setVisible(false);
    });

    ui->controlsLayout->addWidget(saveButton);

    revertButton = new QPushButton(this);
    revertButton->setObjectName("revertButton");
    revertButton->setText("");
    revertButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    revertButton->setVisible(false);

    connect(revertButton, &QPushButton::clicked, this, [=] () {
        if (setOption->typeName == "ENUM") {
            QComboBox *comboBox = qobject_cast<QComboBox*>(inputBox);
            setOption->value = oldValue;
            comboBox->setCurrentIndex(setOption->value);
        } else if (setOption->typeName == "INTEGER") {
            QSpinBox *spinBox = qobject_cast<QSpinBox*>(inputBox);
            setOption->value = oldValue;
            spinBox->setValue(setOption->value);
        } else {
            QLineEdit *lineEdit = qobject_cast<QLineEdit*>(inputBox);
            setOption->valueString = oldStringValue;
            lineEdit->setText(setOption->valueString);
        }
    });

    ui->controlsLayout->addWidget(revertButton);

    if (setOption->link.toString() != "") {
        QPushButton *linkButton = new QPushButton(this);
        linkButton->setObjectName("linkButton");
        linkButton->setText("");
        linkButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        connect(linkButton, &QPushButton::clicked, this, [=] () {
            QDesktopServices::openUrl(setOption->link);
        });
        ui->controlsLayout->addWidget(linkButton);
    }

    if (setOption->link1.toString() != "") {
        QPushButton *link1Button = new QPushButton(this);
        link1Button->setObjectName("link1Button");
        link1Button->setText("");
        link1Button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        connect(link1Button, &QPushButton::clicked, this, [=] () {
            QDesktopServices::openUrl(setOption->link1);
        });
        ui->controlsLayout->addWidget(link1Button);
    }

    if (setOption->info != "") {
        QPushButton *infoButton = new QPushButton(this);
        infoButton->setObjectName("infoButton");
        infoButton->setText("Info");
        infoButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        connect(infoButton, &QPushButton::clicked, this, [=] () {
            QString messageText = setOption->info;
            if (setOption->info1 != "") { messageText = messageText + "\n" + setOption->info1; }
            if (setOption->info2 != "") { messageText = messageText + "\n" + setOption->info2; }
            if (setOption->info3 != "") { messageText = messageText + "\n" + setOption->info3; }
            if (setOption->info4 != "") { messageText = messageText + "\n" + setOption->info4; }
            if (setOption->info5 != "") { messageText = messageText + "\n" + setOption->info5; }
            if (setOption->info6 != "") { messageText = messageText + "\n" + setOption->info6; }
            auto m = new QMessageBox(this);
            m->setText(messageText);
            m->setIcon(QMessageBox::Information);
            m->setWindowModality(Qt::WindowModal);
            m->setStandardButtons(QMessageBox::Ok);
            m->setButtonText(0, "Close");
            m->exec();
        });
        ui->controlsLayout->addWidget(infoButton);
    }
}

void SetOptionWidget::refreshValue() {
    uiUpdating = true;
    if (setOption->typeName == "ENUM") {
        QComboBox *comboBox = qobject_cast<QComboBox*>(inputBox);
        oldValue = setOption->value;
        comboBox->setCurrentIndex(setOption->value);
    } else if (setOption->typeName == "INTEGER") {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(inputBox);
        oldValue = setOption->value;
        spinBox->setValue(setOption->value);
    } else {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(inputBox);
        oldStringValue = setOption->valueString;
        lineEdit->setText(setOption->valueString);
    }
    uiUpdating = false;
}

SetOptionWidget::~SetOptionWidget()
{
    delete ui;
}
