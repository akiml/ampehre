/*
 * CGuiTimer.hpp
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
 * created: 1/29/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 */

#ifndef __CGUITIMER_HPP__
#define __CGUITIMER_HPP__

#include "../common/CAbstractTimer.hpp"

namespace Ui {
	class QMSMFormMeasurementAbstract;
	
	template<typename TAbstractGUI>
	class CGuiTimer : public NCommon::CAbstractTimer {
		private:
			TAbstractGUI *mpFormAbstract;
		
		public:
			CGuiTimer(uint32_t samplingRate, TAbstractGUI *pFormAbstract);
			~CGuiTimer(void);
			
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

#include "CGuiTimer.cpp"

#endif /* __CGUITIMER_HPP__ */
