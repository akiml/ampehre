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
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureNVML.hpp"
#include "CMeasureNVMLThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, lib_variant variant, skip_ms_rate skip_ms, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pNVML;
	
		if(variant == VARIANT_FULL) {
			switch(skip_ms){
				case SKIP_PERIODIC:
					pNVML =  new NLibMeasure::CMeasureNVML<10, VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((gpu_frequency*)pParams));
					break;
				case SKIP_NEVER:
				default:
					pNVML =  new NLibMeasure::CMeasureNVML<1, VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((gpu_frequency*)pParams));
			}
		}else {
			switch(skip_ms){
				case SKIP_PERIODIC:
					pNVML =  new NLibMeasure::CMeasureNVML<10, VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((gpu_frequency*)pParams));
					break;
				case SKIP_NEVER:
				default:
					pNVML =  new NLibMeasure::CMeasureNVML<1, VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((gpu_frequency*)pParams));
			}
		}
		return  (void*) pNVML;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pNVML = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pNVML;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MEASUREMENT* pMeasurement, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pNVMLThread;
		NLibMeasure::CMeasureAbstractResource* pNVML = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		if(pNVML->getVariant() == VARIANT_FULL) {
			pNVMLThread =  new NLibMeasure::CMeasureNVMLThread<VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *pNVML);
		} else {
			pNVMLThread =  new NLibMeasure::CMeasureNVMLThread<VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *pNVML);
		}
		
		return (void*) pNVMLThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread) {
		NLibMeasure::CMeasureAbstractThread* pNVMLThread = (NLibMeasure::CMeasureAbstractThread*) pMeasureResThread;
		
		delete pNVMLThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		// No additional custom function for this resource.
	}
}
