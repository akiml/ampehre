/*
 * QMSMutilplot.cpp
 *
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 * encoding: UTF-8
 * tab size: 4
 *
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation
 */


#include "gui/QMSMutilplot.h"

QMSMUtilPlot::QMSMUtilPlot(int linewidth, int maxData, int width, int height, QWidget *parent):
    QMSMplot(UTIL, linewidth, maxData, width, height, parent)
{
    //nothing to be done
}


QMSMUtilPlot::~QMSMUtilPlot()
{
    std::cout << "delete utilPlot" << std::endl;

}

QWidget* QMSMUtilPlot::getPlot()
{
    return QMSMplot::getPlot();
}

void QMSMUtilPlot::initPlot()
{
    mpCpu0      = new QwtPlotCurve("CPU");
    mpGpu0      = new QwtPlotCurve("GPU Core");
    mpGpu1      = new QwtPlotCurve("GPU Memory");
    mpFpga0     = new QwtPlotCurve("Compute FPGA");
    mpMic0      = new QwtPlotCurve("MIC");

    mpPlot->setTitle("Utilization");
    setWindowTitle("Utilization");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Server uptime [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Utilization");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    resetPen();

    mpCpu0->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpGpu1->attach(mpPlot);
    mpFpga0->attach(mpPlot);
    mpMic0->attach(mpPlot);

    makeZoomable();
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
    while(mTimevalues.size() > maxData)
    {
        mTimevalues.erase(mTimevalues.begin());
        mCpu0values.erase(mCpu0values.begin());
        mGpu0values.erase(mGpu0values.begin());
        mGpu1values.erase(mGpu1values.begin());
        mFpga0values.erase(mFpga0values.begin());
        mMic0values.erase(mMic0values.begin());
    }
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YUtilCpu]);
    mGpu0values.push_back(values[YUtilGpuCore]);
    mGpu1values.push_back(values[YUtilGpuMem]);
    mFpga0values.push_back(values[YUtilFpga]);
    mMic0values.push_back(values[YUtilMic]);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);

}

void QMSMUtilPlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpGpu0->setPen(*mpPaintGpu0);
    mpGpu1->setPen(*mpPaintGpu1);
    mpFpga0->setPen(*mpPaintFpga0);
    mpMic0->setPen(*mpPaintMic0);
}
