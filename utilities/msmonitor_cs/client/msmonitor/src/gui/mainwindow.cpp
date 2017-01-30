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
    mpHeatmapCpu(new QMSMHeatmap(parent, "CPU\n[%]\n", 0, 100)),
    mpHeatmapGpuCore(new QMSMHeatmap(parent, "GPU\nCore\n[%]", 0 ,100)),
    mpHeatmapGpuMemory(new QMSMHeatmap(parent, "GPU\nMemory\n[%]", 0 ,100)),
    mpHeatmapFpga(new QMSMHeatmap(parent, "Compute\nFPGA\n[%]", 0 ,100)),
    mpHeatmapMic(new QMSMHeatmap(parent, "MIC\n[%]\n", 0 ,100)),
    mpTempCpu0( new QMSMHeatmap(parent, "CPU0\n\n[°C]", 20, 60)),
    mpTempCpu1( new QMSMHeatmap(parent, "CPU1\n\n[°C]", 20, 60)),
    mpTempGpu( new QMSMHeatmap(parent, "GPU\n\n[°C]", 30, 70)),
    mpTempFpgaCompute( new QMSMHeatmap(parent, "Compute\nFPGA\n[°C]", 30, 70)),
    mpTempFpgaInterface( new QMSMHeatmap(parent, "Interface\nFPGA\n[°C]", 30, 70)),
    mpTempMic( new QMSMHeatmap(parent, "Mic\nDie\n[°C]", 40, 140)),
    mpTempSystem( new QMSMHeatmap(parent, "Main-\nboard\n[°C]", 20, 40)),
    subwPower(new QMdiSubWindow()),
    subwTemp(new QMdiSubWindow()),
    subwClock(new QMdiSubWindow()),
    subwUtil(new QMdiSubWindow()),
    subwMemory(new QMdiSubWindow()),
    subwHeatmapUtil( new QMdiSubWindow()),
    subwHeatmapTemp( new QMdiSubWindow()),
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

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(mpHeatmapCpu);
    layout->addWidget(mpHeatmapGpuCore);
    layout->addWidget(mpHeatmapGpuMemory);
    layout->addWidget(mpHeatmapFpga);
    layout->addWidget(mpHeatmapMic);

    delete subwHeatmapUtil->layout();
    subwHeatmapUtil->setLayout(layout);
    ui->mdiArea->addSubWindow(subwHeatmapUtil);
    subwHeatmapUtil->resize(600, 600);
    subwHeatmapUtil->setFixedHeight(600);
    subwHeatmapUtil->hide();

    QVBoxLayout* layout1 = new QVBoxLayout;
    layout1->addWidget(mpTempCpu0);
    layout1->addWidget(mpTempCpu1);
    layout1->addWidget(mpTempGpu);
    layout1->addWidget(mpTempFpgaCompute);
    layout1->addWidget(mpTempFpgaInterface);
    layout1->addWidget(mpTempMic);
    layout1->addWidget(mpTempSystem);

    delete subwHeatmapTemp->layout();
    subwHeatmapTemp->setLayout(layout1);
    ui->mdiArea->addSubWindow(subwHeatmapTemp);
    subwHeatmapTemp->resize(600, 800);
    subwHeatmapTemp->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mpPowerplot;
    delete mpTempplot;
    delete mpClockplot;
    delete mpUtilplot;
    delete mpMemoryplot;
    delete mpHeatmapCpu;
    delete mpHeatmapFpga;
    delete mpHeatmapGpuCore;
    delete mpHeatmapGpuMemory;
    delete mpHeatmapMic;

    delete mpTempCpu0;
    delete mpTempCpu1;
    delete mpTempGpu;
    delete mpTempFpgaCompute;
    delete mpTempFpgaInterface;
    delete mpTempMic;
    delete mpTempSystem;

    delete subwClock;
    delete subwTemp;
    delete subwUtil;
    delete subwMemory;
    delete subwPower;
    delete subwHeatmapUtil;
    delete subwHeatmapTemp;

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
    connect(ui->action_Utilization_2, SIGNAL(triggered()), this, SLOT(showHeatmapUtil()));
    connect(ui->action_Temperature_2, SIGNAL(triggered()), this, SLOT(showHeatmapTemp()));

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

void MainWindow::showHeatmapUtil()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpHeatmapCpu, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpHeatmapGpuCore, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpHeatmapGpuMemory, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpHeatmapFpga, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpHeatmapMic, SLOT(redraw()));

    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateHeatmapUtil()));

    subwHeatmapUtil->show();

}

void MainWindow::showHeatmapTemp()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempCpu0, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempCpu1, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempGpu, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempFpgaCompute, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempFpgaInterface, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempMic, SLOT(redraw()));
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempSystem, SLOT(redraw()));


    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateHeatmapTemp()));

    subwHeatmapTemp->show();

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

void MainWindow::updateHeatmapUtil()
{
    mpHeatmapCpu->updateValues(mClient.mValues, UtilCpu);
    mpHeatmapGpuCore->updateValues(mClient.mValues, UtilGpuCore);
    mpHeatmapGpuMemory->updateValues(mClient.mValues, UtilGpuMem);
    mpHeatmapFpga->updateValues(mClient.mValues, UtilFpga);
    mpHeatmapMic->updateValues(mClient.mValues, UtilMic);
}

void MainWindow::updateHeatmapTemp()
{
    mpTempCpu0->updateValues(mClient.mValues, TempCpu0);
    mpTempCpu1->updateValues(mClient.mValues, TempCpu1);
    mpTempGpu->updateValues(mClient.mValues, TempGpu);
    mpTempFpgaCompute->updateValues(mClient.mValues, TempFpgaCompute);
    mpTempFpgaInterface->updateValues(mClient.mValues, TempFpgaInterface);
    mpTempMic->updateValues(mClient.mValues, TempMicDie);
    mpTempSystem->updateValues(mClient.mValues, TempMainBoard);

}
