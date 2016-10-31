#include "oglwindow.h"
#include "menuwindow.h"
#include "preferences.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Preferences::instance();
    MenuWindow mw;
    mw.show();
    return a.exec();
}
