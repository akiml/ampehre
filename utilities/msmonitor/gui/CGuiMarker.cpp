/*
 * CGuiMarker.cpp
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
 * created: 3/06/15
 * version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
 */

#include "CGuiMarker.hpp"

#include "QMSMMainWindow.hpp"

namespace Ui {
	CGuiMarker::CGuiMarker(QMSMMainWindow *pMainWindow) :
		CThread(),
		mThreadStateRun(false),
		mThreadStateStop(true),
		mpMainWindow(pMainWindow),
		mpServer(0)
		{
		
		ushms_server_init(&mpServer);
	}
	
	CGuiMarker::~CGuiMarker(void) {
		if (mThreadStateRun == true) {
			ushms_server_stop(mpServer);
		}
		
		ushms_server_fini(&mpServer);
		
		mpServer = 0;
	}
	
	void *CGuiMarker::startThread(void *pThreadObject) {
		CGuiMarker* thread = static_cast<CGuiMarker*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	void CGuiMarker::start(void) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
	}
	
	void CGuiMarker::stop(void) {
		if (mThreadStateStop == false) {
			ushms_server_stop(mpServer);
		}
		
		mThreadStateStop = true;
	}
	
	void CGuiMarker::exit(void) {
		pthread_join(mThreadID, NULL);
	}
	
	void CGuiMarker::join(void) {
		mThreadStateRun = false;
		
		pthread_exit(NULL);
	}
	
	void CGuiMarker::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		ushms_server_start(mpServer);
		
		while (!mThreadStateStop) {
			int client_id		= 0;
			int client_signal	= 0;
			int client_pid		= 0;
			int accepted		= 0;
			
			accepted = ushms_server_comm(mpServer, &client_id, &client_signal, &client_pid);
			
			if (accepted) {
				mpMainWindow->addMarker(client_pid, client_id, client_signal);
			}
		}
		
		exit();
	}
	
	char *CGuiMarker::getSocketName(void) {
		return mpServer->socket_name;
	}
}
