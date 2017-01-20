#include "gui/QMSMtemperatureplot.h"

QMSMTemperaturePlot::QMSMTemperaturePlot(QWidget *parent):
    QMSMplot(parent)
{
    initPlot(parent);
}

QMSMTemperaturePlot::~QMSMTemperaturePlot()
{
    delete mpPlot;

    delete mpTempCpu0;
    delete mpTempCpu1;
    delete mpTempGpu;
    delete mpTempFpgaI;
    delete mpTempFpgaM;
    delete mpTempMicDie;
    delete mpTempSystem;
}

QWidget* QMSMTemperaturePlot::getPlot()
{
    return QMSMplot::getPlot();
}

void QMSMTemperaturePlot::initPlot(QWidget *parent)
{
    mpPlot          = new QwtPlot(parent);
    mpTempCpu0      = new QwtPlotCurve("CPU0");
    mpTempCpu1      = new QwtPlotCurve("CPU1");
    mpTempGpu       = new QwtPlotCurve("GPU");
    mpTempFpgaI     = new QwtPlotCurve("FPGA I");
    mpTempFpgaM     = new QwtPlotCurve("FPGA M");
    mpTempMicDie    = new QwtPlotCurve("MIC");
    mpTempSystem    = new QwtPlotCurve("System");

    mpPlot->setTitle("Temperature");

    mpTempCpu0->setPen(* new QPen(Qt::blue));
    mpTempCpu1->setPen(* new QPen(Qt::red));
    mpTempGpu->setPen(* new QPen(Qt::green));
    mpTempFpgaI->setPen(* new QPen(Qt::gray));
    mpTempFpgaM->setPen(* new QPen(Qt::gray));
    mpTempMicDie->setPen(* new QPen(Qt::yellow));
    mpTempSystem->setPen(* new QPen(Qt::black));

    mpTempCpu0->attach(mpPlot);
    mpTempCpu1->attach(mpPlot);
    mpTempGpu->attach(mpPlot);
    mpTempFpgaI->attach(mpPlot);
    mpTempFpgaM->attach(mpPlot);
    mpTempMicDie->attach(mpPlot);
    mpTempSystem->attach(mpPlot);
}

void QMSMTemperaturePlot::redraw()
{
    int size = mTimevalues.size();

    mpTempCpu0->setSamples(mTimevalues.data(), mTempCpu0values.data(), size);
    mpTempCpu1->setSamples(mTimevalues.data(), mTempCpu1values.data(), size);
    mpTempGpu->setSamples(mTimevalues.data(), mTempGpuvalues.data(), size);
    mpTempFpgaI->setSamples(mTimevalues.data(), mTempFpgaIvalues.data(), size);
    mpTempFpgaM->setSamples(mTimevalues.data(), mTempFpgaMvalues.data(), size);
    mpTempMicDie->setSamples(mTimevalues.data(), mTempMicDievalues.data(), size);
    mpTempSystem->setSamples(mTimevalues.data(), mTempSystemvalues.data(), size);

    mpPlot->replot();
}

void QMSMTemperaturePlot::updateValues(std::vector<double> &values)
{
    mTimevalues.push_back(values[X]);
    mTempCpu0values.push_back(values[YTempCpu0]);
    mTempCpu1values.push_back(values[YTempCpu1]);
    mTempGpuvalues.push_back(values[YTempGpu]);
    mTempFpgaIvalues.push_back(values[YTempFpgaI]);
    mTempFpgaMvalues.push_back(values[YTempFpgaM]);
    mTempMicDievalues.push_back(values[YTempMicDie]);
    mTempSystemvalues.push_back(values[YTempSystem]);
}

