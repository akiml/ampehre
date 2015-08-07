/*
 * ms_plugin_interface.cpp
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
 * created: 5/29/15
 * version: 0.5.4 - add dynamic loading of resource specific libraries
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureMaxeler.hpp"
#include "CMeasureMaxelerThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, uint64_t* pParams){
		NLibMeasure::CMeasureMaxeler* pMaxeler =  new NLibMeasure::CMeasureMaxeler(*((NLibMeasure::CLogger*)pLogger));
		
		return  (void*) pMaxeler;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureMaxeler* pMaxeler = (NLibMeasure::CMeasureMaxeler*) pMeasureRes;
		
		delete pMaxeler;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MEASUREMENT* pMeasurement, void* pMeasureRes){
		NLibMeasure::CMeasureMaxelerThread* pMaxelerThread =  new NLibMeasure::CMeasureMaxelerThread(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *((NLibMeasure::CMeasureAbstractResource*)pMeasureRes));
		
		return (void*) pMaxelerThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread){
		NLibMeasure::CMeasureMaxelerThread* pMaxelerThread = (NLibMeasure::CMeasureMaxelerThread*) pMeasureResThread;
		
		delete pMaxelerThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes){
		NLibMeasure::CMeasureMaxeler* pMaxeler = (NLibMeasure::CMeasureMaxeler*) pMeasureRes;
		pMaxeler->forceIdle();
	}
}
