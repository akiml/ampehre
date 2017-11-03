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

    mpCpu0Max   = new QwtPlotCurve("Cpu0 Max");
    mpCpu0Min   = new QwtPlotCurve("Cpu0 Min");
    mpCpu1Max   = new QwtPlotCurve("Cpu1 Max");
    mpCpu1Min   = new QwtPlotCurve("Cpu1 Min");
    mpGpu0Max   = new QwtPlotCurve("Gpu0 Max");
    mpGpu0Min   = new QwtPlotCurve("Gpu0 Min");
    mpGpu1Max   = new QwtPlotCurve("Gpu1 Max");
    mpGpu1Min   = new QwtPlotCurve("Gpu1 Min");
    mpMic0Max   = new QwtPlotCurve("Mic0 Max");
    mpMic0Min   = new QwtPlotCurve("Mic0 Min");
    mpMic1Max   = new QwtPlotCurve("Mic1 Max");
    mpMic1Min   = new QwtPlotCurve("Mic1 Min");

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

    mpCpu0Max->attach(mpPlot);
    mpCpu1Max->attach(mpPlot);
    mpGpu0Max->attach(mpPlot);
    mpGpu1Max->attach(mpPlot);
    mpMic0Max->attach(mpPlot);
    mpMic1Max->attach(mpPlot);

    mpCpu0Min->attach(mpPlot);
    mpCpu1Min->attach(mpPlot);
    mpGpu0Min->attach(mpPlot);
    mpGpu1Min->attach(mpPlot);
    mpMic0Min->attach(mpPlot);
    mpMic1Min->attach(mpPlot);

    makeZoomable();
    makeGrid();

}


