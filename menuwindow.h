#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "oglwindow.h"
#include "dialogpreferences.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QTimer>

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = 0);
    ~MenuWindow();

private:
    Ui::MenuWindow *ui;

    OGLWindow* gameWindow;

public slots:
    void newGame();
    void preferences();
    void endGame();

signals:
    void changeWindow();
};

#endif // MAINWINDOW_H
