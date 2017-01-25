#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <vector>
#include <QMdiSubWindow>
#include "QMSMpowerplot.h"
#include "QMSMtemperatureplot.h"
#include "QMSMclockplot.h"
#include "QMSMmemoryplot.h"
#include "QMSMutilplot.h"
#include "CClient.hpp"

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

    QMdiSubWindow* subwPower;
    QMdiSubWindow* subwTemp;
    QMdiSubWindow* subwClock;
    QMdiSubWindow* subwUtil;
    QMdiSubWindow* subwMemory;

    CClient mClient;
    QTimer* mpTimer;
    QTimer* mpGuiTimer;
    int mInterval;
    int mGuiInterval;
    void connectActions();

public slots:
    void start();
    void stop();
    void reset();
    void requestData();

    void setGuiInterval(int val);
    void setInterval(int val);

    void showPower();
    void showTemp();
    void showClock();
    void showUtil();
    void showMemory();

    void updatePower();
    void updateTemp();
    void updateClock();
    void updateUtil();
    void updateMemory();

};

#endif // MAINWINDOW_H
