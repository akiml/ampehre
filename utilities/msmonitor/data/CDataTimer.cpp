/*
 * CDataTimer.cpp
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

#include "CDataTimer.hpp"

namespace NData {
	CDataTimer::CDataTimer(uint32_t samplingRate, NCommon::CMutex *pMutexTimer) :
		CAbstractTimer(samplingRate),
		mpMutexTimer(pMutexTimer)
		{
		
		// nothing todo
	}
	
	CDataTimer::~CDataTimer(void) {
		// nothing todo
	}
	
	void *CDataTimer::startThread(void *pThreadObject) {
		CDataTimer* thread = static_cast<CDataTimer*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	void CDataTimer::start(void) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
	}
	
	void CDataTimer::run (void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		while (!mThreadStateStop) {
			nanosleep(&mTimer, NULL);
			
			mpMutexTimer->unlock();
		}
		
		exit();
	}
}