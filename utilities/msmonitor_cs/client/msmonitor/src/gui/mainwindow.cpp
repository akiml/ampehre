#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    powerplot(new QMSMPowerPlot(parent))
{
    ui->setupUi(this);
    ui->mdiArea->addSubWindow(powerplot->getPlot());
}

MainWindow::~MainWindow()
{
    delete ui;
}
