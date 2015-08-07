/*
 * CMeasureStubThread.cpp
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

#include "CMeasureStubThread.hpp"

namespace NLibMeasure {
	CMeasureStubThread::CMeasureStubThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMeasurement, rMeasureRes)
		{
		mThreadType = "stub";
	}
	
	CMeasureStubThread::~CMeasureStubThread() {
		// nothing todo
	}
	
	void CMeasureStubThread::start(CMutex* pMutexStart) {
		mpMutexStart = pMutexStart;
		mpMutexStart->unlock();
	}

	void CMeasureStubThread::stop(void) {
		//norhing todo
	}
	
	void CMeasureStubThread::join(void) {
		//norhing todo
	}
	
	void CMeasureStubThread::exit(void) {
		//norhing todo
	}
	
	void CMeasureStubThread::run(void) {
		//norhing todo
	}	
}
