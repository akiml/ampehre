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

#include "CMeasureMaxeler.hpp"
#include "CMeasureMaxelerThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pMaxeler;
		lib_variant variant = *((lib_variant*) pParams);
		skip_ms_freq skip_ms = *(skip_ms_freq*)((uint64_t*) pParams + 1);
		
		if(variant == FULL) {
			switch(skip_ms){
				case HIGH:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<10, FULL>(*((NLibMeasure::CLogger*)pLogger));
					break;
				case LOW:
				default:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<1, FULL>(*((NLibMeasure::CLogger*)pLogger));
			}
		} else {
			switch(skip_ms){
				case HIGH:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<10, LIGHT>(*((NLibMeasure::CLogger*)pLogger));
					break;
				case LOW:
				default:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<1, LIGHT>(*((NLibMeasure::CLogger*)pLogger));
			}
		}
		return  (void*) pMaxeler;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pMaxeler = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pMaxeler;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MEASUREMENT* pMeasurement, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pMaxelerThread;
		NLibMeasure::CMeasureAbstractResource* pMaxeler = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		if(pMaxeler->getVariant() == FULL) {
			pMaxelerThread =  new NLibMeasure::CMeasureMaxelerThread<FULL>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *pMaxeler);
		} else {
			pMaxelerThread =  new NLibMeasure::CMeasureMaxelerThread<LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), pMeasurement, *pMaxeler);
		}
			
		return (void*) pMaxelerThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread){
		NLibMeasure::CMeasureAbstractThread* pMaxelerThread = (NLibMeasure::CMeasureAbstractThread*) pMeasureResThread;
		
		delete pMaxelerThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		NLibMeasure::CMeasureAbstractResource* pMaxeler = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		pMaxeler->trigger_resource_custom(pParams);
	}
}
