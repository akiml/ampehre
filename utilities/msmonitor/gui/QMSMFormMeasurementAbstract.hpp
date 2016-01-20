/*
 * QMSMFormMeasurementAbstract.hpp
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
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 1/28/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#ifndef __QMSMFORMMEASUREMENTABSTRACT_HPP__
#define __QMSMFORMMEASUREMENTABSTRACT_HPP__

#include <vector>

#include <QWidget>
#include <QMdiSubWindow>
#include <QCloseEvent>

#include "ui_formmeasurement.h"

#include "CGuiTimer.hpp"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_printfilter.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormMeasurementAbstract : public QMdiSubWindow, protected FormMeasurement {
		Q_OBJECT
		
		protected:
			static uint32_t sNumberOfImages;
			
			QWidget* mpFormMeasurementAbstract;
			
			NData::CDataHandler* mpDataHandler;
			
			CGuiTimer mTimer;
			
			QColor mColorCpu;
			QColor mColorCpuAlternative;
			QColor mColorGpu;
			QColor mColorGpuAlternative;
			QColor mColorFpga;
			QColor mColorFpgaAlternative;
			QColor mColorMic;
			QColor mColorMicAlternative;
			QColor mColorSystem;
			QColor mColorSystemAlternative;
			
			QwtSymbol mVerticalLine;
			
			std::vector<QwtPlotMarker *> mMarker;
			QString mMarkerLabel;
			double mMarkerTime;
			
			QwtLegend *mpLegend;
			QwtPlotPanner *mpPanner;
			QwtPlotMagnifier *mpMagnifier;
			QwtPlotGrid *mpGrid;
			
		public:
			explicit QMSMFormMeasurementAbstract(QWidget* pParent, NData::CDataHandler* pDataHandler);
			~QMSMFormMeasurementAbstract(void);
			
		protected:
			void createActions(void);
			QwtText createTitle(QString title);
			virtual void setupCurves() = 0;
			void scaleAxis(double xValue, double yValueMin, double yValueMax);
			virtual double getMiddleOfYAxis(void) = 0;
			
		public:
			void refreshGui(void);
			void startTimer(void);
			void stopTimer(void);
			void joinTimer(void);
			void triggerMarkerSignal(QString &rLabel);
			void resetMarker(void);
			
		protected slots:
			void slotRefreshGui(void);
			void saveImage(void);
			void close(void);
			void closeEvent(QCloseEvent *pEvent);
			bool event(QEvent *pEvent);
			void addMarker(void);
			
		signals:
			void signalRefreshGui(void);
			void signalAddMarker(void);
	};
}

#endif /* __QMSMFORMMEASUREMENTABSTRACT_HPP__ */
