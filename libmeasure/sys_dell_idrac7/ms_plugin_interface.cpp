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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularised measurement struct
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureIPMI.hpp"
#include "CMeasureIPMIThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, lib_variant variant, skip_ms_rate skip_ms, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pIPMI;
	
		if(variant == VARIANT_FULL) {
			switch(skip_ms){
				case SKIP_PERIODIC:
					pIPMI =  new NLibMeasure::CMeasureIPMI<10, VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
					break;
				case SKIP_NEVER:
				default:
					pIPMI =  new NLibMeasure::CMeasureIPMI<1, VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
			}
		} else {
			switch(skip_ms){
				case SKIP_PERIODIC:
					pIPMI =  new NLibMeasure::CMeasureIPMI<10, VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
					break;
				case SKIP_NEVER:
				default:
					pIPMI =  new NLibMeasure::CMeasureIPMI<1, VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((uint64_t*)pParams));
			}
		}
		return  (void*) pIPMI;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pIPMI = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pIPMI;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MS_LIST* pMs_List, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pIPMIThread;
		NLibMeasure::CMeasureAbstractResource* pIPMI = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		if(pIPMI->getVariant() == VARIANT_FULL) {
			pIPMIThread =  new NLibMeasure::CMeasureIPMIThread<VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, SYSTEM), *pIPMI);
		} else {
			pIPMIThread =  new NLibMeasure::CMeasureIPMIThread<VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, SYSTEM), *pIPMI);
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
