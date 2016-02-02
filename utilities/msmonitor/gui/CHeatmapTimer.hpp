/*
 * CHeatmapTimer.cpp
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

#ifndef __CHEATMAPTIMER_HPP__
#define __CHEATMAPTIMER_HPP__

#include "../common/CAbstractTimer.hpp"

namespace Ui {
	class QMSMFormHeatmapAbstract;
	
	class CHeatmapTimer : public NCommon::CAbstractTimer {
		private:
			QMSMFormHeatmapAbstract *mpFormHeatmap;
		
		public:
			CHeatmapTimer(uint32_t samplingRate, QMSMFormHeatmapAbstract *pFormHeatmap);
			~CHeatmapTimer(void);
			
		protected:
			void run(void);
			
		public:
			void start(void);
			void startTimer(void);
			void stopTimer(void);
			void joinTimer(void);
			
		private:
			static void* startThread(void* pThreadObject);
	};
}

#endif /* __CHEATMAPTIMER_HPP__ */
