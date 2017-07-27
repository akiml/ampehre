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

    mBoxes.push_back(new QCheckBox("CPU"));
    mBoxes.push_back(new QCheckBox("GPU Core"));
    mBoxes.push_back(new QCheckBox("GPU Memory"));
    mBoxes.push_back(new QCheckBox("Compute FPGA"));
    mBoxes.push_back(new QCheckBox("MIC"));

    for(unsigned int i = 0; i < mBoxes.size(); i++)
    {
        mLabelsMin.push_back(new QLabel("Min: "));
        mLabelsMax.push_back(new QLabel("Max: "));
        mBoxes[i]->setChecked(true);

        mLeftVert->addWidget(mBoxes[i]);
        mRightLeftVert->addWidget(mLabelsMin[i]);
        mRightRightVert->addWidget(mLabelsMax[i]);
    }


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
    if(mValue == ABSOLUTE)
    {
        int size = mTimevalues.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
        mpGpu1->setSamples(mTimevalues.data(), mGpu1values.data(), size);
        mpFpga0->setSamples(mTimevalues.data(), mFpga0values.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
    }
    else if(mValue == MEAN)
    {
        int size = mCpu0valuesMean.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMean.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMean.data(), size);
        mpGpu1->setSamples(mTimevalues.data(), mGpu1valuesMean.data(), size);
        mpFpga0->setSamples(mTimevalues.data(), mFpga0valuesMean.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMean.data(), size);
    }
    else if(mValue == MEDIAN)
    {
        int size = mCpu0valuesMedian.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMedian.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMedian.data(), size);
        mpGpu1->setSamples(mTimevalues.data(), mGpu1valuesMedian.data(), size);
        mpFpga0->setSamples(mTimevalues.data(), mFpga0valuesMedian.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMedian.data(), size);
    }

    redrawApplications();
    redrawMinMax();


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
                mpGpu0->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu1->setVisible(true);
            }
            else if(i == 3)
            {
                mpFpga0->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0->setVisible(true);
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
                mpGpu0->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu1->setVisible(false);
            }
            else if(i == 3)
            {
                mpFpga0->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0->setVisible(false);
            }
        }
    }


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

        mCpu0valuesMean.erase(mCpu0valuesMean.begin());
        mGpu0valuesMean.erase(mGpu0valuesMean.begin());
        mGpu1valuesMean.erase(mGpu1valuesMean.begin());
        mFpga0valuesMean.erase(mFpga0valuesMean.begin());
        mMic0valuesMean.erase(mMic0valuesMean.begin());

        mCpu0valuesMedian.erase(mCpu0valuesMedian.begin());
        mGpu0valuesMedian.erase(mGpu0valuesMedian.begin());
        mGpu1valuesMedian.erase(mGpu1valuesMedian.begin());
        mFpga0valuesMedian.erase(mFpga0valuesMedian.begin());
        mMic0valuesMedian.erase(mMic0valuesMedian.begin());
    }
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YUtilCpu]);
    mGpu0values.push_back(values[YUtilGpuCore]);
    mGpu1values.push_back(values[YUtilGpuMem]);
    mFpga0values.push_back(values[YUtilFpga]);
    mMic0values.push_back(values[YUtilMic]);

    computeMean(mCpu0values, mCpu0valuesMean);
    computeMean(mGpu0values, mGpu0valuesMean);
    computeMean(mGpu1values, mGpu1valuesMean);
    computeMean(mFpga0values, mFpga0valuesMean);
    computeMean(mMic0values, mMic0valuesMean);

    computeMedian(mCpu0values, mCpu0valuesMedian);
    computeMedian(mGpu0values, mGpu0valuesMedian);
    computeMedian(mGpu1values, mGpu1valuesMedian);
    computeMedian(mFpga0values, mFpga0valuesMedian);
    computeMedian(mMic0values, mMic0valuesMedian);

    int i = 0;

    if(mCpu0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mCpu0values.back();
    }
    else if(mCpu0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mCpu0values.back();
    }
    i++;
    if(mGpu0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mGpu0values.back();
    }
    else if(mGpu0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mGpu0values.back();
    }
    i++;
    if(mGpu1values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mGpu1values.back();
    }
    else if(mGpu1values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mGpu1values.back();
    }
    i++;
    if(mFpga0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mFpga0values.back();
    }
    else if(mFpga0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mFpga0values.back();
    }
    i++;
    if(mMic0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mMic0values.back();
    }
    else if(mMic0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mMic0values.back();
    }
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
