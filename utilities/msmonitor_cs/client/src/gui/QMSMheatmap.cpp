/*
 * QMSMheatmap.cpp
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


#include "gui/QMSMheatmap.h"



QMSMHeatmap::QMSMHeatmap( QWidget *parent, int alpha, int intEnd, int intStart, int update, QString caption, int zStart, int zEnd):
    QwtPlot( parent ),
    mpSpectrogram( new QwtPlotSpectrogram),
    mpMatrix ( new QwtMatrixRasterData),
    mAlpha(alpha),
    mIntervalEnd(intEnd),
    mIntervalStart(intStart),
    mUpdate(update)
{
    mY.push_back(0);
    mpMatrix->setValueMatrix(mY, mY.size());
    mpMatrix->setInterval( Qt::XAxis, QwtInterval( mIntervalStart, mIntervalEnd ) );
    mpMatrix->setInterval( Qt::YAxis, QwtInterval( 0, 1 ) );
    mpMatrix->setInterval( Qt::ZAxis, QwtInterval( zStart, zEnd ) );

    mpSpectrogram->setData( mpMatrix );
    mpSpectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    mpSpectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    QList<double> contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;
    mpSpectrogram->setContourLevels( contourLevels );
    mpSpectrogram->attach( this );

    const QwtInterval zInterval = mpSpectrogram->data()->interval( Qt::ZAxis );
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    QwtText txt (caption);
    rightAxis->setTitle( txt );
    rightAxis->setColorBarEnabled( true );

    enableAxis( QwtPlot::yRight );
    enableAxis( QwtPlot::yLeft, false);

    plotLayout()->setAlignCanvasToScales( true );

    setColorMap();
}

QMSMHeatmap::~QMSMHeatmap()
{
    delete mpSpectrogram;
    std::cout << "delete heatmap" << std::endl;
}

void QMSMHeatmap::updateValues(std::vector<double> &values, int val)
{
    this->mTime.push_back(values[X]);
    switch(val)
    {
    case UtilCpu:
        this->mY.push_back(values[YUtilCpu]);
        break;
    case UtilGpuCore:
        this->mY.push_back(values[YUtilGpuCore]);
        break;
    case UtilGpuMem:
        this->mY.push_back(values[YUtilGpuMem]);
        break;
    case UtilFpga:
        this->mY.push_back(values[YUtilFpga]);
        break;
    case UtilMic:
        this->mY.push_back(values[YUtilMic]);
        break;
    case TempCpu0:
        this->mY.push_back(values[YTempCpu0]);
        break;
    case TempCpu1:
        this->mY.push_back(values[YTempCpu1]);
        break;
    case TempGpu:
        this->mY.push_back(values[YTempGpu]);
        break;
    case TempFpgaCompute:
        this->mY.push_back(values[YTempFpgaM]);
        break;
    case TempFpgaInterface:
        this->mY.push_back(values[YTempFpgaI]);
        break;
    case TempMicDie:
        this->mY.push_back(values[YTempMicDie]);
        break;
    case TempMainBoard:
        this->mY.push_back(values[YTempSystem]);
        break;
    }
}

void QMSMHeatmap::redraw()
{
    mpMatrix->setValueMatrix(mY, mY.size());
    mpSpectrogram->setData( mpMatrix );

    int32_t first_tick = 0, second_tick = 0;
    double time_show_data	= 60;
    double xValue = mTime.size();

    first_tick		= ((int)(xValue-time_show_data)/10)*10;
    first_tick		= (first_tick >= 0 && (xValue-time_show_data) > 0) ? first_tick+10 : first_tick;
    second_tick		= ((int)(xValue)/10)*10+10;

    setAxisScale(QwtPlot::xBottom, first_tick, second_tick);
    mIntervalEnd = second_tick;
    mpMatrix->setInterval( Qt::XAxis, QwtInterval( mIntervalStart, mIntervalEnd ) );
    mUpdate++;

    if(mUpdate >= 10)
    {
        replot();
        mUpdate = 0;
    }
}

void QMSMHeatmap::setColorMap()
{
    QwtScaleWidget *axis = axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = mpSpectrogram->data()->interval( Qt::ZAxis );

    int alpha = mAlpha;

    mpSpectrogram->setColorMap( new LinearColorMapRGB );
    axis->setColorMap( zInterval, new LinearColorMapRGB );


    mpSpectrogram->setAlpha( alpha );

    replot();

}

void QMSMHeatmap::setAlpha( int alpha )
{
    mAlpha = alpha;

    mpSpectrogram->setAlpha( alpha );
    replot();

}


