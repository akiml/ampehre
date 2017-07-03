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
    std::cout << "delete clcokPlot" << std::endl;
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

    mBoxes.push_back(new QCheckBox("CPU0"));
    mBoxes.push_back(new QCheckBox("CPU1"));
    mBoxes.push_back(new QCheckBox("GPU Core"));
    mBoxes.push_back(new QCheckBox("GPU Mem"));
    mBoxes.push_back(new QCheckBox("MIC Core"));
    mBoxes.push_back(new QCheckBox("MIC Mem"));
    QVBoxLayout* layout = new QVBoxLayout;

    for(unsigned int i = 0; i < mBoxes.size(); i++)
    {
        mBoxes[i]->setChecked(true);
        layout->addWidget(mBoxes[i]);
    }

    mGroupbox->setLayout(layout);

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
    if(mValue == ABSOLUTE)
    {
        int size = mTimevalues.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
        mpGpu1->setSamples(mTimevalues.data(), mGpu1values.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
        mpMic1->setSamples(mTimevalues.data(), mMic1values.data(), size);
    }
    else if(mValue == MEAN)
    {
        int size = mTimevaluesMean.size();

        mpCpu0->setSamples(mTimevaluesMean.data(), mCpu0valuesMean.data(), size);
        mpCpu1->setSamples(mTimevaluesMean.data(), mCpu1valuesMean.data(), size);
        mpGpu0->setSamples(mTimevaluesMean.data(), mGpu0valuesMean.data(), size);
        mpGpu1->setSamples(mTimevaluesMean.data(), mGpu1valuesMean.data(), size);
        mpMic0->setSamples(mTimevaluesMean.data(), mMic0valuesMean.data(), size);
        mpMic1->setSamples(mTimevaluesMean.data(), mMic1valuesMean.data(), size);
    }
    else if(mValue == MEDIAN)
    {
        int size = mTimevaluesMedian.size();

        mpCpu0->setSamples(mTimevaluesMedian.data(), mCpu0valuesMedian.data(), size);
        mpCpu1->setSamples(mTimevaluesMedian.data(), mCpu1valuesMedian.data(), size);
        mpGpu0->setSamples(mTimevaluesMedian.data(), mGpu0valuesMedian.data(), size);
        mpGpu1->setSamples(mTimevaluesMedian.data(), mGpu1valuesMedian.data(), size);
        mpMic0->setSamples(mTimevaluesMedian.data(), mMic0valuesMedian.data(), size);
        mpMic1->setSamples(mTimevaluesMedian.data(), mMic1valuesMedian.data(), size);
    }
    redrawApplications();

    for(unsigned int i = 0; i < mBoxes.size(); i++)
    {
        if(mBoxes[i]->isChecked())
        {
            if(i == 0)
            {
                mpCpu0->setVisible(true);
            }
            else if(i == 1)
            {
                mpCpu1->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu0->setVisible(true);
            }
            else if(i == 3)
            {
                mpGpu1->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0->setVisible(true);
            }
            else if(i == 5)
            {
                mpMic1->setVisible(true);
            }
        }
        else
        {
            if(i == 0)
            {
                mpCpu0->setVisible(false);
            }
            else if(i == 1)
            {
                mpCpu1->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu0->setVisible(false);
            }
            else if(i == 3)
            {
                mpGpu1->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0->setVisible(false);
            }
            else if(i == 5)
            {
                mpMic1->setVisible(false);
            }
        }
    }

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

    computeMean(mTimevalues, mTimevaluesMean);
    computeMean(mCpu0values, mCpu0valuesMean);
    computeMean(mCpu1values, mCpu1valuesMean);
    computeMean(mGpu0values, mGpu0valuesMean);
    computeMean(mGpu1values, mGpu1valuesMean);
    computeMean(mMic0values, mMic0valuesMean);
    computeMean(mMic1values, mMic1valuesMean);

    computeMedian(mTimevalues, mTimevaluesMedian);
    computeMedian(mCpu0values, mCpu0valuesMedian);
    computeMedian(mCpu1values, mCpu1valuesMedian);
    computeMedian(mGpu0values, mGpu0valuesMedian);
    computeMedian(mGpu1values, mGpu1valuesMedian);
    computeMedian(mMic0values, mMic0valuesMedian);
    computeMedian(mMic1values, mMic1valuesMedian);

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
