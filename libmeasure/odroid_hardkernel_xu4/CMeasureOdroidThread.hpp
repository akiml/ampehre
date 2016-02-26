/*
 * CMeasureOdroidThread.hpp
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 2/25/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#ifndef __CMEASUREODROIDTHREAD_HPP__
#define __CMEASUREODROIDTHREAD_HPP__

#include "../common/CMeasureAbstractThread.hpp"
#include "CMeasureOdroid.hpp"

namespace NLibMeasure {
	class CMeasureOdroidThread : public CMeasureAbstractThread {
		public:
			CMeasureOdroidThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes);
			~CMeasureOdroidThread();
			
		private:
			void run(void);
	};
}

#include "CMeasureOdroidThread.cpp"

#endif /* __CMEASUREODROIDTHREAD_HPP__ */
