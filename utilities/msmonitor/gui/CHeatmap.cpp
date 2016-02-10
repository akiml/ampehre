/*
 * CHeatmap.cpp
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
 * version: 0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#include "CHeatmap.hpp"
#include <qwt_scale_draw.h>
namespace Ui {
	Heatmap::Heatmap(QWidget *parent):
		QwtPlot(parent),
		mpSpectrogram(new QwtPlotSpectrogram()),
		mpColorMap(new QwtLinearColorMap(Qt::blue, Qt::red)),
		mpHeatmapData(new HeatmapData()) {

		QwtScaleDraw *scale = new QwtScaleDraw();
		
		mpColorMap->addColorStop(0.3, Qt::cyan);
		mpColorMap->addColorStop(0.5, Qt::green);
		mpColorMap->addColorStop(0.7, Qt::yellow);
		
		mpSpectrogram->setColorMap(*mpColorMap);
		mpSpectrogram->setData(*mpHeatmapData);
		mpSpectrogram->attach(this);
		
		enableAxis(QwtPlot::yLeft, false);
		mpRightAxis = axisWidget(QwtPlot::yRight);
		mpRightAxis->setColorBarEnabled(true);
		mpRightAxis->setColorMap(mpSpectrogram->data().range(),mpSpectrogram->colorMap());
		scale->setMinimumExtent(40);
		mpRightAxis->setScaleDraw(scale);
		updateYAxis();
		enableAxis(QwtPlot::yRight);
		
		plotLayout()->setAlignCanvasToScales(true);
		replot();
	}
	
    Heatmap::~Heatmap() {
		delete mpSpectrogram;
		delete mpHeatmapData;
		delete mpColorMap;
		delete mpRightAxis;
    }

	void Heatmap::setTitle(const std::string &name) {
		QwtText label = QwtText(name.c_str());
		label.setFont(QFont("Helvetica", 12));
		mpRightAxis->setTitle(label);
	}

	void Heatmap::setColorMap(QwtLinearColorMap *colorMap) {
		mpColorMap = colorMap;
		mpSpectrogram->setColorMap(*mpColorMap);
	}

	void Heatmap::setData(double *data, uint32_t size) {
		mpHeatmapData->setDataPtr(data, size);
	}

	void Heatmap::setXInterval(double minX, double maxX) {
		mpHeatmapData->setXInterval(minX, maxX);
		setAxisScale(QwtPlot::xBottom, minX, maxX);
	}
	
    void Heatmap::setYInterval(double minY, double maxY) {
		mpHeatmapData->setYInterval(minY, maxY);
		mpSpectrogram->setData(*mpHeatmapData);
		updateYAxis();
	}
	
	void Heatmap::updateYAxis() {
		double minY = mpHeatmapData->range().minValue();
		double maxY = mpHeatmapData->range().maxValue();
		int interval = (maxY - minY)/5;
		int stepSize = interval + 4 - (interval - 1) % 5;
		setAxisScale(QwtPlot::yRight, minY, maxY, stepSize);
		mpRightAxis->setColorMap(mpSpectrogram->data().range(),mpSpectrogram->colorMap());
	}
	
	void Heatmap::refresh() {
		mpSpectrogram->setData(*mpHeatmapData);
		replot();
	}
}
