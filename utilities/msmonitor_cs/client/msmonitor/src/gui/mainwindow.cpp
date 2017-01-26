#include "gui/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mpPowerplot (new QMSMPowerPlot(parent)),
    mpTempplot (new QMSMTemperaturePlot(parent)),
    mpClockplot (new QMSMClockPlot(parent)),
    mpUtilplot(new QMSMUtilPlot(parent)),
    mpMemoryplot(new QMSMMemoryPlot(parent)),
    mpHeatmap(new QMSMHeatmap(parent)),
    subwPower(new QMdiSubWindow()),
    subwTemp(new QMdiSubWindow()),
    subwClock(new QMdiSubWindow()),
    subwUtil(new QMdiSubWindow()),
    subwMemory(new QMdiSubWindow()),
    subwHeatmap( new QMdiSubWindow()),
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

    mpPowerplot->initPlot(mpPowerplot->getParent());
    subwPower->setWidget(mpPowerplot->getPlot());
    ui->mdiArea->addSubWindow(subwPower);
    subwPower->resize(600, 400);
    subwPower->hide();

    mpClockplot->initPlot(mpClockplot->getParent());
    subwClock->setWidget(mpClockplot->getPlot());
    ui->mdiArea->addSubWindow(subwClock);
    subwClock->resize(600, 400);
    subwClock->hide();

    mpTempplot->initPlot(mpTempplot->getParent());
    subwTemp->setWidget(mpTempplot->getPlot());
    ui->mdiArea->addSubWindow(subwTemp);
    subwTemp->resize(600, 400);
    subwTemp->hide();

    mpMemoryplot->initPlot(mpMemoryplot->getParent());
    subwMemory->setWidget(mpMemoryplot->getPlot());
    ui->mdiArea->addSubWindow(subwMemory);
    subwMemory->resize(600, 400);
    subwMemory->hide();

    mpUtilplot->initPlot(mpUtilplot->getParent());
    subwUtil->setWidget(mpUtilplot->getPlot());
    ui->mdiArea->addSubWindow(subwUtil);
    subwUtil->resize(600, 400);
    subwUtil->hide();

    subwHeatmap->setWidget(mpHeatmap);
    ui->mdiArea->addSubWindow(subwHeatmap);
    subwHeatmap->resize(600, 400);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mpPowerplot;
    delete mpTempplot;
    delete mpClockplot;
    delete mpUtilplot;
    delete mpMemoryplot;
    delete subwClock;
    delete subwTemp;
    delete subwUtil;
    delete subwMemory;
    delete subwPower;

    delete mpGuiTimer;
    delete mpTimer;
}

void MainWindow::connectActions()
{
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(start()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(stop()));

    connect(ui->action_Power, SIGNAL(triggered()),this, SLOT(showPower()));
    connect(ui->action_Temperature, SIGNAL(triggered()),this, SLOT(showTemp()));
    connect(ui->action_Clock, SIGNAL(triggered()),this, SLOT(showClock()));
    connect(ui->action_Utilization, SIGNAL(triggered()), this, SLOT(showUtil()));
    connect(ui->action_Memory, SIGNAL(triggered()), this, SLOT(showMemory()));

    connect(mpTimer, SIGNAL(timeout()), this, SLOT(requestData()));

    connect(ui->horizontalSlider_guiRate, SIGNAL(valueChanged(int)), this, SLOT(setGuiInterval(int)));
    connect(ui->horizontalSlider_dataPlot, SIGNAL(valueChanged(int)), this, SLOT(setInterval(int)));

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
    connect(mpGuiTimer, SIGNAL(timeout()), mpPowerplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updatePower()));

    mpPowerplot->getPlot()->show();

}

void MainWindow::showTemp()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateTemp()));

    mpTempplot->getPlot()->show();

}

void MainWindow::showClock()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpClockplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateClock()));

    mpClockplot->getPlot()->show();

}

void MainWindow::showUtil()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpUtilplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateUtil()));

    mpUtilplot->getPlot()->show();
}

void MainWindow::showMemory()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpMemoryplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateMemory()));

    mpMemoryplot->getPlot()->show();
}

void MainWindow::updatePower()
{
    mpPowerplot->updateValues(mClient.mValues);
}

void MainWindow::updateClock()
{
    mpClockplot->updateValues(mClient.mValues);
}

void MainWindow::updateTemp()
{
    mpTempplot->updateValues(mClient.mValues);
}

void MainWindow::updateMemory()
{
    mpMemoryplot->updateValues(mClient.mValues);
}

void MainWindow::updateUtil()
{
    mpUtilplot->updateValues(mClient.mValues);
}
