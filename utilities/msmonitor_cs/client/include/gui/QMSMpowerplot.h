/*
 * QMSMpowerplot.h
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



#ifndef QMSMPOWERPLOT_H
#define QMSMPOWERPLOT_H

#include "QMSMplot.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <vector>

#include "CProtocol.hpp"

class QMSMPowerPlot :public QMSMplot
{

public:
    QMSMPowerPlot(int linewidth, int maxData, int width, int height, QWidget *parent);
    virtual ~QMSMPowerPlot();
    virtual QWidget* getPlot();

public slots:
    virtual void updateValues(std::vector<double>& values);
    virtual void redraw();
    virtual void initPlot();
    virtual void resetPen();

};

#endif // QMSMPOWERPLOT_H
