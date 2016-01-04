/*
 * CMeasureStub.cpp
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
 *          0.7.0 - modularised measurement struct
 */

#include "CMeasureStub.hpp"

namespace NLibMeasure {
	CMeasureStub::CMeasureStub(CLogger& rLogger) :
		CMeasureAbstractResource(rLogger)
		{
		
		//nothing todo
	}
	
	CMeasureStub::~CMeasureStub() {
		//nothing todo
	}
	
	void CMeasureStub::init(void) {
		//nothing todo
	}
	
	void CMeasureStub::destroy(void) {
		//nothing todo
	}
	
	void CMeasureStub::measure(void* pMsMeasurement, int32_t& rThreadNum) {
		//nothing todo
	}
	
	int CMeasureStub::getVariant() {
		//nothing todo
		return 0;
	}
}
