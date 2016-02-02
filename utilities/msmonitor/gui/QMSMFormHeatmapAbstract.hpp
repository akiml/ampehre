/*
 * QMSMFormHeatmapAbstract.hpp
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

#ifndef __QMSMFORMHEATMAPABSTRACT_HPP__
#define __QMSMFORMHEATMAPABSTRACT_HPP__

#include <vector>
#include <iostream>

#include <QWidget>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_printfilter.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>

#include "ui_formheatmap.h"

#include "../data/CDataHandler.hpp"
#include "CHeatmapTimer.hpp"
#include "CHeatmap.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormHeatmapAbstract : public QMdiSubWindow, protected FormHeatmap {
		Q_OBJECT
		
		protected:
			static uint32_t sNumberOfImages;
			
			QWidget* mpFormHeatmapAbstract;
			
			NData::CDataHandler* mpDataHandler;
			
			CHeatmapTimer mTimer;
			
			std::vector<Ui::Heatmap*> mHeatmaps;
			
		public:
			explicit QMSMFormHeatmapAbstract(QWidget* pParent, NData::CDataHandler* pDataHandler);
			~QMSMFormHeatmapAbstract(void);
			
		protected:
			void createActions(void);
			QwtText createTitle(QString title);
			virtual void setupHeatmaps() = 0;
			
		public:
			void refreshGui(void);
			void startTimer(void);
			void stopTimer(void);
			void joinTimer(void);
			Ui::Heatmap* addHeatmap(const std::string &title);
			
		protected slots:
			void slotRefreshGui(void);
			void saveImage(void);
			void close(void);
			void closeEvent(QCloseEvent *pEvent);
			bool event(QEvent *pEvent);
			
		signals:
			void signalRefreshGui(void);
	};
}

#endif /* __QMSMFORMHEATMAPABSTRACT_HPP__ */
