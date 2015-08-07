/*
 * CGuiTimer.cpp
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

#include "CGuiTimer.hpp"

#include "QMSMFormMeasurementAbstract.hpp"

namespace Ui {
	CGuiTimer::CGuiTimer(uint32_t samplingRate, QMSMFormMeasurementAbstract *pFormPlot) :
		CAbstractTimer(samplingRate),
		mpFormPlot(pFormPlot)
		{
		
		// nothing todo
	}
	
	CGuiTimer::~CGuiTimer(void) {
		if (mThreadStateRun == true) {
			stopTimer();
			joinTimer();
		}
	}
	
	void *CGuiTimer::startThread(void *pThreadObject) {
		CGuiTimer* thread = static_cast<CGuiTimer*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	void CGuiTimer::start(void) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
	}
	
	void CGuiTimer::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		while (!mThreadStateStop) {
			nanosleep(&mTimer, NULL);
			
			mpFormPlot->refreshGui();
		}
		
		exit();
	}
	
	void CGuiTimer::startTimer(void) {
		start();
	}
	
	void CGuiTimer::stopTimer(void) {
		stop();
	}
	
	void CGuiTimer::joinTimer(void) {
		join();
	}
}
