/*
 * QMSMHeatmap.hpp
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

#ifndef __QMSMHEATMAP_H__
#define __QMSMHEATMAP_H__
 
#include "CHeatmapData.hpp"
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#define START_COLOR			Qt::blue
#define FIRST_COLOR_STOP	0.3, Qt::cyan
#define SECOND_COLOR_STOP	0.5, Qt::green
#define THIRD_COLOR_STOP	0.7, Qt::yellow
#define STOP_COLOR			Qt::red

namespace Ui {
	class QMSMHeatmap: public QwtPlot {
		Q_OBJECT

		public:
			QMSMHeatmap(QWidget * = NULL);
			~QMSMHeatmap();
			
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
			QwtLinearColorMap *mpAxisColorMap;
			HeatmapData *mpHeatmapData;
			
		private:
			void updateYAxis();
	};
}
#endif /* __QMSMHEATMAP_H__ */
