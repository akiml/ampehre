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

#include "CMeasureMaxeler.hpp"
#include "CMeasureMaxelerThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, lib_variant variant, skip_ms_rate skip_ms, void* pParams){
		NLibMeasure::CMeasureAbstractResource* pMaxeler;
	
		if(variant == VARIANT_FULL) {
			switch(skip_ms){
				case SKIP_PERIODIC:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<10, VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger));
					break;
				case SKIP_NEVER:
				default:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<1, VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger));
			}
		} else {
			switch(skip_ms){
				case SKIP_PERIODIC:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<10, VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger));
					break;
				case SKIP_NEVER:
				default:
					pMaxeler =  new NLibMeasure::CMeasureMaxeler<1, VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger));
			}
		}
		return  (void*) pMaxeler;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureAbstractResource* pMaxeler = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		delete pMaxeler;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MS_LIST* pMs_List, void* pMeasureRes){
		NLibMeasure::CMeasureAbstractThread* pMaxelerThread;
		NLibMeasure::CMeasureAbstractResource* pMaxeler = (NLibMeasure::CMeasureAbstractResource*) pMeasureRes;
		
		if(pMaxeler->getVariant() == VARIANT_FULL) {
			pMaxelerThread =  new NLibMeasure::CMeasureMaxelerThread<VARIANT_FULL>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, FPGA), *pMaxeler);
		} else {
			pMaxelerThread =  new NLibMeasure::CMeasureMaxelerThread<VARIANT_LIGHT>(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), getMeasurement(&pMs_List, FPGA), *pMaxeler);
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
