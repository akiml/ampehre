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

#ifndef __CHEATMAPDATA_H__
#define __CHEATMAPDATA_H__

#include <qwt_raster_data.h>
#include <stdint.h>

namespace Ui {
	class HeatmapData:
		public QwtRasterData {
			
		public:
			HeatmapData();
			HeatmapData(double *data, uint32_t size, uint32_t mMinX, uint32_t mMaxX);
			
			void setDataPtr(double *data, uint32_t size);
			void setXInterval(double minX, double maxX);	
			
			virtual QwtRasterData *copy() const;	
			virtual QwtDoubleInterval range() const;		
			virtual double value(double x, double y) const;		
			virtual QSize rasterHint(const QwtDoubleRect &rect) const;
			
		private:
			double *mpData;
			uint32_t mSize;
			uint32_t mMinX;
			uint32_t mMaxX;
	};
}

#endif /* __CHEATMAPDATA_H__ */