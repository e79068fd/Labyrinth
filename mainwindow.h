#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "oglwindow.h"
#include "dialogpreferences.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    OGLWindow* gameWindow;

public slots:
    void newGame();
    void preferences();
    void endGame();

signals:
    void changeWindow();
};

#endif // MAINWINDOW_H
