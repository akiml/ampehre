/*
 * mainwindow.cpp
 *
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 * encoding: UTF-8
 * tab size: 4
 *
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation
 */


#include "gui/mainwindow.h"
#include "ui_mainwindow.h"

#include <cstdlib>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mpConfig(new CConfig(QString(getenv("HOME")) + "/default.conf")),
    mpPowerplot (new QMSMPowerPlot(mpConfig->lineWidth, mpConfig->maxDataRecord, mpConfig->width, mpConfig->height, parent)),
    mpTempplot (new QMSMTemperaturePlot(mpConfig->lineWidth, mpConfig->maxDataRecord, mpConfig->width, mpConfig->height, parent)),
    mpClockplot (new QMSMClockPlot(mpConfig->lineWidth, mpConfig->maxDataRecord, mpConfig->width, mpConfig->height, parent)),
    mpUtilplot(new QMSMUtilPlot(mpConfig->lineWidth, mpConfig->maxDataRecord, mpConfig->width, mpConfig->height, parent)),
    mpMemoryplot(new QMSMMemoryPlot(mpConfig->lineWidth, mpConfig->maxDataRecord, mpConfig->width, mpConfig->height, parent)),
    mpSettings(new QMSMSettings(parent)),
    mpHeatmapCpu(new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime,"CPU\n[%]\n", 0, 100)),
    mpHeatmapGpuCore(new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "GPU\nCore\n[%]", 0 ,100)),
    mpHeatmapGpuMemory(new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "GPU\nMemory\n[%]", 0 ,100)),
    mpHeatmapFpga(new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "Compute\nFPGA\n[%]", 0 ,100)),
    mpHeatmapMic(new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "MIC\n[%]\n", 0 ,100)),
    mpTempCpu0( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "CPU0\n\n[\xB0 C]", 0, 100)),
    mpTempCpu1( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "CPU1\n\n[\xB0 C]", 0, 100)),
    mpTempGpu( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "GPU\n\n[\xB0 C]", 0, 100)),
    mpTempFpgaCompute( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "Comp\nFPGA\n[\xB0 C]", 0, 100)),
    mpTempFpgaInterface( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "Inter\nFPGA\n[\xB0 C]", 0, 100)),
    mpTempMic( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "Mic\nDie\n[\xB0 C]", 40, 140)),
    mpTempSystem( new QMSMHeatmap(parent,mpConfig->alpha, mpConfig->intervalEnd, mpConfig->intervalStart, mpConfig->updateTime, "Main-\nboard\n[\xB0 C]", 20, 40)),
    mpSystemOverview (new QMSMSystemOverview()),
    subwPower(new QMdiSubWindow()),
    subwTemp(new QMdiSubWindow()),
    subwClock(new QMdiSubWindow()),
    subwUtil(new QMdiSubWindow()),
    subwMemory(new QMdiSubWindow()),
    subwHeatmapUtil( new QMdiSubWindow()),
    subwHeatmapTemp( new QMdiSubWindow()),
    subwSettings( new QMdiSubWindow()),
    mClient(CClient()),
    mpTimer(new QTimer()),
    mpGuiTimer(new QTimer()),
    mPlotInterval(mpConfig->plot),
    mGuiInterval(mpConfig->gui),
    mSafetyTimeServer(500)
{
    ui->setupUi(this);
    connectActions();
    setInitSettings();


    addPlot((QMSMplot*)mpPowerplot, subwPower);
    addPlot((QMSMplot*)mpClockplot, subwClock);
    addPlot((QMSMplot*)mpTempplot, subwTemp);
    addPlot((QMSMplot*)mpMemoryplot, subwMemory);
    addPlot((QMSMplot*)mpUtilplot, subwUtil);
    ui->mdiArea->addSubWindow(mpSystemOverview);
    mpSystemOverview->hide();

    subwSettings->setWidget(mpSettings);
    ui->mdiArea->addSubWindow(subwSettings);
    subwSettings->hide();
    subwSettings->resize(400, 600);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(mpHeatmapCpu);
    layout->addWidget(mpHeatmapGpuCore);
    layout->addWidget(mpHeatmapGpuMemory);
    layout->addWidget(mpHeatmapFpga);
    layout->addWidget(mpHeatmapMic);

    addHeatmap(subwHeatmapUtil, layout);

    QVBoxLayout* layout1 = new QVBoxLayout;
    layout1->addWidget(mpTempCpu0);
    layout1->addWidget(mpTempCpu1);
    layout1->addWidget(mpTempGpu);
    layout1->addWidget(mpTempFpgaCompute);
    layout1->addWidget(mpTempFpgaInterface);
    layout1->addWidget(mpTempMic);
    layout1->addWidget(mpTempSystem);

    addHeatmap(subwHeatmapTemp, layout1);

}

