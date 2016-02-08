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

namespace Ui {
	template<typename TAbstractGUI>
	CGuiTimer<TAbstractGUI>::CGuiTimer(uint32_t samplingRate, TAbstractGUI *pFormAbstract) :
		CAbstractTimer(samplingRate),
		mpFormAbstract(pFormAbstract)
		{
		
		// nothing todo
	}
	
	template<typename TAbstractGUI>
	CGuiTimer<TAbstractGUI>::~CGuiTimer(void) {
		if (mThreadStateRun == true) {
			stopTimer();
			joinTimer();
		}
	}
	
	template<typename TAbstractGUI>
	void *CGuiTimer<TAbstractGUI>::startThread(void *pThreadObject) {
		CGuiTimer* thread = static_cast<CGuiTimer*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	template<typename TAbstractGUI>
	void CGuiTimer<TAbstractGUI>::start(void) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
	}
	
	template<typename TAbstractGUI>
	void CGuiTimer<TAbstractGUI>::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		while (!mThreadStateStop) {
			nanosleep(&mTimer, NULL);
			
			mpFormAbstract->refreshGui();
		}
		
		exit();
	}
	
	template<typename TAbstractGUI>
	void CGuiTimer<TAbstractGUI>::startTimer(void) {
		start();
	}
	
	template<typename TAbstractGUI>
	void CGuiTimer<TAbstractGUI>::stopTimer(void) {
		stop();
	}
	
	template<typename TAbstractGUI>
	void CGuiTimer<TAbstractGUI>::joinTimer(void) {
		join();
	}
}
