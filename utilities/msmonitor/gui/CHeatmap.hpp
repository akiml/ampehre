/*
 * CHeatmap.hpp
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

#ifndef __CHEATMAP_H__
#define __CHEATMAP_H__
 
#include "CHeatmapData.hpp"
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

namespace Ui {
	class Heatmap: public QwtPlot {
		Q_OBJECT

		public:
			Heatmap(QWidget * = NULL);
			~Heatmap();
			
			void setTitle(const std::string &name);
			void setColorMap(QwtLinearColorMap *colorMap);
			void setData(double *data, uint32_t size);
			void setXInterval(double minX, double maxX);
			void setYInterval(double minY, double maxY);
			void refresh();
			

		private:
			QwtPlotSpectrogram *mpSpectrogram;
			QwtScaleWidget *mpRightAxis;
			QwtLinearColorMap *mpColorMap;
			HeatmapData *mpHeatmapData;
			
		private:
			void updateYAxis();
	};
}
#endif /* __CHEATMAP_H__ */