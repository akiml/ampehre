/*
 * QMSMtemperatureplot.cpp
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


#include "gui/QMSMtemperatureplot.h"

QMSMTemperaturePlot::QMSMTemperaturePlot(int linewidth, int maxData, int width, int height, QWidget *parent):
    QMSMplot(TEMP, linewidth, maxData, width, height, parent)
{
    //nothing to be done
}

QMSMTemperaturePlot::~QMSMTemperaturePlot()
{
    std::cout << "delete tempPlot" << std::endl;
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
    setWindowTitle("Temperature");
    mpPlot->setAxisTitle( QwtPlot::xBottom, "Server uptime [s]" );
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

    makeZoomable();
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
    while(mTimevalues.size() > maxData)
    {
        mTimevalues.erase(mTimevalues.begin());
        mCpu0values.erase(mCpu0values.begin());
        mCpu1values.erase(mCpu1values.begin());
        mGpu0values.erase(mGpu0values.begin());
        mFpga0values.erase(mFpga0values.begin());
        mFpga1values.erase(mFpga1values.begin());
        mMic0values.erase(mMic0values.begin());
        mSystemvalues.erase(mSystemvalues.begin());
    }
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

