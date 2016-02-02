/*
 * CHeatmapTimer.cpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 01/27/16
 * version: 0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#include "CHeatmapTimer.hpp"

#include "QMSMFormHeatmapAbstract.hpp"

namespace Ui {
	CHeatmapTimer::CHeatmapTimer(uint32_t samplingRate, QMSMFormHeatmapAbstract *pFormHeatmap) :
		CAbstractTimer(samplingRate),
		mpFormHeatmap(pFormHeatmap)
		{
		
		// nothing todo
	}
	
	CHeatmapTimer::~CHeatmapTimer(void) {
		if (mThreadStateRun == true) {
			stopTimer();
			joinTimer();
		}
	}
	
	void *CHeatmapTimer::startThread(void *pThreadObject) {
		CHeatmapTimer* thread = static_cast<CHeatmapTimer*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	void CHeatmapTimer::start(void) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
	}
	
	void CHeatmapTimer::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		while (!mThreadStateStop) {
			nanosleep(&mTimer, NULL);
			
			mpFormHeatmap->refreshGui();
		}
		
		exit();
	}
	
	void CHeatmapTimer::startTimer(void) {
		start();
	}
	
	void CHeatmapTimer::stopTimer(void) {
		stop();
	}
	
	void CHeatmapTimer::joinTimer(void) {
		join();
	}
}
