/*
 * QMSMplot.h
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



#ifndef QMSMPLOT_H
#define QMSMPLOT_H

#include <QWidget>
#include <QPen>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <vector>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <iostream>


namespace Ui {
class QMSMplot;
}

enum PLOTTYPE
{
    POWER,
    CLOCK,
    MEMORY,
    TEMP,
    UTIL
};

class QMSMplot : public QWidget
{
    Q_OBJECT
public:
    explicit QMSMplot(int type, int linewidth, int maxData, int width, int height, QWidget* parent);
    virtual ~QMSMplot();
    virtual QWidget* getPlot() = 0;
    virtual QWidget* getParent();

    virtual double getCurrentTime();
    virtual double getCurrentCpu0();
    virtual double getCurrentCpu1();
    virtual double getCurrentGpu0();
    virtual double getCurrentGpu1();
    virtual double getCurrentFpga0();
    virtual double getCurrentFpga1();
    virtual double getCurrentMic0();
    virtual double getCurrentMic1();
    virtual double getCurrentSystem();

    std::vector<double> mTimevalues;
    std::vector<double> mCpu0values;
    std::vector<double> mCpu1values;
    std::vector<double> mGpu0values;
    std::vector<double> mGpu1values;
    std::vector<double> mFpga0values;
    std::vector<double> mFpga1values;
    std::vector<double> mMic0values;
    std::vector<double> mMic1values;
    std::vector<double> mSystemvalues;
    int mType;

    virtual void setLineWidth(int val);



public slots:
    virtual void updateValues(std::vector<double>& values) = 0;
    virtual void redraw() = 0;
    virtual void initPlot() = 0;
    virtual void makeGrid();
    virtual void scaleAxis(double xValue);
    virtual void resetLineWidth(int lValue);
    virtual void resetPen();
    virtual void screenshot();
    virtual void makeZoomable();
    virtual void setMaxData(int v);
    virtual void exportToCSV();

signals:
    void signal_export(QMSMplot*);



protected:
    Ui::QMSMplot* ui;
    int mLineWidth;
    unsigned int maxData;

    QwtPlot* mpPlot;
    QWidget* parent;
    QwtLegend* mpLegend;

    QwtPlotCurve* mpCpu0;
    QwtPlotCurve* mpCpu1;
    QwtPlotCurve* mpGpu0;
    QwtPlotCurve* mpGpu1;
    QwtPlotCurve* mpFpga0;
    QwtPlotCurve* mpFpga1;
    QwtPlotCurve* mpMic0;
    QwtPlotCurve* mpMic1;
    QwtPlotCurve* mpSystem;

    QPen* mpPaintCpu0;
    QPen* mpPaintCpu1;
    QPen* mpPaintGpu0;
    QPen* mpPaintGpu1;
    QPen* mpPaintFpga0;
    QPen* mpPaintFpga1;
    QPen* mpPaintMic0;
    QPen* mpPaintMic1;
    QPen* mpPaintSystem;

    QwtPlotMagnifier* mpMagnifier;
    QwtPlotPanner* mpPanner;
    QwtPlotGrid* mpGrid;


};

#endif // QMSMPLOT_H