/*
 * CMeasureAbstractThreadTimer.hpp
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
 * created: 9/09/14
 * version: 0.1.12 - use more C++ features in library
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.1 - modularized libmeasure
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 */

#ifndef __CMEASURETHREADTIMER_HPP__
#define __CMEASURETHREADTIMER_HPP__

#include "CThread.hpp"
#include "CLogger.hpp"
#include "CMutex.hpp"
#include "CSemaphore.hpp"

namespace NLibMeasure {
	class CMeasureThreadTimer : public CThread {
		protected:
			pthread_t mThreadID;
			int32_t mThreadNum;
			bool mThreadStateRun;
			bool mThreadStateStop;
			
			CLogger& mrLog;
			
			CSemaphore& mrStartSem;
			
			struct timespec *mpTimer;
			double mTimerHertz;
			double mTimerMilliSecond;
			CMutex* mpMutexTimer;
			
			std::string mThreadName;
						
		protected:
			void run(void);
			void exit(void);
			
		public:
			CMeasureThreadTimer(CLogger &rLogger, CSemaphore &rStartSem, struct timespec* pTimer, const char* cpThreadName);
			CMeasureThreadTimer(CLogger &rLogger, CSemaphore &rStartSem);
			~CMeasureThreadTimer();
		
		private:
			static void* startThread(void* pThreadObject);
			
		public:
			void start(CMutex *pMutexStart = 0);
			void stop(void);
			void join(void);
			
			void shareMutex(CMutex *pMutexTimer);
			void setTimer(struct timespec *mpTimer);
			void setThreadName(std::string threadName);
			double getTimerHertz();
			double getTimerMillisecond();
	};
}

#endif /* __CMEASURETHREADTIMER_HPP__ */
