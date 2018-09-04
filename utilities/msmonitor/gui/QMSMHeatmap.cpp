/*
 * QMSMHeatmap.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 01/27/16
 * version: 0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#include "QMSMHeatmap.hpp"
#include <qwt_scale_draw.h>
namespace Ui {
	QMSMHeatmap::QMSMHeatmap(QWidget *parent):
		QwtPlot(parent),
		mpSpectrogram(new QwtPlotSpectrogram()),
		mpColorMap(new QwtLinearColorMap(START_COLOR, STOP_COLOR)),
		mpHeatmapData(new HeatmapData()) {

		QwtScaleDraw *scale = new QwtScaleDraw();
		
		mpColorMap->addColorStop(FIRST_COLOR_STOP);
		mpColorMap->addColorStop(SECOND_COLOR_STOP);
		mpColorMap->addColorStop(THIRD_COLOR_STOP);
		
		mpSpectrogram->setColorMap(mpColorMap);
		mpSpectrogram->setData(mpHeatmapData);
		mpSpectrogram->attach(this);
		
		enableAxis(QwtPlot::yLeft, false);
		mpRightAxis = axisWidget(QwtPlot::yRight);
		mpRightAxis->setColorBarEnabled(true);
		mpRightAxis->setColorMap(mpSpectrogram->data()->interval(Qt::XAxis),(QwtColorMap*)(mpSpectrogram->colorMap()));
		scale->setMinimumExtent(40);
		mpRightAxis->setScaleDraw(scale);
		updateYAxis();
		enableAxis(QwtPlot::yRight);
		
		plotLayout()->setAlignCanvasToScales(true);
		replot();
	}
	
    QMSMHeatmap::~QMSMHeatmap() {
		delete mpSpectrogram;
		delete mpHeatmapData;
		delete mpColorMap;
		delete mpRightAxis;
    }

	void QMSMHeatmap::setTitle(const std::string &name) {
		QwtText label = QwtText(name.c_str());
		label.setFont(QFont("Helvetica", 12));
		mpRightAxis->setTitle(label);
	}

	void QMSMHeatmap::setColorMap(QwtLinearColorMap *colorMap) {
		mpColorMap = colorMap;
		mpSpectrogram->setColorMap(mpColorMap);
	}

	void QMSMHeatmap::setData(double *data, uint32_t size) {
		mpHeatmapData->setDataPtr(data, size);
	}

	void QMSMHeatmap::setXInterval(double minX, double maxX) {
		mpHeatmapData->setXInterval(minX, maxX);
		setAxisScale(QwtPlot::xBottom, minX, maxX);
	}
	
    void QMSMHeatmap::setYInterval(double minY, double maxY) {
		mpHeatmapData->setYInterval(minY, maxY);
		mpSpectrogram->setData(mpHeatmapData);
		updateYAxis();
	}
	
	void QMSMHeatmap::updateYAxis() {
		double minY = mpHeatmapData->range().minValue();
		double maxY = mpHeatmapData->range().maxValue();
		int interval = (maxY - minY)/5;
		int stepSize = interval + 4 - (interval - 1) % 5;
		setAxisScale(QwtPlot::yRight, minY, maxY, stepSize);
		mpRightAxis->setColorMap(mpSpectrogram->data()->interval(Qt::XAxis),(QwtColorMap*)(mpSpectrogram->colorMap()));
	}
	
	void QMSMHeatmap::refresh() {
		mpSpectrogram->setData(mpHeatmapData);
		replot();
	}
}
