#include "menuwindow.h"
#include "ui_menuwindow.h"

MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    connect(ui->buttonNewGame, SIGNAL(released()), this, SLOT(newGame()));
    connect(ui->buttonPreferences, SIGNAL(released()), this, SLOT(preferences()));

    gameWindow = new OGLWindow();
    connect(gameWindow, SIGNAL(endGame()), this, SLOT(endGame()));
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::newGame() {
    hide();
    gameWindow->startGame();
    gameWindow->show();
}

void MenuWindow::preferences() {
    DialogPreferences dialog;
    dialog.exec();
}

void MenuWindow::endGame() {
    show();
}
