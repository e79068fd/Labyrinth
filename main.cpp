#include "oglwindow.h"
#include <mainwindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mw;
    OGLWindow oglw;
    oglw.show();
    QObject::connect(&oglw, SIGNAL(changeWindow()), &mw, SLOT(show()));
    QObject::connect(&mw, SIGNAL(changeWindow()), &oglw, SLOT(show()));
    //w.show();
    return a.exec();
}
