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
 * created: 6/01/15
 * version: 0.5.4 - add dynamic loading of resource specific libraries
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.7.0 - modularized measurement struct
 */

#include "../../include/ms_plugin_interface.h"

#include "CMeasureStub.hpp"
#include "CMeasureStubThread.hpp"

extern "C" {
	void* init_resource(void* pLogger, lib_variant variant, skip_ms_rate skip_ms, void* pParams){
		NLibMeasure::CMeasureStub* pStub =  new NLibMeasure::CMeasureStub(*((NLibMeasure::CLogger*)pLogger));
	
		return  (void*) pStub;
	}
	
	void fini_resource(void* pMeasureRes){
		NLibMeasure::CMeasureStub* pStub = (NLibMeasure::CMeasureStub*) pMeasureRes;
		
		delete pStub;
	}
	
	void* init_resource_thread(void* pLogger, void* pStartSem, MS_LIST* pMs_List, void* pMeasureRes){
		NLibMeasure::CMeasureStubThread* pStubThread =  new NLibMeasure::CMeasureStubThread(*((NLibMeasure::CLogger*)pLogger), *((NLibMeasure::CSemaphore*)pStartSem), NULL, *((NLibMeasure::CMeasureAbstractResource*)pMeasureRes));
		
		return (void*) pStubThread;
	}
	
	void fini_resource_thread(void* pMeasureResThread) {
		NLibMeasure::CMeasureStubThread* pStubThread = (NLibMeasure::CMeasureStubThread*) pMeasureResThread;
		
		delete pStubThread;
	}
	
	void trigger_resource_custom(void* pMeasureRes, void* pParams) {
		// No additional custom function for this resource.
	}
}