void QMSMClockPlot::redraw()
{
    mpCpu0Max->setSamples(mTimevalues.data(), mCpu0valuesMax.data(), mCpu0valuesMax.size());
    mpCpu0Min->setSamples(mTimevalues.data(), mCpu0valuesMin.data(), mCpu0valuesMin.size());
    mpCpu1Max->setSamples(mTimevalues.data(), mCpu1valuesMax.data(), mCpu1valuesMax.size());
    mpCpu1Min->setSamples(mTimevalues.data(), mCpu1valuesMin.data(), mCpu1valuesMin.size());
    mpGpu0Max->setSamples(mTimevalues.data(), mGpu0valuesMax.data(), mGpu0valuesMax.size());
    mpGpu0Min->setSamples(mTimevalues.data(), mGpu0valuesMin.data(), mGpu0valuesMin.size());
    mpGpu1Max->setSamples(mTimevalues.data(), mGpu1valuesMax.data(), mGpu1valuesMax.size());
    mpGpu1Min->setSamples(mTimevalues.data(), mGpu1valuesMin.data(), mGpu1valuesMin.size());
    mpMic0Max->setSamples(mTimevalues.data(), mMic0valuesMax.data(), mMic0valuesMax.size());
    mpMic0Min->setSamples(mTimevalues.data(), mMic0valuesMin.data(), mMic0valuesMin.size());
    mpMic1Max->setSamples(mTimevalues.data(), mMic1valuesMax.data(), mMic1valuesMax.size());
    mpMic1Min->setSamples(mTimevalues.data(), mMic1valuesMin.data(), mMic1valuesMin.size());

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
        int size = mCpu0valuesMean.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMean.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1valuesMean.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMean.data(), size);
        mpGpu1->setSamples(mTimevalues.data(), mGpu1valuesMean.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMean.data(), size);
        mpMic1->setSamples(mTimevalues.data(), mMic1valuesMean.data(), size);
    }
    else if(mValue == MEDIAN)
    {
        int size = mCpu0valuesMedian.size();

        mpCpu0->setSamples(mTimevalues.data(), mCpu0valuesMedian.data(), size);
        mpCpu1->setSamples(mTimevalues.data(), mCpu1valuesMedian.data(), size);
        mpGpu0->setSamples(mTimevalues.data(), mGpu0valuesMedian.data(), size);
        mpGpu1->setSamples(mTimevalues.data(), mGpu1valuesMedian.data(), size);
        mpMic0->setSamples(mTimevalues.data(), mMic0valuesMedian.data(), size);
        mpMic1->setSamples(mTimevalues.data(), mMic1valuesMedian.data(), size);
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
                mpGpu1Max->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0Max->setVisible(true);
            }
            else if(i == 5)
            {
                mpMic1Max->setVisible(true);
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
                mpGpu1Max->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0Max->setVisible(false);
            }
            else if(i == 5)
            {
                mpMic1Max->setVisible(false);
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
                mpGpu1Min->setVisible(true);
            }
            else if(i == 4)
            {
                mpMic0Min->setVisible(true);
            }
            else if(i == 5)
            {
                mpMic1Min->setVisible(true);
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
                mpGpu1Min->setVisible(false);
            }
            else if(i == 4)
            {
                mpMic0Min->setVisible(false);
            }
            else if(i == 5)
            {
                mpMic1Min->setVisible(false);
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

        mCpu0valuesMean.erase(mCpu0valuesMean.begin());
        mCpu1valuesMean.erase(mCpu1valuesMean.begin());
        mGpu0valuesMean.erase(mGpu0valuesMean.begin());
        mGpu1valuesMean.erase(mGpu1valuesMean.begin());
        mMic0valuesMean.erase(mMic0valuesMean.begin());
        mMic1valuesMean.erase(mMic1valuesMean.begin());

        mCpu0valuesMedian.erase(mCpu0valuesMedian.begin());
        mCpu1valuesMedian.erase(mCpu1valuesMedian.begin());
        mGpu0valuesMedian.erase(mGpu0valuesMedian.begin());
        mGpu1valuesMedian.erase(mGpu1valuesMedian.begin());
        mMic0valuesMedian.erase(mMic0valuesMedian.begin());
        mMic1valuesMedian.erase(mMic1valuesMedian.begin());
    }
    mTimevalues.push_back(values[X]);
    mCpu0values.push_back(values[YClockCpu0]);
    mCpu1values.push_back(values[YClockCpu1]);
    mGpu0values.push_back(values[YClockGpuCore]);
    mGpu1values.push_back(values[YClockGpuMem]);
    mMic0values.push_back(values[YClockMicCore]);
    mMic1values.push_back(values[YClockMicMem]);

    computeMean(mCpu0values, mCpu0valuesMean);
    computeMean(mCpu1values, mCpu1valuesMean);
    computeMean(mGpu0values, mGpu0valuesMean);
    computeMean(mGpu1values, mGpu1valuesMean);
    computeMean(mMic0values, mMic0valuesMean);
    computeMean(mMic1values, mMic1valuesMean);

    computeMedian(mCpu0values, mCpu0valuesMedian);
    computeMedian(mCpu1values, mCpu1valuesMedian);
    computeMedian(mGpu0values, mGpu0valuesMedian);
    computeMedian(mGpu1values, mGpu1valuesMedian);
    computeMedian(mMic0values, mMic0valuesMedian);
    computeMedian(mMic1values, mMic1valuesMedian);


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
    i++;
    if(mMic1values.back() > mExVal[i].max)
    {
        mExVal[i].max =  mMic1values.back();
        for(unsigned int i = 0; i < mMic1valuesMax.size(); i++)
        {
            mMic1valuesMax[i] = mMic1values.back();
        }
    }
    else if(mMic1values.back() < mExVal[i].min)
    {
        mExVal[i].min =  mMic1values.back();
        for(unsigned int i = 0; i < mMic1valuesMin.size(); i++)
        {
            mMic1valuesMin[i] = mMic1values.back();
        }
    }
    mMic1valuesMin.push_back(mExVal[i].min);
    mMic1valuesMax.push_back(mExVal[i].max);

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

    mpCpu0Max->setPen(*mpPaintMax);
    mpCpu1Max->setPen(*mpPaintMax);
    mpGpu0Max->setPen(*mpPaintMax);
    mpGpu1Max->setPen(*mpPaintMax);
    mpMic0Max->setPen(*mpPaintMax);
    mpMic1Max->setPen(*mpPaintMax);

    mpCpu0Min->setPen(*mpPaintMin);
    mpCpu1Min->setPen(*mpPaintMin);
    mpGpu0Min->setPen(*mpPaintMin);
    mpGpu1Min->setPen(*mpPaintMin);
    mpMic0Min->setPen(*mpPaintMin);
    mpMic1Min->setPen(*mpPaintMin);
}
