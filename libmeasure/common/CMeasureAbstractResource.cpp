/*
 * CMeasureAbstractResource.cpp
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
 * created: 5/22/15
 * version: 0.5.3 - add abstract measure and abstract measure thread
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularized measurement struct
 */
#include "CMeasureAbstractResource.hpp"

#include <iostream>

namespace NLibMeasure {
	CMeasureAbstractResource::CMeasureAbstractResource(CLogger & rLogger):
		mrLog(rLogger),
		mMeasureCounter(0)
		{
		// nothing todo
	}
	
	CMeasureAbstractResource::~CMeasureAbstractResource() {
		// nothing todo
	}
	
	void CMeasureAbstractResource::read_memory_total(void *pMsMeasurement, int32_t& rThreadNum) {
		// nothing todo; concrete implementation in subclasses
	}
	
	void CMeasureAbstractResource::trigger_resource_custom(void* pParams){
		// nothing todo
	}
}
