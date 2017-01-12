#include "gui/QMSMpowerplot.h"

QMSMPowerPlot::QMSMPowerPlot(QWidget *parent):
    QWidget(parent)
{
    initPlot(parent);
}

QMSMPowerPlot::~QMSMPowerPlot()
{
    delete plot;
    delete cpu0;
    delete cpu1;
    delete gpu;
    delete fpga;
    delete mic;
    delete system;
}

void QMSMPowerPlot::initPlot(QWidget* parent)
{
    plot    = new QwtPlot(parent);
    cpu0    = new QwtPlotCurve("CPU0");
    cpu1    = new QwtPlotCurve("CPU1");
    gpu     = new QwtPlotCurve("GPU");
    fpga    = new QwtPlotCurve("FPGA");
    mic     = new QwtPlotCurve("MIC");
    system  = new QwtPlotCurve("System");

//    cpu0->setSamples(x,y, 101);
    cpu0->attach(plot);
    cpu1->attach(plot);
    gpu->attach(plot);
    fpga->attach(plot);
    mic->attach(plot);
    system->attach(plot);

}

QWidget* QMSMPowerPlot::getPlot()
{
    return plot;
}
