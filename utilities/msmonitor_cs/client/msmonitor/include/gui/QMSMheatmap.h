/*
 * QMSMheatmap.h
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



#ifndef QMSMHEATMAP_H
#define QMSMHEATMAP_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_rescaler.h>
#include <vector>
#include <QVector>
#include "CProtocol.hpp"

enum YValue
{
    UtilCpu,
    UtilGpuCore,
    UtilGpuMem,
    UtilFpga,
    UtilMic,
    TempCpu0,
    TempCpu1,
    TempGpu,
    TempFpgaCompute,
    TempFpgaInterface,
    TempMicDie,
    TempMainBoard
};

class QMSMHeatmap: public QwtPlot
{
    Q_OBJECT

public:
    enum ColorMap
    {
        RGBMap,
        IndexMap,
        HueMap,
        AlphaMap
    };
    QMSMHeatmap(QWidget * , int alpha, int intEnd, int intStart, int update, QString caption, int zStart, int zEnd);

public Q_SLOTS:
    void setColorMap( int );
    void setAlpha( int );
    void updateValues(std::vector<double>& values, int val);
    void redraw();

private:
    QwtPlotSpectrogram* mpSpectrogram;
    QwtMatrixRasterData* mpMatrix;

    int mMapType;
    int mAlpha;
    int mIntervalEnd;
    int mIntervalStart;
    QVector<double> mTime;
    QVector<double> mY;
    int mUpdate;
};


#endif // QMSMHEATMAP_H
