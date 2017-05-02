/*
 * QMSMmemoryplot.cpp
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


#include "gui/QMSMmemoryplot.h"

QMSMMemoryPlot::QMSMMemoryPlot(int linewidth, int maxData, int width, int height,QWidget *parent):
    QMSMplot(MEMORY, linewidth, maxData, width, height, parent)
{
    //nothing to be done
}


QMSMMemoryPlot::~QMSMMemoryPlot()
{
}

QWidget* QMSMMemoryPlot::getPlot()
{
    return QMSMplot::getPlot();
}


void QMSMMemoryPlot::initPlot()
{
    mpCpu0      = new QwtPlotCurve("CPU Ram");
    mpCpu1      = new QwtPlotCurve("CPU Swap");
    mpGpu0      = new QwtPlotCurve("GPU");
    mpMic0      = new QwtPlotCurve("MIC");

    mpPlot->setTitle("Memory");
    setWindowTitle("Memory");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Server uptime [s]" );
    mpPlot->setAxisTitle(QwtPlot::yLeft, "Memory");
    mpPlot->insertLegend(mpLegend, QwtPlot::BottomLegend );

    resetPen();

    mpCpu0->attach(mpPlot);
    mpCpu1->attach(mpPlot);
    mpGpu0->attach(mpPlot);
    mpMic0->attach(mpPlot);

    makeZoomable();
    makeGrid();

}


void QMSMMemoryPlot::redraw()
{
    int size = mTimevalues.size();

    mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
    mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
    mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
    mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);

    mpPlot->replot();
}

void QMSMMemoryPlot::updateValues(std::vector<double> &values)
{
    while(mTimevalues.size() > maxData)
    {
        mTimevalues.erase(mTimevalues.begin());
        mCpu0values.erase(mCpu0values.begin());
        mCpu1values.erase(mCpu1values.begin());
        mGpu0values.erase(mGpu0values.begin());
        mMic0values.erase(mMic0values.begin());
    }
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YMemoryCpu]);
    mCpu1values.push_back(values[YSwapCpu]);
    mGpu0values.push_back(values[YMemoryGpu]);
    mMic0values.push_back(values[YMemoryMic]);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);

}

void QMSMMemoryPlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpCpu1->setPen(*mpPaintCpu1);
    mpGpu0->setPen(*mpPaintGpu0);
    mpMic0->setPen(*mpPaintMic0);
}
