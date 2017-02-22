#include "gui/QMSMutilplot.h"

QMSMUtilPlot::QMSMUtilPlot(QWidget *parent):
    QMSMplot(parent)
{
    //nothing to be done
}


QMSMUtilPlot::~QMSMUtilPlot()
{
}

QWidget* QMSMUtilPlot::getPlot()
{
    return QMSMplot::getPlot();
}


void QMSMUtilPlot::initPlot(QWidget *parent)
{
    mpPlot      = new QwtPlot(parent);
    mpCpu0      = new QwtPlotCurve("CPU");
    mpGpu0      = new QwtPlotCurve("GPU Core");
    mpGpu1      = new QwtPlotCurve("GPU Memory");
    mpFpga0     = new QwtPlotCurve("Compute FPGA");
    mpMic0      = new QwtPlotCurve("MIC");

    mpPlot->setTitle("Utilization");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Utilization");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    mpCpu0->setPen(*mpPaintCpu0);
    mpGpu0->setPen(*mpPaintGpu0);
    mpGpu1->setPen(*mpPaintGpu1);
    mpFpga0->setPen(*mpPaintFpga0);
    mpMic0->setPen(*mpPaintMic0);

    mpCpu0->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpGpu1->attach(mpPlot);
    mpFpga0->attach(mpPlot);
    mpMic0->attach(mpPlot);

    makeGrid();

}


void QMSMUtilPlot::redraw()
{
    int size = mTimevalues.size();

    mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
    mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
    mpGpu1->setSamples(mTimevalues.data(), mGpu1values.data(), size);
    mpFpga0->setSamples(mTimevalues.data(), mFpga0values.data(), size);
    mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);

    mpPlot->replot();
}

void QMSMUtilPlot::updateValues(std::vector<double> &values)
{
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YUtilCpu]);
    mGpu0values.push_back(values[YUtilGpuCore]);
    mGpu1values.push_back(values[YUtilGpuMem]);
    mFpga0values.push_back(values[YUtilFpga]);
    mMic0values.push_back(values[YUtilMic]);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);

}
