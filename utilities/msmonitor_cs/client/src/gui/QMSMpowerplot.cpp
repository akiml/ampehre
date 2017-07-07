/*
 * QMSMpowerplot.cpp
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

#include "gui/QMSMpowerplot.h"
#include <QCheckBox>
#include <QVBoxLayout>


QMSMPowerPlot::QMSMPowerPlot(int linewidth, int maxData, int width, int height, QWidget *parent):
    QMSMplot(POWER, linewidth, maxData, width, height, parent)
{
    //nothing to be done
}

QMSMPowerPlot::~QMSMPowerPlot()
{
    delete mpCpu0;
    delete mpCpu1;
    delete mpFpga0;
    delete mpMic0;
    delete mpSystem;
    std::cout << "delete powerPlot" << std::endl;
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

    mBoxes.push_back(new QCheckBox("CPU0"));
    mBoxes.push_back(new QCheckBox("CPU1"));
    mBoxes.push_back(new QCheckBox("GPU"));
    mBoxes.push_back(new QCheckBox("FPGA"));
    mBoxes.push_back(new QCheckBox("MIC"));
    mBoxes.push_back(new QCheckBox("System"));
    QVBoxLayout* layout = new QVBoxLayout;

    for(unsigned int i = 0; i < mBoxes.size(); i++)
    {
        mBoxes[i]->setChecked(true);
        layout->addWidget(mBoxes[i]);
    }

    mGroupbox->setLayout(layout);

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
    if(mValue == ABSOLUTE)
    {
        int size = mTimevalues.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
        mpFpga0->setSamples(mTimevalues.data(), mFpga0values.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
        mpSystem->setSamples(mTimevalues.data(), mSystemvalues.data(), size);
    }
    else if(mValue == MEAN)
    {
        int size = mCpu0valuesMean.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMean.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1valuesMean.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMean.data(), size);
        mpFpga0->setSamples(mTimevalues.data(), mFpga0valuesMean.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMean.data(), size);
        mpSystem->setSamples(mTimevalues.data(), mSystemvaluesMean.data(), size);
    }
    else if(mValue == MEDIAN)
    {
        int size = mCpu0valuesMedian.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMedian.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1valuesMedian.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMedian.data(), size);
        mpFpga0->setSamples(mTimevalues.data(), mFpga0valuesMedian.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMedian.data(), size);
        mpSystem->setSamples(mTimevalues.data(), mSystemvaluesMedian.data(), size);
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
                mpFpga0->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0->setVisible(true);
            }
            else if(i == 5)
            {
                mpSystem->setVisible(true);
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
                mpFpga0->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0->setVisible(false);
            }
            else if(i == 5)
            {
                mpSystem->setVisible(false);
            }
        }
    }

    mpPlot->replot();
}

void QMSMPowerPlot::updateValues(std::vector<double> &values)
{

    while(mTimevalues.size() > maxData)
    {
        mTimevalues.erase(mTimevalues.begin());
        mCpu0values.erase(mCpu0values.begin());
        mCpu1values.erase(mCpu1values.begin());
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

    computeMean(mCpu0values, mCpu0valuesMean);
    computeMean(mCpu1values, mCpu1valuesMean);
    computeMean(mGpu0values, mGpu0valuesMean);
    computeMean(mFpga0values, mFpga0valuesMean);
    computeMean(mMic0values, mMic0valuesMean);
    computeMean(mSystemvalues, mSystemvaluesMean);

    computeMedian(mCpu0values, mCpu0valuesMedian);
    computeMedian(mCpu1values, mCpu1valuesMedian);
    computeMedian(mGpu0values, mGpu0valuesMedian);
    computeMedian(mFpga0values, mFpga0valuesMedian);
    computeMedian(mMic0values, mMic0valuesMedian);
    computeMedian(mSystemvalues, mSystemvaluesMedian);

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
