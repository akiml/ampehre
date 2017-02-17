#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <vector>
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "QMSMpowerplot.h"
#include "QMSMtemperatureplot.h"
#include "QMSMclockplot.h"
#include "QMSMmemoryplot.h"
#include "QMSMutilplot.h"
#include "QMSMheatmap.h"
#include "CClient.hpp"
#include "QMSMsettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
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

    QMdiSubWindow* subwPower;
    QMdiSubWindow* subwTemp;
    QMdiSubWindow* subwClock;
    QMdiSubWindow* subwUtil;
    QMdiSubWindow* subwMemory;
    QMdiSubWindow* subwHeatmapUtil;
    QMdiSubWindow* subwHeatmapTemp;
    QMdiSubWindow* subwSettings;

    CClient mClient;
    QTimer* mpTimer;
    QTimer* mpGuiTimer;
    int mPlotInterval;
    int mHeatmapInterval;
    int mGuiInterval;
    void connectActions();
    void addPlot(QMSMplot* plot, QMdiSubWindow* subw);
    void addHeatmap(QMdiSubWindow* heat, QVBoxLayout* layout);
    void setSlider();

public slots:
    void start();
    void stop();
    void reset();
    void requestData();

    void setGuiInterval(int val);
    void setInterval(int val);
    void setFreq(std::vector<int>& fv);



    void showPower();
    void showTemp();
    void showClock();
    void showUtil();
    void showMemory();
    void showHeatmapUtil();
    void showHeatmapTemp();
    void showSettings();

    void updatePower();
    void updateTemp();
    void updateClock();
    void updateUtil();
    void updateMemory();
    void updateHeatmapUtil();
    void updateHeatmapTemp();

};

#endif // MAINWINDOW_H
