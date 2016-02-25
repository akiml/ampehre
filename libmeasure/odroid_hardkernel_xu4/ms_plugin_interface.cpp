/*
 * ms_plugin_interface.cpp
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

#include "../../include/ms_plugin_interface.h"

#include "CMeasureOdroid.hpp"
#include "CMeasureOdroidThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, lib_variant variant, skip_ms_rate skip_ms, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pOdroid;
	
		pOdroid =  new NLibMeasure::CMeasureOdroid(*((NLibMeasure::CLogger*)pLogger));
		
		return  (void*) pOdroid;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pOdroid = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pOdroid;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MS_LIST* pMs_List, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pOdroidThread;
		NLibMeasure::CMeasureAbstractResource* pOdroid = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		pOdroidThread =  new NLibMeasure::CMeasureOdroidThread(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, ODROID), *pOdroid);
		
		return (void*) pOdroidThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread) {
		NLibMeasure::CMeasureAbstractThread* pOdroidThread = (NLibMeasure::CMeasureAbstractThread*) pMeasureResThread;
		
		delete pOdroidThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		// No additional custom function for this resource.
	}
}
