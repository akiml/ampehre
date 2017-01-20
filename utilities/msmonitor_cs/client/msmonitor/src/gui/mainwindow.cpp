#include "gui/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mpPowerplot(new QMSMPowerPlot(parent)),
    mpTempplot(new QMSMTemperaturePlot(parent)),
    mpClockplot(new QMSMClockPlot(parent)),
    mClient(CClient()),
    mpTimer(new QTimer()),
    mpGuiTimer(new QTimer()),
    mInterval(250),
    mGuiInterval(250)
{
    ui->setupUi(this);
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_reset->setDisabled(true);
    connectActions();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mpPowerplot;
    delete mpTempplot;
    delete mpClockplot;
    delete mpTimer;
}

void MainWindow::connectActions()
{
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(start()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(requestData()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpPowerplot, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempplot, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpClockplot, SLOT(redraw()));
    connect(ui->horizontalSlider_guiRate, SIGNAL(valueChanged(int)), this, SLOT(setGuiInterval(int)));
    connect(ui->horizontalSlider_dataPlot, SIGNAL(valueChanged(int)), this, SLOT(setInterval(int)));
    connect(ui->action_Power, SIGNAL(triggered()),this, SLOT(showPower()));
    connect(ui->action_Temperature, SIGNAL(triggered()),this, SLOT(showTemp()));
    connect(ui->action_Clock, SIGNAL(triggered()),this, SLOT(showClock()));


}

void MainWindow::start()
{
    std::vector<int> values;
    CProtocolC::addAll(values);
    mClient.registerToServer(values, 2900, "131.234.58.31");

    mpTimer->setInterval(mInterval);
    mpGuiTimer->setInterval(mGuiInterval);
    mpTimer->start();
    mpGuiTimer->start();
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
    mpPowerplot->updateValues(mClient.mValues);
    mpTempplot->updateValues(mClient.mValues);
    mpClockplot->updateValues(mClient.mValues);
}

void MainWindow::reset(){}

void MainWindow::setGuiInterval(int val)
{
    if(val > 0)
    {
        this->mGuiInterval = val;
        mpGuiTimer->setInterval(mGuiInterval);
        QString s = QString::number(mGuiInterval) + " ms";
        ui->label_guiRate->setText(s);
    }
}


void MainWindow::setInterval(int val)
{
    if(val > 0)
    {
        this->mInterval = val;
        mpTimer->setInterval(mInterval);
        QString s = QString::number(mInterval) + " ms";
        ui->label_dataPlot->setText(s);
    }
}

void MainWindow::showPower()
{
    QMdiSubWindow* subw = new QMdiSubWindow;
    subw->setWidget(mpPowerplot->getPlot());
    subw->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(subw);
    subw->resize(600, 400);
    subw->show();
}

void MainWindow::showTemp()
{
    QMdiSubWindow* subw = new QMdiSubWindow;
    subw->setWidget(mpTempplot->getPlot());
    subw->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(subw);
    subw->resize(600, 400);
    subw->show();
}

void MainWindow::showClock()
{
    QMdiSubWindow* subw = new QMdiSubWindow;
    subw->setWidget(mpClockplot->getPlot());
    subw->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(subw);
    subw->resize(600, 400);
    subw->show();
}


