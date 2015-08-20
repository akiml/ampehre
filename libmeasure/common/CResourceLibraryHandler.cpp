/*
 * CResourceLibraryHandler.cpp
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
 * created: 6/11/15
 * version: 0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 */

#include "CResourceLibraryHandler.hpp"

typedef void* (*init_res_t)(void*, uint64_t*);
typedef void (*fini_res_t)(void*);

typedef void* (*init_measure_thread_t)(void*, void*, MEASUREMENT*, void* pMeasureRes);
typedef void (*fini_measure_thread_t)(void*);

namespace NLibMeasure{
	CResourceLibraryHandler::CResourceLibraryHandler(CLogger& rLogger,const std::string& rLibname, uint64_t* pParams):
		mpLibhandler(0),
		mpResource(0),
		mpResourceThread(0),
		mMutexStartResource(),
		mrLog(rLogger)
		{
			mpLibhandler = openLibrary(rLibname);
			initResource(pParams);
	}

	CResourceLibraryHandler::~CResourceLibraryHandler() {
		finiResource();
		closeLibrary(&mpLibhandler);
	}
    
	CMeasureAbstractThread* CResourceLibraryHandler::getResourceThread() {
		return mpResourceThread;
	}
    
	CMutex& CResourceLibraryHandler::getMutexStartResource() {
		return mMutexStartResource;
	}
	
	CMeasureAbstractResource* CResourceLibraryHandler::getResource() {
		return mpResource;
	}
	
	void CResourceLibraryHandler::initResource(uint64_t* pParams) {
		init_res_t init_resource = (init_res_t) loadFunction("init_resource");
		
		mpResource = (NLibMeasure::CMeasureAbstractResource*) init_resource((void*)(&mrLog), pParams);
	}

    
	void CResourceLibraryHandler::initResourceThread(CSemaphore& rStartSem, MEASUREMENT* pMeasurement) {
		init_measure_thread_t init_thread = (init_measure_thread_t) loadFunction("init_resource_thread");
		
		mpResourceThread = (CMeasureAbstractThread*) init_thread((void*) &mrLog, (void*) &rStartSem, pMeasurement, (void*) mpResource);
	}

	void CResourceLibraryHandler::finiResourceThread() {
		fini_measure_thread_t fini_measure_thread = (fini_measure_thread_t) loadFunction("fini_resource_thread");
		
		fini_measure_thread((void*)mpResourceThread);
		
		mpResourceThread = 0;
	}
	
	void* CResourceLibraryHandler::loadFunction(const std::string& rFunctionname) {
		void* function = (void*) dlsym(mpLibhandler, rFunctionname.c_str());
		char* dlsym_error = dlerror();
		if(dlsym_error){
			mrLog.lock();
			mrLog()
			<< "!!! 'mgmt' (thread main): Error: Cannot load symbol " << rFunctionname << ": " << dlsym_error << " (file: " << __FILE__ << ", line: " << __LINE__ << ")." << std::endl
			<< std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		return function;
	}
	
	void CResourceLibraryHandler::finiResource() {
		fini_res_t fini_resource = (fini_res_t) loadFunction("fini_resource");
		
		fini_resource((void*)mpResource);
	}
	
	void* CResourceLibraryHandler::openLibrary(const std::string& rLibname) {
		void* handler = dlopen(rLibname.c_str() , RTLD_LAZY | RTLD_LOCAL);
		if (NULL == handler) {
			mrLog.lock();
			mrLog()
			<< "!!! 'mgmt' (thread main): Error: Cannot open library " << rLibname << ": " << dlerror() << " (file: " << __FILE__ << ", line: " << __LINE__ << ")." << std::endl
			<< std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		return handler;
	}

	void CResourceLibraryHandler::closeLibrary(void** handler) {
		dlclose(*handler);
	
		*handler = NULL;
	}
}