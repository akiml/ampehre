#include "gui/QMSMtemperatureplot.h"

QMSMTemperaturePlot::QMSMTemperaturePlot(QWidget *parent):
    QMSMplot(parent)
{
    //nothing to be done
}

QMSMTemperaturePlot::~QMSMTemperaturePlot()
{
}

QWidget* QMSMTemperaturePlot::getPlot()
{
    return QMSMplot::getPlot();
}


void QMSMTemperaturePlot::initPlot()
{
    mpCpu0      = new QwtPlotCurve("CPU0");
    mpCpu1      = new QwtPlotCurve("CPU1");
    mpGpu0      = new QwtPlotCurve("GPU");
    mpFpga0     = new QwtPlotCurve("FPGA I");
    mpFpga1     = new QwtPlotCurve("FPGA M");
    mpMic0      = new QwtPlotCurve("MIC");
    mpSystem    = new QwtPlotCurve("System");

    mpPlot->setTitle("Temperature");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Temperature [C]");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    resetPen();

    mpCpu0->attach(mpPlot);
    mpCpu1->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpFpga0->attach(mpPlot);
    mpFpga1->attach(mpPlot);
    mpMic0->attach(mpPlot);
    mpSystem->attach(mpPlot);

    makeGrid();
}

void QMSMTemperaturePlot::redraw()
{
    int size = mTimevalues.size();

    mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
    mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
    mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
    mpFpga0->setSamples(mTimevalues.data(), mFpga0values.data(), size);
    mpFpga1->setSamples(mTimevalues.data(), mFpga1values.data(), size);
    mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
    mpSystem->setSamples(mTimevalues.data(), mSystemvalues.data(), size);

    mpPlot->replot();
}

void QMSMTemperaturePlot::updateValues(std::vector<double> &values)
{
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YTempCpu0]);
    mCpu1values.push_back(values[YTempCpu1]);
    mGpu0values.push_back(values[YTempGpu]);
    mFpga0values.push_back(values[YTempFpgaI]);
    mFpga1values.push_back(values[YTempFpgaM]);
    mMic0values.push_back(values[YTempMicDie]);
    mSystemvalues.push_back(values[YTempSystem]);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);

}

void QMSMTemperaturePlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpCpu1->setPen(*mpPaintCpu1);
    mpGpu0->setPen(*mpPaintGpu0);
    mpFpga0->setPen(*mpPaintFpga0);
    mpFpga1->setPen(*mpPaintFpga1);
    mpMic0->setPen(*mpPaintMic0);
    mpSystem->setPen(*mpPaintSystem);
}

