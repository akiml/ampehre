/*
 * CTimer.cpp
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
 * created: 1/28/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 */

#include "CAbstractTimer.hpp"

namespace NCommon {
	CAbstractTimer::CAbstractTimer(uint32_t samplingRate) :
		CThread(),
		mSamplingRate(samplingRate),
		mThreadStateRun(false),
		mThreadStateStop(true),
		mTimer()
		{
		
		mTimer.tv_sec	= mSamplingRate/1000;
		mTimer.tv_nsec	= mSamplingRate%1000 * 1000000;
	}
	
	CAbstractTimer::~CAbstractTimer() {
		// nothing todo
	}
	
	void CAbstractTimer::stop (void) {
		mThreadStateStop = true;
	}
	
	void CAbstractTimer::exit (void) {
		mThreadStateRun = false;
		
		pthread_exit(NULL);
	}
	
	void CAbstractTimer::join (void) {
		pthread_join(mThreadID, NULL);
	}
}
