#include "gui/QMSMpowerplot.h"

QMSMPowerPlot::QMSMPowerPlot(int linewidth, int maxData, int width, int height, QWidget *parent):
    QMSMplot(linewidth, maxData, width, height, parent)
{
    //nothing to be done
}

QMSMPowerPlot::~QMSMPowerPlot()
{
}

QWidget* QMSMPowerPlot::getPlot()
{
    return QMSMplot::getPlot();
}

void QMSMPowerPlot::initPlot()
{
    mpCpu0      = new QwtPlotCurve("CPU0");
    mpCpu1      = new QwtPlotCurve("CPU1");
    mpGpu0      = new QwtPlotCurve("GPU");
    mpFpga0     = new QwtPlotCurve("FPGA");
    mpMic0      = new QwtPlotCurve("MIC");
    mpSystem    = new QwtPlotCurve("System");

    mpPlot->setTitle("Power");
    setWindowTitle("Power");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Server uptime [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Power [W]");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    resetPen();

    mpCpu0->attach(mpPlot);
    mpCpu1->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpFpga0->attach(mpPlot);
    mpMic0->attach(mpPlot);
    mpSystem->attach(mpPlot);

    makeZoomable();
    makeGrid();

}


void QMSMPowerPlot::redraw()
{
    int size = mTimevalues.size();

    mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
    mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
    mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
    mpFpga0->setSamples(mTimevalues.data(), mFpga0values.data(), size);
    mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
    mpSystem->setSamples(mTimevalues.data(), mSystemvalues.data(), size);

    mpPlot->replot();
}

void QMSMPowerPlot::updateValues(std::vector<double> &values)
{

    while(mTimevalues.size() > maxData)
    {
        mTimevalues.erase(mTimevalues.begin());
        mCpu0values.erase(mCpu0values.begin());
        mGpu0values.erase(mGpu0values.begin());
        mFpga0values.erase(mFpga0values.begin());
        mMic0values.erase(mMic0values.begin());
        mSystemvalues.erase(mSystemvalues.begin());
    }

    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YPowerCpu0]);
    mCpu1values.push_back(values[YPowerCpu1]);
    mGpu0values.push_back(values[YPowerGpu]);
    mFpga0values.push_back(values[YPowerFpga]);
    mMic0values.push_back(values[YPowerMic]);
    mSystemvalues.push_back(values[YPowerSystem]);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);

}

void QMSMPowerPlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpCpu1->setPen(*mpPaintCpu1);
    mpGpu0->setPen(*mpPaintGpu0);
    mpFpga0->setPen(*mpPaintFpga0);
    mpMic0->setPen(*mpPaintMic0);
    mpSystem->setPen(*mpPaintSystem);
}
