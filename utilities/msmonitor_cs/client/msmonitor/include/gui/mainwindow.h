#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <vector>
#include "QMSMpowerplot.h"
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
    CClient mClient;
    QTimer* mpTimer;
    int mInterval;
    void connectActions();

public slots:
    void start();
    void stop();
    void reset();
    void requestData();

};

#endif // MAINWINDOW_H
