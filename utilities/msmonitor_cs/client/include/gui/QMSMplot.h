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
#include <qwt_symbol.h>
#include <vector>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <qwt_plot_marker.h>
#include <iostream>
#include "utils.h"
#include <QCheckBox>
#include <QGroupBox>


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

enum FILTER
{
    ABSOLUTE,
    MEAN,
    MEDIAN,
    FILTER_SIZE
};

enum CHECKBOX
{
    CPU0,
    CPU1,
    GPU0,
    GPU1,
    FPGA0,
    FPGA1,
    MIC0,
    MIC1,
    SYSTEM,
    CHECKBOX_SIZE
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

    std::vector<double> mTimevaluesMean;
    std::vector<double> mCpu0valuesMean;
    std::vector<double> mCpu1valuesMean;
    std::vector<double> mGpu0valuesMean;
    std::vector<double> mGpu1valuesMean;
    std::vector<double> mFpga0valuesMean;
    std::vector<double> mFpga1valuesMean;
    std::vector<double> mMic0valuesMean;
    std::vector<double> mMic1valuesMean;
    std::vector<double> mSystemvaluesMean;

    std::vector<double> mTimevaluesMedian;
    std::vector<double> mCpu0valuesMedian;
    std::vector<double> mCpu1valuesMedian;
    std::vector<double> mGpu0valuesMedian;
    std::vector<double> mGpu1valuesMedian;
    std::vector<double> mFpga0valuesMedian;
    std::vector<double> mFpga1valuesMedian;
    std::vector<double> mMic0valuesMedian;
    std::vector<double> mMic1valuesMedian;
    std::vector<double> mSystemvaluesMedian;

    unsigned int mCurrentAppSize;
    bool enableApplications;
    std::vector<Application> mApplications;
    std::vector<QwtPlotMarker*> mMarker;
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
    virtual void updateApplications(const std::vector<Application>& apps);
    virtual void redrawApplications();
    virtual void applicationsEnabled(bool val);
    virtual void radioButtonChecked_Mean();
    virtual void radioButtonChecked_Median();
    virtual void radioButtonChecked_Abs();
    virtual void spinBoxIntervalMean(int val);
    virtual void spinBoxIntervalMedian(int val);

signals:
    void signal_export(QMSMplot*);
    void signal_value(int val);

protected:
    Ui::QMSMplot* ui;
    int mLineWidth;
    unsigned int maxData;
    int mMeanInterval;
    int mMedianInterval;
    int mValue;

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

    QwtSymbol mVerticalLineStart;
    QwtSymbol mVerticalLineEnd;

    QwtPlotMagnifier* mpMagnifier;
    QwtPlotPanner* mpPanner;
    QwtPlotGrid* mpGrid;

    std::vector<QCheckBox*> mBoxes;
    QGroupBox* mGroupbox;

    void computeMedian(std::vector<double>& src, std::vector<double>& dst);
    void computeMean(std::vector<double> &src, std::vector<double>& dst);


};

#endif // QMSMPLOT_H
