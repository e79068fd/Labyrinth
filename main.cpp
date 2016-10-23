#include "oglwindow.h"
#include "mainwindow.h"
#include "preferences.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Preferences::instance();
    MainWindow mw;
    mw.show();
    return a.exec();
}
