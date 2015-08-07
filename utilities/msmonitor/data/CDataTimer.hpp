/*
 * CDataTimer.hpp
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

#ifndef __CDATATIMER_HPP__
#define __CDATATIMER_HPP__

#include "../common/CAbstractTimer.hpp"

namespace NData {
	class CDataTimer : public NCommon::CAbstractTimer {
		private:
			NCommon::CMutex* mpMutexTimer;
			
		public:
			CDataTimer(uint32_t samplingRate, NCommon::CMutex* pMutexTimer);
			~CDataTimer(void);
			
		protected:
			void run(void);
			
		public:
			void start(void);
			
		private:
			static void* startThread(void* pThreadObject);
	};
}

#endif /* __CDATATIMER_HPP__ */
