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
 */
#include "CMeasureAbstractResource.hpp"

namespace NLibMeasure {
    CMeasureAbstractResource::CMeasureAbstractResource(CLogger & rLogger):
		mrLog(rLogger),
		mMutex()
		{
		// nothing todo
    }
    
    CMeasureAbstractResource::~CMeasureAbstractResource() {
		// nothing todo
    }
}