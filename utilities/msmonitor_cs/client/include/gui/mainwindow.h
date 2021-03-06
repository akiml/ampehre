/*
 * mainwindow.h
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



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QString>
#include <vector>
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "QMSMpowerplot.h"
#include "QMSMtemperatureplot.h"
#include "QMSMclockplot.h"
#include "QMSMmemoryplot.h"
#include "QMSMutilplot.h"
#include "QMSMheatmap.h"
#include "CClient.hpp"
#include "QMSMsettings.h"
#include "CConfig.h"
#include "QMSMsystemoverview.h"
#include "QMSMabout.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int port, QWidget *parent = 0);
    ~MainWindow();

private:
    int mPort;
    Ui::MainWindow *ui;
    CConfig* mpConfig;

    QMSMPowerPlot* mpPowerplot;
    QMSMTemperaturePlot* mpTempplot;
    QMSMClockPlot* mpClockplot;
    QMSMUtilPlot* mpUtilplot;
    QMSMMemoryPlot* mpMemoryplot;
    QMSMSettings* mpSettings;

    QMSMHeatmap* mpHeatmapCpu;
    QMSMHeatmap* mpHeatmapGpuCore;
    QMSMHeatmap* mpHeatmapGpuMemory;
    QMSMHeatmap* mpHeatmapFpga;
    QMSMHeatmap* mpHeatmapMic;

    QMSMHeatmap* mpTempCpu0;
    QMSMHeatmap* mpTempCpu1;
    QMSMHeatmap* mpTempGpu;
    QMSMHeatmap* mpTempFpgaCompute;
    QMSMHeatmap* mpTempFpgaInterface;
    QMSMHeatmap* mpTempMic;
    QMSMHeatmap* mpTempSystem;

    QMSMSystemOverview* mpSystemOverview;

    QMdiSubWindow* subwPower;
    QMdiSubWindow* subwTemp;
    QMdiSubWindow* subwClock;
    QMdiSubWindow* subwUtil;
    QMdiSubWindow* subwMemory;
    QMdiSubWindow* subwHeatmapUtil;
    QMdiSubWindow* subwHeatmapTemp;
    QMdiSubWindow* subwSettings;
    QMSMabout* mpAbout;

    CClient mClient;
    QTimer* mpTimer;
    QTimer* mpGuiTimer;

    int mPlotInterval;
    int mGuiInterval;
    int mSafetyTimeServer;
    int mValuePower;
    int mValueUtil;
    int mValueTemp;
    int mValueMemory;
    int mValueClock;
    void connectActions();
    void addPlot(QMSMplot* plot, QMdiSubWindow* subw);
    void addHeatmap(QMdiSubWindow* heat, QVBoxLayout* layout);
    void setSlider();
    void controlTimer(const int time);

public slots:
    void start();
    void stop();
    void reset();
    void requestData();

    void valuePower(int v);
    void valueUtil(int v);
    void valueTemp(int v);
    void valueMemory(int v);
    void valueClock(int v);

    void setGuiInterval(int val);
    void setInterval(int val);

    void setMaxData(int val);

    void showPower();
    void showTemp();
    void showClock();
    void showUtil();
    void showMemory();
    void showHeatmapUtil();
    void showHeatmapTemp();
    void showSettings();
    void showSystemOverview();
    void showAbout();

    void setInitSettings();

    void updatePower();
    void updateTemp();
    void updateClock();
    void updateUtil();
    void updateMemory();
    void updateHeatmapUtil();
    void updateHeatmapTemp();
    void updateSystemOverview();

    void exportConfig();
    void importConfig();
    void setServerIP(QString v);
    void setServerPort(int v);

};

#endif // MAINWINDOW_H
