/*
 * QMSMMainWindow.hpp
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
 * created: 1/16/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#ifndef __QMSMMAINWINDOW_HPP__
#define __QMSMMAINWINDOW_HPP__

#include <QMainWindow>
#include "ui_mainwindow.h"

#include "../data/CDataHandler.hpp"
#include "CGuiMarker.hpp"

namespace Ui {
	class QMSMFormInfo;
	class QMSMFormSettings;
	class QMSMFormMeasurementAbstract;
	class QMSMFormHeatmapAbstract;
	
	class QMSMMainWindow : public QMainWindow, private MainWindow {
		Q_OBJECT
		
		private:
			QApplication* mpApplication;
			QMSMFormInfo* mpFormInfo;
			QMSMFormSettings* mpFormSettings;
			NData::CDataHandler& mrDataHandler;
			
			QMSMFormMeasurementAbstract* mpFormPower;
			QMSMFormMeasurementAbstract* mpFormTemperature;
			QMSMFormMeasurementAbstract* mpFormClock;
			QMSMFormMeasurementAbstract* mpFormUtilization;
			QMSMFormMeasurementAbstract* mpFormMemory;
			
			QMSMFormHeatmapAbstract *mpFormHeatmapUtilization;
			QMSMFormHeatmapAbstract *mpFormHeatmapTemperature;
			
			CGuiMarker mGuiMarker;
			
		public:
			explicit QMSMMainWindow(QWidget* pParent, NData::CDataHandler& rDataHandler, QApplication* pApplication);
			~QMSMMainWindow();
			
		public:
			void addMarker(int pid, int id, int signal);
			
		public slots:
			void quitApplication(void);
			void showSettings(void);
			void showAbout(void);
			void pressStart(void);
			void pressStop(void);
			void pressReset(void);
			void showPower(void);
			void showTemperature(void);
			void showClock(void);
			void showUtilization(void);
			void showMemory(void);
			void showHeatmapUtilization(void);
			void showHeatmapTemperature(void);
			bool event(QEvent *pEvent);
			
		private:
			void createActions(void);
			void createShortcuts(void);
	};
}

#endif /* __QMSMMAINWINDOW_HPP__ */
