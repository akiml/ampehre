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

    mpCpu0Max   = new QwtPlotCurve("Cpu0 Max");
    mpCpu0Min   = new QwtPlotCurve("Cpu0 Min");
    mpGpu0Max   = new QwtPlotCurve("Gpu0 Max");
    mpGpu0Min   = new QwtPlotCurve("Gpu0 Min");
    mpGpu1Max   = new QwtPlotCurve("Gpu1 Max");
    mpGpu1Min   = new QwtPlotCurve("Gpu1 Min");
    mpFpga0Max  = new QwtPlotCurve("Fpga0 Max");
    mpFpga0Min  = new QwtPlotCurve("Fpga0 Min");
    mpMic0Max   = new QwtPlotCurve("Mic0 Max");
    mpMic0Min   = new QwtPlotCurve("Mic0 Min");

    mBoxes.push_back(new QCheckBox("CPU"));
    mBoxes.push_back(new QCheckBox("GPU Core"));
    mBoxes.push_back(new QCheckBox("GPU Memory"));
    mBoxes.push_back(new QCheckBox("Compute FPGA"));
    mBoxes.push_back(new QCheckBox("MIC"));

    for(unsigned int i = 0; i < mBoxes.size(); i++)
    {
        mBoxes[i]->setChecked(true);
        mBoxesMin.push_back(new QCheckBox("Min"));
        mBoxesMax.push_back(new QCheckBox("Max"));
        mBoxesMin[i]->setChecked(false);
        mBoxesMax[i]->setChecked(false);

        mLeftVert->addWidget(mBoxes[i]);
        mLeftRightLeftVert->addWidget(mBoxesMin[i]);
        mLeftRightRightVert->addWidget(mBoxesMax[i]);
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

    mpCpu0Max->attach(mpPlot);
    mpGpu0Max->attach(mpPlot);
    mpGpu1Max->attach(mpPlot);
    mpFpga0Max->attach(mpPlot);
    mpMic0Max->attach(mpPlot);

    mpCpu0Min->attach(mpPlot);
    mpGpu0Min->attach(mpPlot);
    mpGpu1Min->attach(mpPlot);
    mpFpga0Min->attach(mpPlot);
    mpMic0Min->attach(mpPlot);

    makeZoomable();
    makeGrid();

}


