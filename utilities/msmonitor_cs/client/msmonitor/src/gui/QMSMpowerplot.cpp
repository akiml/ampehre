#include "gui/QMSMpowerplot.h"

QMSMPowerPlot::QMSMPowerPlot(QWidget *parent):
    QWidget(parent)
{
    initPlot(parent);
}

QMSMPowerPlot::~QMSMPowerPlot()
{
    delete mpPlot;
    delete mpCpu0;
    delete mpCpu1;
    delete mpGpu;
    delete mpFpga;
    delete mpMic;
    delete mpSystem;
}

void QMSMPowerPlot::initPlot(QWidget* parent)
{
    mpPlot    = new QwtPlot(parent);
    mpCpu0    = new QwtPlotCurve("CPU0");
    mpCpu1    = new QwtPlotCurve("CPU1");
    mpGpu     = new QwtPlotCurve("GPU");
    mpFpga    = new QwtPlotCurve("FPGA");
    mpMic     = new QwtPlotCurve("MIC");
    mpSystem  = new QwtPlotCurve("System");

    mpCpu0->attach(mpPlot);
    mpCpu1->attach(mpPlot);
    mpGpu->attach(mpPlot);
    mpFpga->attach(mpPlot);
    mpMic->attach(mpPlot);
    mpSystem->attach(mpPlot);

}

QWidget* QMSMPowerPlot::getPlot()
{
    return mpPlot;
}

void QMSMPowerPlot::redraw(std::vector<double> &values)
{
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YPowerCpu0]);
    mCpu1values.push_back(values[YPowerCpu1]);
    mGpuvalues.push_back(values[YPowerGpu]);
    mFpgavalues.push_back(values[YPowerFpga]);
    mMicvalues.push_back(values[YPowerMic]);
    mSystemvalues.push_back(values[YPowerSystem]);

    int size = mTimevalues.size();

    mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
    mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
    mpGpu->setSamples(mTimevalues.data(), mGpuvalues.data(), size);
    mpFpga->setSamples(mTimevalues.data(), mFpgavalues.data(), size);
    mpMic->setSamples(mTimevalues.data(), mMicvalues.data(), size);
    mpSystem->setSamples(mTimevalues.data(), mSystemvalues.data(), size);

    mpPlot->replot();
}
