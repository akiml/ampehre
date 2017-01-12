#include "gui/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mpPowerplot(new QMSMPowerPlot(parent)),
    mClient(CClient()),
    mpTimer(new QTimer()),
    mInterval(500)
{
    ui->setupUi(this);
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_reset->setDisabled(true);
    ui->mdiArea->addSubWindow(mpPowerplot->getPlot());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mpPowerplot;
    delete mpTimer;
}

void MainWindow::connectActions()
{
    connect(this, SIGNAL(ui->pushButton_start->clicked()), this, SLOT(start()));
    connect(this, SIGNAL(ui->pushButton_stop->clicked()), this, SLOT(stop()));
    connect(mpTimer, SIGNAL(mpTimer->timeout()), this, SLOT(requestData()));
}

void MainWindow::start()
{
    std::vector<int> values;
    CProtocolC::addAll(values);
    mClient.registerToServer(values, 2900, "131.234.58.31");

    mpTimer->setInterval(mInterval);
    mpTimer->start();
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(false);
    ui->pushButton_reset->setDisabled(false);
}

void MainWindow::stop()
{
    mpTimer->stop();
    mClient.terminate();
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_start->setDisabled(false);
}

void MainWindow::requestData()
{
    mClient.requestData();
    mpPowerplot->redraw(mClient.mValues);
}

void MainWindow::reset(){}
