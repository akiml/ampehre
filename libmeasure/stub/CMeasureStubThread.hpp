/*
 * CMeasureStubThread.hpp
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 6/01/15
 * version: 0.5.4 - add dynamic loading of resource specific libraries
 */

#ifndef __CMEASURESTUBTHREAD_HPP__
#define __CMEASURESTUBTHREAD_HPP__

#include "../common/CMeasureAbstractThread.hpp"

#include "CMeasureStub.hpp"

namespace NLibMeasure {
	class CMeasureStubThread : public CMeasureAbstractThread {
		public:
			CMeasureStubThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes);
			~CMeasureStubThread();
			
		public:
			void start(CMutex *pMutexStart = 0);
			void stop(void);
			void join(void);
			void exit(void);
			
		private:
			void run(void);
	};
}

#endif /* __CMEASURESTUBTHREAD_HPP__ */