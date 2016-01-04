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
 *          0.7.0 - modularised measurement struct
 */

#include "CMeasureAbstractThread.hpp"

namespace NLibMeasure {
    CMeasureAbstractThread::CMeasureAbstractThread(CLogger& rLogger, CSemaphore& rStartSem, void *pMsMeasurement, CMeasureAbstractResource& rMeasureResource):
		CThread(),
		mThreadID(0),
		mThreadNum(-1),
		mThreadStateRun(false),
		mThreadStateStop(true),
		mrLog(rLogger),
		mrStartSem(rStartSem),
		mpMsMeasurement(pMsMeasurement),
		mrMeasureResource(rMeasureResource),
		mTimer(rLogger, rStartSem),
		mMutexTimer(),
		mpMutexStart(0),
		mThreadType("abstract")
		{
		// nothing todo
    }
    
    CMeasureAbstractThread::~CMeasureAbstractThread() {
		// nothing todo
    }
    
    void CMeasureAbstractThread::start ( CMutex* pMutexStart ) {
		mpMutexStart = pMutexStart;
		
		pthread_create(&mThreadID, NULL, &startThread, (void *) this);
		mTimer.start();
    }

    
    void* CMeasureAbstractThread::startThread(void* pThreadObject) {
		CMeasureAbstractThread* thread = static_cast<CMeasureAbstractThread*>(pThreadObject);
		
		thread->run();
		
		return 0;
    }
    
    void CMeasureAbstractThread::stop(void) {
		mTimer.stop();
		mThreadStateStop = true;
		mMutexTimer.unlock();
	}
    
    void CMeasureAbstractThread::exit(void){
		mThreadStateRun = false;
		
		pthread_exit(NULL);
    }
    
    void CMeasureAbstractThread::join(void) {
		mTimer.join();
		pthread_join(mThreadID, NULL);
		
		mrLog.lock();
		mrLog() << "<<< '" << mThreadType << " thread' (thread #" << mThreadNum << "): finish" << std::endl;
		mrLog.unlock();
	}
	
	void CMeasureAbstractThread::calcTimeDiff(struct timespec* time_cur, struct timespec* time_temp, struct timespec* time_diff, double* time_diff_double){
		clock_gettime(CLOCK_REALTIME, time_cur);
		if ((time_cur->tv_nsec - time_temp->tv_nsec)<0) {
			time_diff->tv_sec	= time_cur->tv_sec - time_temp->tv_sec - 1;
			time_diff->tv_nsec	= 1000000000 + time_cur->tv_nsec - time_temp->tv_nsec;
		} else {
			time_diff->tv_sec	= time_cur->tv_sec - time_temp->tv_sec;
			time_diff->tv_nsec	= time_cur->tv_nsec - time_temp->tv_nsec;
		}
		*time_diff_double = (double)time_diff->tv_sec + ((double)time_diff->tv_nsec / 1000000000.0);
		time_temp->tv_sec	= time_cur->tv_sec;
		time_temp->tv_nsec	= time_cur->tv_nsec;
	}
}