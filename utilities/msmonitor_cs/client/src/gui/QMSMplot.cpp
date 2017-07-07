/*
 * QMSMplot.cpp
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


#include "gui/QMSMplot.h"
#include "ui_qmsmplot.h"

//seg fault
//socket closes after some time -> CComC.cpp while anstatt if
//draw start stop better

QMSMplot::QMSMplot(int type, int linewidth, int maxData, int width, int height, QWidget* parent):
    QWidget(parent),
    mType(type),
    ui(new Ui::QMSMplot),
    mLineWidth(linewidth),
    maxData(maxData),
    parent(parent),
    mpLegend(new QwtLegend()),
    mpPaintCpu0(new QPen(Qt::darkBlue, mLineWidth)),
    mpPaintCpu1(new QPen(Qt::blue, mLineWidth)),
    mpPaintGpu0(new QPen(Qt::darkRed, mLineWidth)),
    mpPaintGpu1(new QPen(Qt::red, mLineWidth)),
    mpPaintFpga0(new QPen(Qt::darkGreen, mLineWidth)),
    mpPaintFpga1(new QPen(Qt::green, mLineWidth)),
    mpPaintMic0(new QPen(Qt::darkMagenta, mLineWidth)),
    mpPaintMic1(new QPen(Qt::magenta, mLineWidth)),
    mpPaintSystem( new QPen(QColor(255,165,0), mLineWidth)),
    mVerticalLineStart(QwtSymbol::VLine, QBrush(Qt::green), QPen(Qt::green), QSize(1, 300)), //change color
    mVerticalLineEnd(QwtSymbol::VLine, QBrush(Qt::red), QPen(Qt::red), QSize(1, 300))
{
    mMedianInterval = 2;
    mMeanInterval = 2;
    mValue = ABSOLUTE;
    mRefreshRateMult = 1.0;
    mCurrentAppSize = 0;
    enableApplications = true;

    ui->setupUi(this);
    mGroupbox = ui->groupBox;
    setLineWidth(mLineWidth);
    mpPlot = ui->qwtPlot;
    mpLegend->setFrameStyle(QFrame::Box | QFrame::Sunken);

    resize(width,height);

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(resetLineWidth(int)));
    connect(ui->pushButtonScreenshot, SIGNAL(clicked()), this, SLOT(screenshot()));
    connect(ui->pushButtonCSV, SIGNAL(clicked()), this, SLOT(exportToCSV()));
    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(applicationsEnabled(bool)));
    connect(ui->radioButton_absolute, SIGNAL(clicked()), this, SLOT(radioButtonChecked_Abs()));
    connect(ui->radioButton_mean, SIGNAL(clicked()), this, SLOT(radioButtonChecked_Mean()));
    connect(ui->radioButton_median, SIGNAL(clicked()), this, SLOT(radioButtonChecked_Median()));
    connect(ui->spinBox_mean, SIGNAL(valueChanged(int)), this, SLOT(spinBoxIntervalMean(int)));
    connect(ui->spinBox_median, SIGNAL(valueChanged(int)), this, SLOT(spinBoxIntervalMedian(int)));

    radioButtonChecked_Abs();
}

QMSMplot::~QMSMplot()
{
    delete ui;

    mMarker.clear();

    delete mpLegend;
    delete mpPaintCpu0;
    delete mpPaintCpu1;
    delete mpPaintGpu0;
    delete mpPaintGpu1;
    delete mpPaintFpga0;
    delete mpPaintFpga1;
    delete mpPaintMic0;
    delete mpPaintMic1;
    delete mpPaintSystem;

    delete mpMagnifier;
    delete mpPanner;
    delete mpGrid;
}

void QMSMplot::radioButtonChecked_Abs()
{
    ui->radioButton_absolute->setChecked(true);
    ui->radioButton_mean->setChecked(false);
    ui->radioButton_median->setChecked(false);
    mValue = ABSOLUTE;
}

void QMSMplot::radioButtonChecked_Mean()
{
    ui->radioButton_mean->setChecked(true);
    ui->radioButton_absolute->setChecked(false);
    ui->radioButton_median->setChecked(false);
    mValue = MEAN;
}

void QMSMplot::radioButtonChecked_Median()
{
    ui->radioButton_median->setChecked(true);
    ui->radioButton_absolute->setChecked(false);
    ui->radioButton_mean->setChecked(false);
    mValue = MEDIAN;
}

void QMSMplot::spinBoxIntervalMean(int val)
{
    mMeanInterval = val;
}

void QMSMplot::spinBoxIntervalMedian(int val)
{
    mMedianInterval = val;
}

void QMSMplot::applicationsEnabled(bool val)
{
    enableApplications = val;
}

double QMSMplot::getCurrentCpu0()
{
    if(mCpu0values.size())
        return mCpu0values.back();
    else
        return 0;
}

double QMSMplot::getCurrentCpu1()
{
    if(mCpu1values.size())
        return mCpu1values.back();
    else
        return 0;
}

double QMSMplot::getCurrentTime()
{
    if(mTimevalues.size())
        return mTimevalues.back();
    else
        return 0;
}

double QMSMplot::getCurrentGpu0()
{
    if(mGpu0values.size())
        return mGpu0values.back();
    else
        return 0;
}

double QMSMplot::getCurrentGpu1()
{
    if(mGpu1values.size())
        return mGpu1values.back();
    else
        return 0;
}

double QMSMplot::getCurrentFpga0()
{
    if(mFpga0values.size())
        return mFpga0values.back();
    else
        return 0;
}

double QMSMplot::getCurrentFpga1()
{
    if(mFpga1values.size())
        return mFpga1values.back();
    else
        return 0;
}

double QMSMplot::getCurrentMic0()
{
    if(mMic0values.size())
        return mMic0values.back();
    else
        return 0;
}

double QMSMplot::getCurrentMic1()
{
    if(mMic1values.size())
        return mMic1values.back();
    else
        return 0;
}

double QMSMplot::getCurrentSystem()
{
    if(mSystemvalues.size())
        return mSystemvalues.back();
    else
        return 0;
}

void QMSMplot::clearAllData()
{
    mTimevalues.clear();
    mCpu0values.clear();
    mCpu1values.clear();
    mGpu0values.clear();
    mGpu1values.clear();
    mFpga0values.clear();
    mFpga1values.clear();
    mMic0values.clear();
    mMic1values.clear();
    mSystemvalues.clear();

    mCpu0valuesMean.clear();
    mCpu1valuesMean.clear();
    mGpu0valuesMean.clear();
    mGpu1valuesMean.clear();
    mFpga0valuesMean.clear();
    mFpga1valuesMean.clear();
    mMic0valuesMean.clear();
    mMic1valuesMean.clear();
    mSystemvaluesMean.clear();

    mCpu0valuesMedian.clear();
    mCpu1valuesMedian.clear();
    mGpu0valuesMedian.clear();
    mGpu1valuesMedian.clear();
    mFpga0valuesMedian.clear();
    mFpga1valuesMedian.clear();
    mMic0valuesMedian.clear();
    mMic1valuesMedian.clear();
    mSystemvaluesMedian.clear();
}


void QMSMplot::setLineWidth(int val)
{
    ui->spinBox->setValue(val);
}

void QMSMplot::exportToCSV()
{
    emit signal_export(this);
}

QWidget* QMSMplot::getPlot()
{
    return this->mpPlot;
}

void QMSMplot::computeMean(std::vector<double>& src, std::vector<double> &dst)
{
    int numValues = mMeanInterval*mRefreshRateMult;
    if(src.size() > numValues)
    {
        double tmp = 0;

        //add up all values and devide by interval
        for(unsigned int i = 0; i < numValues; i++)
        {
            tmp += src[src.size()-(i+1)];
        }

        tmp = tmp/numValues;

        dst.push_back(tmp);
    }
}

void QMSMplot::computeMedian(std::vector<double> &src, std::vector<double> &dst)
{
    int numValues = mMedianInterval*mRefreshRateMult;
    if(src.size() > numValues)
    {
        std::vector<double> tmp;

        //save last n data
        for(unsigned int i = 0; i < numValues; i++)
        {
            tmp.push_back(src[src.size()-(i+1)]);
        }

        //delete the n/2 smallest ones
        for(unsigned int i = 0; i < numValues/2; i++)
        {
            int pos = 0;
            for(unsigned int n = 0; n < tmp.size(); n++)
            {
                if(tmp[pos] > tmp[n])
                {
                    pos = n;
                }
            }
            tmp.erase(tmp.begin() + pos);
        }

        //find smallest one -> middle value
        int pos = 0;
        for(unsigned int n = 0; n < tmp.size(); n++)
        {
            if(tmp[pos] > tmp[n])
            {
                pos = n;
            }
        }
        dst.push_back(tmp[pos]);
    }
}

void QMSMplot::screenshot()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                "/",
                                tr("Images (*.png *.xpm *.jpg)"));

    QPixmap pixmap (mpPlot->geometry().width(),mpPlot->geometry().height());
    QPainter * painter=new QPainter(&pixmap);
    QwtPlotRenderer rend;
    rend.render(mpPlot,painter,mpPlot->geometry());
    pixmap.save(fileName);
}

QWidget* QMSMplot::getParent()
{
    return this->parent;
}

void QMSMplot::setRefreshRate(float val)
{
    this->mRefreshRateMult = val;
}

void QMSMplot::makeGrid()
{
    mpGrid = new QwtPlotGrid();
    mpGrid->enableXMin(true);
    mpGrid->enableYMin(true);
    mpGrid->setPen(QPen(Qt::black, 0, Qt::DotLine));
    mpGrid->attach(mpPlot);
}

void QMSMplot::makeZoomable()
{
    mpPanner = new QwtPlotPanner(mpPlot->canvas());
    mpPanner->setMouseButton(Qt::LeftButton);
    mpMagnifier = new QwtPlotMagnifier(mpPlot->canvas());
 //   mpPanner>setMouseButton(Qt::LeftButton);
}

void QMSMplot::setMaxData(int v)
{
    this->maxData = v;
}

void QMSMplot::scaleAxis(double xValue)
{
    int32_t first_tick		= 0;
    int32_t second_tick		= 0;
    double time_show_data	= 60;

    first_tick		= ((int)(xValue-time_show_data)/10)*10;
    first_tick		= (first_tick >= 0 && (xValue-time_show_data) > 0) ? first_tick+10 : first_tick;
    second_tick		= ((int)(xValue)/10)*10+10;

    mpPlot->setAxisScale(QwtPlot::xBottom, first_tick, second_tick);
//    mpPlot->setAxisScale(QwtPlot::yLeft, yValueMin, yValueMax);
}

void QMSMplot::resetLineWidth(int lValue)
{
    if( lValue > 0 ){
        mLineWidth = lValue;
        mpPaintCpu0->setWidth(lValue);
        mpPaintCpu1->setWidth(lValue);
        mpPaintGpu0->setWidth(lValue);
        mpPaintGpu1->setWidth(lValue);
        mpPaintFpga0->setWidth(lValue);
        mpPaintFpga1->setWidth(lValue);
        mpPaintMic0->setWidth(lValue);
        mpPaintMic1->setWidth(lValue);
        mpPaintSystem->setWidth(lValue);

        resetPen();
    }

}

void QMSMplot::resetPen()
{
    //nothing to do here
}

void QMSMplot::updateApplications(const std::vector<Application> &apps)
{
    this->mApplications = apps;
}

void QMSMplot::redrawApplications()
{

    for(unsigned int i = 0; i < mMarker.size(); i++)
    {
        mMarker[i]->setVisible(enableApplications);
    }

    if(mCurrentAppSize < mApplications.size())
    {
        for(unsigned int i = mCurrentAppSize; i < mApplications.size(); i++)
        {
            QwtPlotMarker *marker = new QwtPlotMarker();

            if(mApplications[i].start)
                marker->setSymbol(&mVerticalLineStart);
            else
                marker->setSymbol(&mVerticalLineEnd);

            marker->setLabel(QwtText(QString::number(mApplications[i].mPid)));
            marker->setLabelAlignment(Qt::AlignTop);
            marker->setXValue(mApplications[i].mTime);
            marker->setVisible(enableApplications);
            mMarker.push_back(marker);
            mMarker.back()->attach(mpPlot);
            qDebug() << "added application symbol";
        }
        mCurrentAppSize = mApplications.size();
    }
}

