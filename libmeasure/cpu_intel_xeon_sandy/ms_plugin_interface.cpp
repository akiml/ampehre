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
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library.
 *          0.7.0 - modularised measurement struct 
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureMSR.hpp"
#include "CMeasureMSRThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pMSR;
		lib_variant variant = *((lib_variant*) pParams);
		skip_ms_freq skip_ms = *(skip_ms_freq*)((uint64_t*) pParams + 1);
		pParams = (uint64_t*) pParams + 2; 
		
		if(variant == FULL) {
			switch(skip_ms){
				case HIGH:
					pMSR =  new NLibMeasure::CMeasureMSR<10, FULL>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
					break;
				case LOW:
				default:
					pMSR = new NLibMeasure::CMeasureMSR<1, FULL>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
			}
		} else {
			switch(skip_ms){
				case HIGH:
					pMSR =  new NLibMeasure::CMeasureMSR<10, LIGHT>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
					break;
				case LOW:
				default:
					pMSR = new NLibMeasure::CMeasureMSR<1, LIGHT>(*((NLibMeasure::CLogger*)pLogger), (cpu_governor)((uint64_t*)pParams)[0], ((uint64_t*)pParams)[1], ((uint64_t*)pParams)[2]);
			}
		}
		return  (void*) pMSR;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pMSR = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pMSR;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MS_LIST* pMs_List, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pMSRThread;
		NLibMeasure::CMeasureAbstractResource* pMSR = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		if(pMSR->getVariant() == FULL) {
			pMSRThread =  new NLibMeasure::CMeasureMSRThread<FULL>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, CPU), *pMSR);
		} else {
			pMSRThread =  new NLibMeasure::CMeasureMSRThread<LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, CPU), *pMSR);
		}
		
		return (void*) pMSRThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread) {
		NLibMeasure::CMeasureAbstractThread* pMSRThread = (NLibMeasure::CMeasureAbstractThread*) pMeasureResThread;
		
		delete pMSRThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		// No additional custom function for this resource.
	}
}
