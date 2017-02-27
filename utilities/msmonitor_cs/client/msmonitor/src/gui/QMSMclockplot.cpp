#include "gui/QMSMclockplot.h"


QMSMClockPlot::QMSMClockPlot(QWidget *parent):
    QMSMplot(parent)
{
    //nothing to be done
}


QMSMClockPlot::~QMSMClockPlot()
{
}

QWidget* QMSMClockPlot::getPlot()
{
    return QMSMplot::getPlot();
}


void QMSMClockPlot::initPlot()
{
    mpCpu0      = new QwtPlotCurve("CPU0");
    mpCpu1      = new QwtPlotCurve("CPU1");
    mpGpu0      = new QwtPlotCurve("GPU Core");
    mpGpu1      = new QwtPlotCurve("GPU Mem");
    mpMic0      = new QwtPlotCurve("MIC Core");
    mpMic1      = new QwtPlotCurve("MIC Mem");

    mpPlot->setTitle("Clock");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Frequency [1/s]");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    resetPen();

    mpCpu0->attach(mpPlot);
    mpCpu1->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpGpu1->attach(mpPlot);
    mpMic0->attach(mpPlot);
    mpMic1->attach(mpPlot);

    makeGrid();

}


void QMSMClockPlot::redraw()
{
    int size = mTimevalues.size();

    mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
    mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
    mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
    mpGpu1->setSamples(mTimevalues.data(), mGpu1values.data(), size);
    mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
    mpMic1->setSamples(mTimevalues.data(), mMic1values.data(), size);

    mpPlot->replot();
}

void QMSMClockPlot::updateValues(std::vector<double> &values)
{
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YClockCpu0]);
    mCpu1values.push_back(values[YClockCpu1]);
    mGpu0values.push_back(values[YClockGpuCore]);
    mGpu1values.push_back(values[YClockGpuMem]);
    mMic0values.push_back(values[YClockMicCore]);
    mMic1values.push_back(values[YClockMicMem]);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);
}

void QMSMClockPlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpCpu1->setPen(*mpPaintCpu1);
    mpGpu0->setPen(*mpPaintGpu0);
    mpGpu1->setPen(*mpPaintGpu1);
    mpMic0->setPen(*mpPaintMic0);
    mpMic1->setPen(*mpPaintMic1);
}
