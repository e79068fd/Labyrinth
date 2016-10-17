#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonNewGame, SIGNAL(released()), this, SLOT(newGame()));
    connect(ui->buttonPreferences, SIGNAL(released()), this, SLOT(preferences()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newGame() {
    gameWindow = new OGLWindow();
    connect(gameWindow, SIGNAL(endGame()), this, SLOT(show()));
    hide();
    gameWindow->show();
}

void MainWindow::preferences() {

}
