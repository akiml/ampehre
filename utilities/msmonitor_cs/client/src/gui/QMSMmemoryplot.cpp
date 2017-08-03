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
    std::cout << "delete memPlot" << std::endl;
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

    mpCpu0Max   = new QwtPlotCurve("Cpu0 Max");
    mpCpu0Min   = new QwtPlotCurve("Cpu0 Min");
    mpCpu1Max   = new QwtPlotCurve("Cpu1 Max");
    mpCpu1Min   = new QwtPlotCurve("Cpu1 Min");
    mpGpu0Max   = new QwtPlotCurve("Gpu0 Max");
    mpGpu0Min   = new QwtPlotCurve("Gpu0 Min");
    mpMic0Max   = new QwtPlotCurve("Mic0 Max");
    mpMic0Min   = new QwtPlotCurve("Mic0 Min");

    mBoxes.push_back(new QCheckBox("CPU Ram"));
    mBoxes.push_back(new QCheckBox("CPU Swap"));
    mBoxes.push_back(new QCheckBox("GPU"));
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

    mpCpu0Max->attach(mpPlot);
    mpCpu1Max->attach(mpPlot);
    mpGpu0Max->attach(mpPlot);
    mpMic0Max->attach(mpPlot);

    mpCpu0Min->attach(mpPlot);
    mpCpu1Min->attach(mpPlot);
    mpGpu0Min->attach(mpPlot);
    mpMic0Min->attach(mpPlot);

    makeZoomable();
    makeGrid();

}


void QMSMMemoryPlot::redraw()
{
    mpCpu0Max->setSamples(mTimevalues.data(), mCpu0valuesMax.data(), mCpu0valuesMax.size());
    mpCpu0Min->setSamples(mTimevalues.data(), mCpu0valuesMin.data(), mCpu0valuesMin.size());
    mpCpu1Max->setSamples(mTimevalues.data(), mCpu1valuesMax.data(), mCpu1valuesMax.size());
    mpCpu1Min->setSamples(mTimevalues.data(), mCpu1valuesMin.data(), mCpu1valuesMin.size());
    mpGpu0Max->setSamples(mTimevalues.data(), mGpu0valuesMax.data(), mGpu0valuesMax.size());
    mpGpu0Min->setSamples(mTimevalues.data(), mGpu0valuesMin.data(), mGpu0valuesMin.size());
    mpMic0Max->setSamples(mTimevalues.data(), mMic0valuesMax.data(), mMic0valuesMax.size());
    mpMic0Min->setSamples(mTimevalues.data(), mMic0valuesMin.data(), mMic0valuesMin.size());
    if(mValue == ABSOLUTE)
    {
        int size = mTimevalues.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0values.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1values.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0values.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0values.data(), size);
    }
    else if(mValue == MEAN)
    {
        int size = mCpu0valuesMean.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMean.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1valuesMean.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMean.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMean.data(), size);
    }
    else if(mValue == MEDIAN)
    {
        int size = mCpu0valuesMedian.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMedian.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1valuesMedian.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMedian.data(), size);
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
                mpCpu1->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu0->setVisible(true);
            }
            else if(i == 3)
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
                mpCpu1->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu0->setVisible(false);
            }
            else if(i == 3)
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
                mpCpu1Max->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu0Max->setVisible(true);
            }
            else if(i == 3)
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
                mpCpu1Max->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu0Max->setVisible(false);
            }
            else if(i == 3)
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
                mpCpu1Min->setVisible(true);
            }
            else if(i == 2)
            {
                mpGpu0Min->setVisible(true);
            }
            else if(i == 3)
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
                mpCpu1Min->setVisible(false);
            }
            else if(i == 2)
            {
                mpGpu0Min->setVisible(false);
            }
            else if(i == 3)
            {
                mpMic0Min->setVisible(false);
            }
        }
    }

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

        mCpu0valuesMean.erase(mCpu0valuesMean.begin());
        mCpu1valuesMean.erase(mCpu1valuesMean.begin());
        mGpu0valuesMean.erase(mGpu0valuesMean.begin());
        mMic0valuesMean.erase(mMic0valuesMean.begin());

        mCpu0valuesMedian.erase(mCpu0valuesMedian.begin());
        mCpu1valuesMedian.erase(mCpu1valuesMedian.begin());
        mGpu0valuesMedian.erase(mGpu0valuesMedian.begin());
        mMic0valuesMedian.erase(mMic0valuesMedian.begin());
    }
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YMemoryCpu]);
    mCpu1values.push_back(values[YSwapCpu]);
    mGpu0values.push_back(values[YMemoryGpu]);
    mMic0values.push_back(values[YMemoryMic]);

    computeMean(mCpu0values, mCpu0valuesMean);
    computeMean(mCpu1values, mCpu1valuesMean);
    computeMean(mGpu0values, mGpu0valuesMean);
    computeMean(mMic0values, mMic0valuesMean);

    computeMedian(mCpu0values, mCpu0valuesMedian);
    computeMedian(mCpu1values, mCpu1valuesMedian);
    computeMedian(mGpu0values, mGpu0valuesMedian);
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
    if(mCpu1values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mCpu1values.back();
        for(unsigned int i = 0; i < mCpu1valuesMax.size(); i++)
        {
            mCpu1valuesMax[i] = mCpu1values.back();
        }
    }
    else if(mCpu1values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mCpu1values.back();
        for(unsigned int i = 0; i < mCpu1valuesMin.size(); i++)
        {
            mCpu1valuesMin[i] = mCpu1values.back();
        }
    }
    mCpu1valuesMin.push_back(mExVal[i].min);
    mCpu1valuesMax.push_back(mExVal[i].max);
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

void QMSMMemoryPlot::resetPen()
{
    mpCpu0->setPen(*mpPaintCpu0);
    mpCpu1->setPen(*mpPaintCpu1);
    mpGpu0->setPen(*mpPaintGpu0);
    mpMic0->setPen(*mpPaintMic0);

    mpCpu0Max->setPen(*mpPaintMax);
    mpCpu1Max->setPen(*mpPaintMax);
    mpGpu0Max->setPen(*mpPaintMax);
    mpMic0Max->setPen(*mpPaintMax);

    mpCpu0Min->setPen(*mpPaintMin);
    mpCpu1Min->setPen(*mpPaintMin);
    mpGpu0Min->setPen(*mpPaintMin);
    mpMic0Min->setPen(*mpPaintMin);
}
