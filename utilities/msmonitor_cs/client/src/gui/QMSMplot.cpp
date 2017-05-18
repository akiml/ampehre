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

QMSMplot::QMSMplot(int type, int linewidth, int maxData, int width, int height, QWidget* parent):
    mType(type),
    QWidget(parent),
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
    mpPaintSystem( new QPen(QColor(255,165,0), mLineWidth))
{
    ui->setupUi(this);
    setLineWidth(mLineWidth);
    mpPlot = ui->qwtPlot;
    mpLegend->setFrameStyle(QFrame::Box | QFrame::Sunken);

    resize(width,height);

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(resetLineWidth(int)));
    connect(ui->pushButtonScreenshot, SIGNAL(clicked()), this, SLOT(screenshot()));
    connect(ui->pushButtonCSV, SIGNAL(clicked()), this, SLOT(exportToCSV()));
}

QMSMplot::~QMSMplot()
{
    delete ui;

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
//    mMarker.clear();
//    for(unsigned int i = 0; i < mSymbols.size(); i++)
//    {
//        delete mSymbols[i];
//    }
//    mSymbols.clear();

//    int width = 2;

//    for(unsigned int i = 0; i < mApplications.size(); i++)
//    {
//        QwtPlotMarker marker;
//        marker.setLabel(QwtText(QString::number(mApplications[i].mPid)));
//        mSymbols.push_back(new QwtSymbol(QwtSymbol::VLine, QBrush(QColor("#000000")), QPen( Qt::black, 1 ), QSize( width,0 )));
//        marker.setSymbol(&mSymbols.back());
//        mMarker.push_back(marker);
//        mMarker.back().attach(mpPlot);
//        mMarker.back().show();
//    }
}
