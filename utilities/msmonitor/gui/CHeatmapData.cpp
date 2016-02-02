/*
 * CHeatmapData.hpp
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

#include "CHeatmapData.hpp"
//TODO delete
#include <iostream>
namespace Ui {
	HeatmapData::HeatmapData():
		 mpData(0), mSize(0), mMinX(0), mMaxX(0) {

	}

	HeatmapData::HeatmapData(double *data, uint32_t size, uint32_t minX, uint32_t maxX):
		QwtRasterData(QwtDoubleRect(0, minX, minX+size, 1.0)),
		mpData(data),
		mSize(size),
		mMinX(minX),
		mMaxX(maxX) {

	}
			
	QwtRasterData * HeatmapData::copy() const {
		return new HeatmapData(mpData, mSize, mMinX, mMaxX);
	}
		
	QwtDoubleInterval HeatmapData::range() const {
		return QwtDoubleInterval(0.0, 100.0);
	}
			
	double HeatmapData::value(double x, double y) const {
		//TODO not correct at the moment
		double interval = (mMaxX-mMinX);
		int index = (x/(double)interval*mSize);
		//std::cout << "x: " << x << "index: " << index << std::endl;
		return mpData[index];
	}

	QSize HeatmapData::rasterHint(const QwtDoubleRect& rect) const {
		return QSize(mSize, 1);
	}

	void HeatmapData::setDataPtr(double* data, uint32_t size) {
		mpData = data;
		mSize = size;
	}

	void HeatmapData::setXInterval(double minX, double maxX) {
		mMinX = minX;
		mMaxX = maxX;
	}
}


