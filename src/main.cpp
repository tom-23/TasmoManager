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

    if (QFontDatabase::addApplicationFont(":/fonts/fonts/Inter-Light.ttf") == -1) {
        qDebug() << "[Fonts] WARNING! Couldn't load Inter-Light.ttf";
    }

    if (QFontDatabase::addApplicationFont(":/fonts/fonts/Inter-Regular.ttf") == -1) {
        qDebug() << "[Fonts] WARNING! Couldn't load Inter-Regular.ttf";
    }

    if (QFontDatabase::addApplicationFont(":/fonts/fonts/Inter-Medium.ttf") == -1) {
        qDebug() << "[Fonts] WARNING! Couldn't load Inter-Medium.ttf";
    }

    if (QFontDatabase::addApplicationFont(":/fonts/fonts/Inter-SemiBold.ttf") == -1) {
        qDebug() << "[Fonts] WARNING! Couldn't load Inter-SemiBold.ttf";
    }

    if (QFontDatabase::addApplicationFont(":/fonts/fonts/Inter-Bold.ttf") == -1) {
        qDebug() << "[Fonts] WARNING! Couldn't load Inter-Bold.ttf";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
