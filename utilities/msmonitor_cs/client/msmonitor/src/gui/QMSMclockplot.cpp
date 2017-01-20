#include "gui/QMSMclockplot.h"

QMSMClockPlot::QMSMClockPlot(QWidget *parent):
    QMSMplot(parent)
{
    initPlot(parent);
}


QMSMClockPlot::~QMSMClockPlot()
{
    delete mpPlot;

    delete mpClockCpu0;
    delete mpClockCpu1;
    delete mpClockGpuCore;
    delete mpClockGpuMem;
    delete mpClockMicCore;
    delete mpClockMicMem;
}

QWidget* QMSMClockPlot::getPlot()
{
    return QMSMplot::getPlot();
}

void QMSMClockPlot::initPlot(QWidget* parent)
{
    mpPlot          = new QwtPlot(parent);
    mpClockCpu0     = new QwtPlotCurve("CPU0");
    mpClockCpu1     = new QwtPlotCurve("CPU1");
    mpClockGpuCore  = new QwtPlotCurve("GPU Core");
    mpClockGpuMem   = new QwtPlotCurve("GPU Mem");
    mpClockMicCore  = new QwtPlotCurve("MIC Core");
    mpClockMicMem   = new QwtPlotCurve("MIC Mem");

    mpPlot->setTitle("Clock");

    mpClockCpu0->setPen(* new QPen(Qt::blue));
    mpClockCpu1->setPen(* new QPen(Qt::red));
    mpClockGpuCore->setPen(* new QPen(Qt::green));
    mpClockGpuMem->setPen(* new QPen(Qt::gray));
    mpClockMicCore->setPen(* new QPen(Qt::yellow));
    mpClockMicMem->setPen(* new QPen(Qt::black));

    mpClockCpu0->attach(mpPlot);
    mpClockCpu1->attach(mpPlot);
    mpClockGpuCore->attach(mpPlot);
    mpClockGpuMem->attach(mpPlot);
    mpClockMicCore->attach(mpPlot);
    mpClockMicMem->attach(mpPlot);

}


void QMSMClockPlot::redraw()
{
    int size = mTimevalues.size();

    mpClockCpu0->setSamples(mTimevalues.data(), mClockCpu0values.data(), size);
    mpClockCpu1->setSamples(mTimevalues.data(), mClockCpu1values.data(), size);
    mpClockGpuCore->setSamples(mTimevalues.data(), mClockGpuCorevalues.data(), size);
    mpClockGpuMem->setSamples(mTimevalues.data(), mClockGpuMemvalues.data(), size);
    mpClockMicCore->setSamples(mTimevalues.data(), mClockMicCorevalues.data(), size);
    mpClockMicMem->setSamples(mTimevalues.data(), mClockMicMemvalues.data(), size);

    mpPlot->replot();
}

void QMSMClockPlot::updateValues(std::vector<double> &values)
{
    mTimevalues.push_back(values[X]);
    mClockCpu0values.push_back(values[YClockCpu0]);
    mClockCpu1values.push_back(values[YClockCpu1]);
    mClockGpuCorevalues.push_back(values[YClockGpuCore]);
    mClockGpuMemvalues.push_back(values[YClockGpuMem]);
    mClockMicCorevalues.push_back(values[YClockMicCore]);
    mClockMicMemvalues.push_back(values[YClockMicMem]);
}