MainWindow::~MainWindow()
{
    std::cout << "delete mainwindow" << std::endl;
    delete ui;

    delete subwClock;
    delete subwTemp;
    delete subwUtil;
    delete subwMemory;
    delete subwPower;
    delete subwHeatmapUtil;
    delete subwHeatmapTemp;
    delete subwSettings;

    delete mpConfig;
    delete mpSystemOverview;

    delete mpGuiTimer;
    delete mpTimer;
    std::cout << "reached end" << std::endl;
}

void MainWindow::setInitSettings()
{
    setSlider();
    mpSettings->emit_guiRate(mpConfig->gui);
    mpSettings->setSaveData(mpConfig->maxDataRecord);
    mpSettings->set_ip(mpConfig->serverIP);
    mpSettings->set_port(mpConfig->serverPort);

    mpPowerplot->setLineWidth(mpConfig->lineWidth);
    mpTempplot->setLineWidth(mpConfig->lineWidth);
    mpUtilplot->setLineWidth(mpConfig->lineWidth);
    mpClockplot->setLineWidth(mpConfig->lineWidth);
    mpMemoryplot->setLineWidth(mpConfig->lineWidth);
}

void MainWindow::setSlider()
{
    std::vector<int> v;
    v.resize(SLIDER_VALUESIZE);
    v[GUI] = mGuiInterval;
    v[DATA_PLOT] = mPlotInterval;

    mpSettings->setSliderPos(v);
}

void MainWindow::addPlot(QMSMplot *plot, QMdiSubWindow *subw)
{
    plot->initPlot();
    subw->setWidget(plot);

    ui->mdiArea->addSubWindow(subw);
    subw->resize(plot->width(), plot->height());
    subw->hide();
}

void MainWindow::addHeatmap(QMdiSubWindow *heat, QVBoxLayout *layout)
{
    delete heat->layout();
    heat->setLayout(layout);
    ui->mdiArea->addSubWindow(heat);
    heat->resize(600, 800);
    heat->setFixedHeight(800);
    heat->hide();
}

void MainWindow::exportConfig()
{
    mpConfig->serverIP = mpSettings->get_ip();
    mpConfig->serverPort = mpSettings->get_port();
    mpConfig->plot = mPlotInterval;
    mpConfig->gui = mGuiInterval;
    //lineWidth export not possible -> can be changed individually
    mpConfig->maxDataRecord= mpSettings->get_maxData();
    //width
    //height has to be set manually
    //rest cannot be set from gui
    mpConfig->exportConfig();

}

void MainWindow::importConfig()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getOpenFileName(NULL, "Load config file","","*.conf");
    if(mpTimer->isActive())
    {
        QMessageBox msgBox;
        msgBox.setText("Please stop current measurement before importing new configuration!");
        msgBox.exec();
        return;
    }
    else
    {
        mpConfig->importConfig(filename);
        setInitSettings();
    }
}

void MainWindow::setServerIP(QString v)
{
    mpConfig->serverIP = v;
}

void MainWindow::setServerPort(int v)
{
    mpConfig->serverPort = v;
}

void MainWindow::connectActions()
{

    connect(mpSettings, SIGNAL(signal_start()), this, SLOT(start()));
    connect(mpSettings, SIGNAL(signal_stop()), this, SLOT(stop()));
    connect(mpSettings, SIGNAL(signal_saveData(int)), this, SLOT(setMaxData(int)));
    connect(mpSettings, SIGNAL(signal_exportConfig()), this, SLOT(exportConfig()));
    connect(mpSettings, SIGNAL(signal_loadConfig()), this, SLOT(importConfig()));
    connect(mpSettings, SIGNAL(signal_ipChanged(QString)), this, SLOT(setServerIP(QString)));
    connect(mpSettings, SIGNAL(signal_portChanged(int)), this, SLOT(setServerPort(int)));



    connect((QMSMplot*)mpPowerplot, SIGNAL(signal_export(QMSMplot*)), mpConfig, SLOT(exportPlotToCSV(QMSMplot*)));
    connect((QMSMplot*)mpTempplot, SIGNAL(signal_export(QMSMplot*)), mpConfig, SLOT(exportPlotToCSV(QMSMplot*)));
    connect((QMSMplot*)mpClockplot, SIGNAL(signal_export(QMSMplot*)), mpConfig, SLOT(exportPlotToCSV(QMSMplot*)));
    connect((QMSMplot*)mpUtilplot, SIGNAL(signal_export(QMSMplot*)), mpConfig, SLOT(exportPlotToCSV(QMSMplot*)));
    connect((QMSMplot*)mpMemoryplot, SIGNAL(signal_export(QMSMplot*)), mpConfig, SLOT(exportPlotToCSV(QMSMplot*)));




    connect(mpSettings, SIGNAL(signal_guiRate(int)), this, SLOT(setGuiInterval(int)));
    connect(mpSettings, SIGNAL(signal_dataPlot(int)), this, SLOT(setInterval(int)));

    connect(ui->action_Power, SIGNAL(triggered()),this, SLOT(showPower()));
    connect(ui->action_Temperature, SIGNAL(triggered()),this, SLOT(showTemp()));
    connect(ui->action_Clock, SIGNAL(triggered()),this, SLOT(showClock()));
    connect(ui->action_Utilization, SIGNAL(triggered()), this, SLOT(showUtil()));
    connect(ui->action_Memory, SIGNAL(triggered()), this, SLOT(showMemory()));
    connect(ui->action_Settings, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(ui->action_Utilization_2, SIGNAL(triggered()), this, SLOT(showHeatmapUtil()));
    connect(ui->action_Temperature_2, SIGNAL(triggered()), this, SLOT(showHeatmapTemp()));
    connect(ui->action_Systemoverview, SIGNAL(triggered()), this, SLOT(showSystemOverview()));
    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));

    connect(mpTimer, SIGNAL(timeout()), this, SLOT(requestData()));

}

