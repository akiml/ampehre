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
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library. 
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureIPMI.hpp"
#include "CMeasureIPMIThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pIPMI;
		lib_variant variant = *((lib_variant*) pParams);
		skip_ms_freq skip_ms = *(skip_ms_freq*)((uint64_t*) pParams + 1);
		pParams = (uint64_t*) pParams + 2; 
		
		if(variant == FULL) {
			switch(skip_ms){
				case HIGH:
					pIPMI =  new NLibMeasure::CMeasureIPMI<10, FULL>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
					break;
				case LOW:
				default:
					pIPMI =  new NLibMeasure::CMeasureIPMI<1, FULL>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
			}
		} else {
			switch(skip_ms){
				case HIGH:
					pIPMI =  new NLibMeasure::CMeasureIPMI<10, LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
					break;
				case LOW:
				default:
					pIPMI =  new NLibMeasure::CMeasureIPMI<1, LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
			}
		}
		return  (void*) pIPMI;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pIPMI = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pIPMI;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MEASUREMENT* pMeasurement, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pIPMIThread;
		NLibMeasure::CMeasureAbstractResource* pIPMI = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		if(pIPMI->getVariant() == FULL) {
			pIPMIThread =  new NLibMeasure::CMeasureIPMIThread<FULL>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *pIPMI);
		} else {
			pIPMIThread =  new NLibMeasure::CMeasureIPMIThread<LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *pIPMI);
		}
		
		return (void*) pIPMIThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread) {
		NLibMeasure::CMeasureAbstractThread* pIPMIThread = (NLibMeasure::CMeasureAbstractThread*) pMeasureResThread;
		
		delete pIPMIThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		NLibMeasure::CMeasureAbstractResource* pIPMI = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		pIPMI->trigger_resource_custom(pParams);
	}
}
