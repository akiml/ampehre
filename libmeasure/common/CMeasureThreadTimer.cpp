/*
 * CMeasureAbstractThreadTimer.cpp
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
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#include "CMeasureThreadTimer.hpp"

namespace NLibMeasure {
	CMeasureThreadTimer::CMeasureThreadTimer (CLogger &rLogger, CSemaphore &rStartSem, struct timespec *pTimer, const char *cpThreadName) :
		CThread(),
		mrLog(rLogger),
		mrStartSem(rStartSem),
		mpTimer(pTimer),
		mTimerHertz(1.0/((double)pTimer->tv_sec + ((double)pTimer->tv_nsec / 1000000000.0))),
		mTimerMilliSecond(((double)pTimer->tv_sec + ((double)pTimer->tv_nsec / 1000000000.0)) * 1000.0),
		mThreadName(cpThreadName)
		{
		// nothing todo
	}
	
	CMeasureThreadTimer::CMeasureThreadTimer (CLogger &rLogger, CSemaphore &rStartSem) :
		CThread(),
		mrLog(rLogger),
		mrStartSem(rStartSem)
		{
		// nothing todo
	}
	
	CMeasureThreadTimer::~CMeasureThreadTimer() {
		// nothing todo
	}
	
	void CMeasureThreadTimer::shareMutex (CMutex *pMutexTimer) {
		mpMutexTimer = pMutexTimer;
	}
	
	void CMeasureThreadTimer::start(CMutex *pMutexStart) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
	}
	
	void* CMeasureThreadTimer::startThread(void* pThreadObject) {
		CMeasureThreadTimer* thread = static_cast<CMeasureThreadTimer*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	void CMeasureThreadTimer::stop (void) {
		mThreadStateStop = true;
	}
	
	void CMeasureThreadTimer::exit (void) {
		mThreadStateRun = false;
		
		pthread_exit(NULL);
	}
	
	void CMeasureThreadTimer::join (void) {
		pthread_join(mThreadID, NULL);
		
		mrLog.lock();
		mrLog() << "<<< '" << mThreadName << "' (thread #" << mThreadNum << "): finish" << std::endl;
		mrLog.unlock();
	}
	
	void CMeasureThreadTimer::run (void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog()
		<< ">>> '" << mThreadName << "' (thread #" << mThreadNum << "): init" << std::endl
		<< "     sampling rate: " << mTimerHertz << " Hz / " << mTimerMilliSecond << " ms" << std::endl;
		mrLog.unlock();
		
		mrStartSem.wait();
		
		while (!mThreadStateStop) {
			nanosleep(mpTimer, NULL);
			
			mpMutexTimer->unlock();
		}
		
		exit();
	}
	
	void CMeasureThreadTimer::setThreadName(std::string threadName){
		mThreadName = threadName;
	}
	
	void CMeasureThreadTimer::setTimer(timespec* pTimer){
		mpTimer = pTimer;
		mTimerHertz = 1.0/((double)pTimer->tv_sec + ((double)pTimer->tv_nsec / 1000000000.0));
		mTimerMilliSecond = ((double)pTimer->tv_sec + ((double)pTimer->tv_nsec / 1000000000.0)) * 1000.0;
	}
}
