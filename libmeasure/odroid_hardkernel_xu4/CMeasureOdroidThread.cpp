/*
 * CMeasureOdroidThread.cpp
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

#include "CMeasureOdroidThread.hpp"

namespace NLibMeasure {
	CMeasureOdroidThread::CMeasureOdroidThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes):
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		
		mThreadType = "odroid";
		mTimer.setThreadName("odroid timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_ODROID *)pMsMeasurement)->odroid_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	CMeasureOdroidThread::~CMeasureOdroidThread() {
		// nothing todo
	}
	
	void CMeasureOdroidThread::run(void) {
		/* TODO */
	}
}
