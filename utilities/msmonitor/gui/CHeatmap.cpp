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

namespace Ui {
	Heatmap::Heatmap(QWidget *parent):
		QwtPlot(parent){
		mpSpectrogram = new QwtPlotSpectrogram();
		mpHeatmapData = new HeatmapData();
		mpColorMap = new QwtLinearColorMap(Qt::blue, Qt::red);
		
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

		setAxisScale(QwtPlot::yRight, mpSpectrogram->data().range().minValue(),
					mpSpectrogram->data().range().maxValue());
		enableAxis(QwtPlot::yRight);
		
		plotLayout()->setAlignCanvasToScales(true);
		replot();
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

	void Heatmap::refresh() {
		mpSpectrogram->setData(*mpHeatmapData);
		replot();
	}
}
