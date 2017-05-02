/*
 * QMSMclockplot.cpp
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

#include "gui/QMSMclockplot.h"


QMSMClockPlot::QMSMClockPlot(int linewidth, int maxData, int width, int height, QWidget *parent):
    QMSMplot(CLOCK,linewidth, maxData, width, height, parent)
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
    setWindowTitle("Clock");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Server uptime [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Frequency [1/s]");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    resetPen();

    mpCpu0->attach(mpPlot);
    mpCpu1->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpGpu1->attach(mpPlot);
    mpMic0->attach(mpPlot);
    mpMic1->attach(mpPlot);

    makeZoomable();
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
    while(mTimevalues.size() > maxData)
    {
        mTimevalues.erase(mTimevalues.begin());
        mCpu0values.erase(mCpu0values.begin());
        mCpu1values.erase(mCpu1values.begin());
        mGpu0values.erase(mGpu0values.begin());
        mGpu1values.erase(mGpu1values.begin());
        mMic0values.erase(mMic0values.begin());
        mMic1values.erase(mMic1values.begin());
    }
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
