/*
 * CMeasureAbstractThread.hpp
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
 * created: 5/22/15
 * version: 0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library.
 *          0.7.0 - modularised measurement struct
 */

#ifndef __CMEASUREABSTRACTTHREAD_HPP__
#define __CMEASUREABSTRACTTHREAD_HPP__

#include <iostream>
#include <ctime>
#include <string>

#include <stdint.h>

#include "CMeasureAbstractResource.hpp"
#include "CMeasureThreadTimer.hpp"
#include "CThread.hpp"
#include "CMutex.hpp"
#include "CSemaphore.hpp"

#include "../../include/ms_measurement.h"

#define UINT64_MAX (0xffffffffffffffff)

namespace NLibMeasure {
	class CMeasureAbstractThread : protected CThread {
		protected:
			pthread_t mThreadID;
			int32_t mThreadNum;
			bool mThreadStateRun;
			bool mThreadStateStop;
			
			CLogger& mrLog;
			
			CSemaphore& mrStartSem;
			
			void *mpMsMeasurement;
			CMeasureAbstractResource& mrMeasureResource;
			CMeasureThreadTimer mTimer;
			CMutex mMutexTimer;
			
			CMutex *mpMutexStart;
			
			std::string mThreadType;
			
		public:
			CMeasureAbstractThread(CLogger& rLogger, CSemaphore& rStartSem, void *pMsMeasurement, CMeasureAbstractResource& rMeasureResource);
			virtual ~CMeasureAbstractThread();
			
		private:
			virtual void run(void) = 0;
			
		public:
			static void calcTimeDiff(struct timespec* time_cur, struct timespec* time_temp, struct timespec* time_diff, double* time_diff_double);
			static void* startThread(void* pThreadObject);
			virtual void start(CMutex *pMutexStart = 0);
			virtual void stop(void);
			virtual void join(void);
			virtual void exit(void);
	};
}

#endif /* __CMEASUREABSTRACTTHREAD_HPP__ */