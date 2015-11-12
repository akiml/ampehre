/*
 * CMeasureAbstractResource.hpp
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
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#ifndef __CMEASUREABSTRACTRESOURCE_HPP__
#define __CMEASUREABSTRACTRESOURCE_HPP__

#include <stdint.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <utility>

#include "CLogger.hpp"
#include "CMutex.hpp"

#include "../../include/measurement.h"

#define UINT64_MAX (0xffffffffffffffff)

namespace NLibMeasure {
	class CMeasureAbstractResource {
		protected:
			CLogger& mrLog;
			uint64_t mMeasureCounter;
			
		public:
			CMeasureAbstractResource(CLogger& rLogger);
			virtual ~CMeasureAbstractResource();
			
		private:
			virtual void init(void) = 0;
			virtual void destroy(void) = 0;
			
		public:
			virtual void measure(MEASUREMENT *pMeasurement, int32_t& rThreadNum) = 0;
			virtual void read_memory_total(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			virtual void trigger_resource_custom(void* pParams);
	};
}

#endif /* __CMEASUREABSTRACTRESOURCE_HPP__ */
