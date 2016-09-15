#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(released()), this, SLOT(changeWindow_s()));
    //OGLWindow* ogl = new OGLWindow();

    //QWidget* container = QWidget::createWindowContainer(ogl);
    //QGridLayout* layout = new QGridLayout();
    //layout->addWidget(container);
    //ui->centralwidget->setLayout(layout);
    //QWidget* widget = new QWidget();
    //widget->setStyleSheet("background:black");
    //layout->addWidget(widget);
    //QTimer* timer = new QTimer();
    //timer->start(500);
    //connect(timer, SIGNAL(timeout()), ogl, SLOT(update()));
    //container->show();
    //container->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeWindow_s() {
    hide();
    emit changeWindow();
}
