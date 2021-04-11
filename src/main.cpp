#include "gui/mainwindow.h"
#include "version.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);
    a.setApplicationName("TasmoManager");
    a.setOrganizationName("TasmoManager");

    qDebug() << "TasmoManager version" << TASMOMANAGER_VERSION;

    QFontDatabase::addApplicationFont(":/fonts/fonts/Rublik/Rubik-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Rublik/Rubik-Medium.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Rublik/Rubik-MediumItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Rublik/Rubik-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Rublik/Rubik-Italic.ttf");
    MainWindow w;
    w.show();
    return a.exec();
}