void MainWindow::start()
{
    std::vector<int> values;
    CProtocolC::addAll(values);
    if(mClient.registerToServer(values, mpConfig->serverPort, mpConfig->serverIP.toStdString())  < 0)
    {
        mpSettings->stop();
        QMessageBox msgBox;
        msgBox.setText("Error connecting to Server.\nPlease check if Server is running or its Port and IP address are correct.");
        msgBox.exec();
        return;
    }

    std::vector<uint64_t> frequencies;
    mClient.getFreq(frequencies);
    mpSettings->setFreqLabels(frequencies);

    mpTimer->setInterval(mPlotInterval);
    mpGuiTimer->setInterval(mGuiInterval);
    mpTimer->start();
    mpGuiTimer->start();
}

void MainWindow::stop()
{
    if(mpTimer->isActive())
    {
        mpTimer->stop();
        mClient.terminate();
    }
}

void MainWindow::requestData()
{
    QTime exchangeTimer;
    exchangeTimer.start();

    if(mClient.requestData() < 0)
    {
        qDebug() << "Error connecting to Server!";
        mpSettings->stop();
        mpTimer->stop();
        QMessageBox msgBox;
        msgBox.setText("Error connecting to Server! Possible Server Crash detected.");
        msgBox.exec();
        return;
    }
    controlTimer(exchangeTimer.elapsed());
}

void MainWindow::controlTimer(const int time)
{
    if(time >= mpTimer->interval()-mSafetyTimeServer)
    {
        mPlotInterval = time+mSafetyTimeServer+20;
        mpTimer->setInterval(mPlotInterval);
        setSlider();
        QMessageBox msgBox;
        msgBox.setText("Timer had to be set automatically, due to network delay!");
        msgBox.exec();
    }
}

void MainWindow::setMaxData(int val)
{
    mpPowerplot->setMaxData(val);
    mpTempplot->setMaxData(val);
    mpUtilplot->setMaxData(val);
    mpClockplot->setMaxData(val);
    mpMemoryplot->setMaxData(val);
}

void MainWindow::reset(){}

void MainWindow::setGuiInterval(int val)
{
    if(val > 0)
    {
        this->mGuiInterval = val;
        mpGuiTimer->setInterval(mGuiInterval);
    }
}


void MainWindow::setInterval(int val)
{
    if(val > 0)
    {
        this->mPlotInterval = val;
        mpTimer->setInterval(mPlotInterval);
    }
}


void MainWindow::showPower()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpPowerplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updatePower()));

    mpPowerplot->show();

}

void MainWindow::showTemp()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpTempplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateTemp()));

    mpTempplot->show();

}

void MainWindow::showClock()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpClockplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateClock()));

    mpClockplot->show();

}

void MainWindow::showUtil()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpUtilplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateUtil()));

    mpUtilplot->show();
}

void MainWindow::showMemory()
{
    connect(mpGuiTimer, SIGNAL(timeout()), mpMemoryplot, SLOT(redraw()));
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateMemory()));

    mpMemoryplot->show();
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

void MainWindow::showSettings()
{
    subwSettings->show();
    subwSettings->widget()->show();
}

void MainWindow::showSystemOverview()
{
    connect(mpGuiTimer, SIGNAL(timeout()), this, SLOT(updateSystemOverview()));
    mpSystemOverview->show();
}

void MainWindow::updateSystemOverview()
{
    mpSystemOverview->update(mClient.mValues);
    mpSystemOverview->updatePID(mClient.mValues_pid);
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
