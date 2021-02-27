#include "gui/mainwindow.h"
#include "src/version.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    qDebug() << "TasmoManager version" << TASMOMANAGER_VERSION;

    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Medium.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-MediumItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Italic.ttf");

    MainWindow w;
    w.show();
    return a.exec();
}