void QMSMUtilPlot::redraw()
{
    mpCpu0Max->setSamples(mTimevalues.data(), mCpu0valuesMax.data(), mCpu0valuesMax.size());
    mpCpu0Min->setSamples(mTimevalues.data(), mCpu0valuesMin.data(), mCpu0valuesMin.size());
    mpGpu0Max->setSamples(mTimevalues.data(), mGpu0valuesMax.data(), mGpu0valuesMax.size());
    mpGpu0Min->setSamples(mTimevalues.data(), mGpu0valuesMin.data(), mGpu0valuesMin.size());
    mpGpu1Max->setSamples(mTimevalues.data(), mGpu1valuesMax.data(), mGpu1valuesMax.size());
    mpGpu1Min->setSamples(mTimevalues.data(), mGpu1valuesMin.data(), mGpu1valuesMin.size());
    mpFpga0Max->setSamples(mTimevalues.data(), mFpga0valuesMax.data(), mFpga0valuesMax.size());
    mpFpga0Min->setSamples(mTimevalues.data(), mFpga0valuesMin.data(), mFpga0valuesMin.size());
    mpMic0Max->setSamples(mTimevalues.data(), mMic0valuesMax.data(), mMic0valuesMax.size());
    mpMic0Min->setSamples(mTimevalues.data(), mMic0valuesMin.data(), mMic0valuesMin.size());

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
        if(mBoxesMax[i]->isChecked())
        {
            if(i == 0)
            {
                mpCpu0Max->setVisible(true);
            }
            else if(i == 1)
            {
                mpGpu0Max->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu1Max->setVisible(true);
            }
            else if(i == 3)
            {
                mpFpga0Max->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0Max->setVisible(true);
            }
        }
        else
        {
            if(i == 0)
            {
                mpCpu0Max->setVisible(false);
            }
            else if(i == 1)
            {
                mpGpu0Max->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu1Max->setVisible(false);
            }
            else if(i == 3)
            {
                mpFpga0Max->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0Max->setVisible(false);
            }
        }

        if(mBoxesMin[i]->isChecked())
        {
            if(i == 0)
            {
                mpCpu0Min->setVisible(true);
            }
            else if(i == 1)
            {
                mpGpu0Min->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu1Min->setVisible(true);
            }
            else if(i == 3)
            {
                mpFpga0Min->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0Min->setVisible(true);
            }
        }
        else
        {
            if(i == 0)
            {
                mpCpu0Min->setVisible(false);
            }
            else if(i == 1)
            {
                mpGpu0Min->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu1Min->setVisible(false);
            }
            else if(i == 3)
            {
                mpFpga0Min->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0Min->setVisible(false);
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
        for(unsigned int i = 0; i < mCpu0valuesMax.size(); i++)
        {
            mCpu0valuesMax[i] = mCpu0values.back();
        }
    }
    else if(mCpu0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mCpu0values.back();
        for(unsigned int i = 0; i < mCpu0valuesMin.size(); i++)
        {
            mCpu0valuesMin[i] = mCpu0values.back();
        }
    }
    mCpu0valuesMin.push_back(mExVal[i].min);
    mCpu0valuesMax.push_back(mExVal[i].max);
    i++;
    if(mGpu0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mGpu0values.back();
        for(unsigned int i = 0; i < mGpu0valuesMax.size(); i++)
        {
            mGpu0valuesMax[i] = mGpu0values.back();
        }
    }
    else if(mGpu0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mGpu0values.back();
        for(unsigned int i = 0; i < mGpu0valuesMin.size(); i++)
        {
            mGpu0valuesMin[i] = mGpu0values.back();
        }
    }
    mGpu0valuesMin.push_back(mExVal[i].min);
    mGpu0valuesMax.push_back(mExVal[i].max);
    i++;
    if(mGpu1values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mGpu1values.back();
        for(unsigned int i = 0; i < mGpu1valuesMax.size(); i++)
        {
            mGpu1valuesMax[i] = mGpu1values.back();
        }
    }
    else if(mGpu1values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mGpu1values.back();
        for(unsigned int i = 0; i < mGpu1valuesMin.size(); i++)
        {
            mGpu1valuesMin[i] = mGpu1values.back();
        }
    }
    mGpu1valuesMin.push_back(mExVal[i].min);
    mGpu1valuesMax.push_back(mExVal[i].max);
    i++;
    if(mFpga0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mFpga0values.back();
        for(unsigned int i = 0; i < mFpga0valuesMax.size(); i++)
        {
            mFpga0valuesMax[i] = mFpga0values.back();
        }
    }
    else if(mFpga0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mFpga0values.back();
        for(unsigned int i = 0; i < mFpga0valuesMin.size(); i++)
        {
            mFpga0valuesMin[i] = mFpga0values.back();
        }
    }
    mFpga0valuesMin.push_back(mExVal[i].min);
    mFpga0valuesMax.push_back(mExVal[i].max);
    i++;
    if(mMic0values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mMic0values.back();
        for(unsigned int i = 0; i < mMic0valuesMax.size(); i++)
        {
            mMic0valuesMax[i] = mMic0values.back();
        }
    }
    else if(mMic0values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mMic0values.back();
        for(unsigned int i = 0; i < mMic0valuesMin.size(); i++)
        {
            mMic0valuesMin[i] = mMic0values.back();
        }
    }
    mMic0valuesMin.push_back(mExVal[i].min);
    mMic0valuesMax.push_back(mExVal[i].max);

    scaleAxis(mTimevalues[mTimevalues.size()-1]);

}

void QMSMUtilPlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpGpu0->setPen(*mpPaintGpu0);
    mpGpu1->setPen(*mpPaintGpu1);
    mpFpga0->setPen(*mpPaintFpga0);
    mpMic0->setPen(*mpPaintMic0);

    mpCpu0Max->setPen(*mpPaintMax);
    mpGpu0Max->setPen(*mpPaintMax);
    mpGpu1Max->setPen(*mpPaintMax);
    mpFpga0Max->setPen(*mpPaintMax);
    mpMic0Max->setPen(*mpPaintMax);

    mpCpu0Min->setPen(*mpPaintMin);
    mpGpu0Min->setPen(*mpPaintMin);
    mpGpu1Min->setPen(*mpPaintMin);
    mpFpga0Min->setPen(*mpPaintMin);
    mpMic0Min->setPen(*mpPaintMin);
}
