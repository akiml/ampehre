/*
 * QMSMFormSystemOverview.hpp
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
 * created: 02/24/16
 * version: 0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 */

#ifndef __QMSMFORMSYSTEMOVERVIEW_H__
#define __QMSMFORMSYSTEMOVERVIEW_H__
 
#include "ui_formsystemoverview.h"
#include "../data/CDataHandler.hpp"
#include "CGuiTimer.hpp"
#include "QMSMGroupboxSystemOverview.hpp"
#include <QMdiSubWindow>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <math.h>
#include <deque>

#define MAX_VISIBLE_GPU_PROCSSES 5

namespace Ui {
	class QMSMFormSystemOverview:
		public QMdiSubWindow, private FormSystemOverview {
		Q_OBJECT
		
		private:
			QWidget* mpFormSystemOverview;
			
			NData::CDataHandler* mpDataHandler;
			
			CGuiTimer<QMSMFormSystemOverview> mTimer;
			
			static QMSMFormSystemOverview* spFormSystemOverview;
			
			std::deque< std::pair<uint32_t, std::string> > mGPUProcessList;
			
			QMSMGroupboxSystemOverview *mpGroupboxCPU;
			QMSMGroupboxSystemOverview *mpGroupboxGPU;
			QMSMGroupboxSystemOverview *mpGroupboxFPGA;
			QMSMGroupboxSystemOverview *mpGroupboxMIC;
			
			void initGUI(void);
			void addFPGAProcessToTable(uint32_t pid,const std::string &name,const std::string &user);
			void addGPUProcessesToTable(uint32_t processCount, uint32_t *pids, std::string *names);
			void updateActiveProcesses(void);
			void updateUtilization(void);
			void updateTemperature(void);
			double calcMean(double* data, uint32_t size);
			void createActions(void);
			
		public:
			explicit QMSMFormSystemOverview(QWidget *pParent, NData::CDataHandler* pDataHandler);
			~QMSMFormSystemOverview();
			
			static QMSMFormSystemOverview* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
			void refreshGui(void);
			void startTimer(void);
			void stopTimer(void);
			void joinTimer(void);
		
		protected slots:
			void slotRefreshGui(void);
			void closeEvent(QCloseEvent *pEvent);
			bool event(QEvent *pEvent);
			
		signals:
			void signalRefreshGui(void);
	
	};
} 
#endif /* __QMSMFORMSYSTEMOVERVIEW_H__ */