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
 * created: 5/27/15
 * version: 0.5.4 - add dynamic loading of resource specific libraries
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureMSR.hpp"
#include "CMeasureMSRThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pMSR;
		skip_ms_freq skip_ms = *((skip_ms_freq*) pParams);
		pParams = (uint64_t*) pParams + 1; 
		switch(skip_ms){
			case LOW:
				pMSR =  new NLibMeasure::CMeasureMSR<1>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
				break;
			case HIGH:
				pMSR =  new NLibMeasure::CMeasureMSR<10>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
				break;
			default:
				pMSR = new NLibMeasure::CMeasureMSR<1>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
		}
		return  (void*) pMSR;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pMSR = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pMSR;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MEASUREMENT* pMeasurement, void* pMeasureRes){
		NLibMeasure::CMeasureMSRThread* pMSRThread =  new NLibMeasure::CMeasureMSRThread(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *((NLibMeasure::CMeasureAbstractResource*)pMeasureRes));
		
		return (void*) pMSRThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread) {
		NLibMeasure::CMeasureMSRThread* pMSRThread = (NLibMeasure::CMeasureMSRThread*) pMeasureResThread;
		
		delete pMSRThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		// No additional custom function for this resource.
	}
}
