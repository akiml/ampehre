/*
 * CMeasureMIC.hpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 4/2/15
 * version: 0.4.0 - MIC integration into libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.1 - modularised libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library.
 */

#ifndef __CMEASUREMIC_HPP__
#define __CMEASUREMIC_HPP__

#include "../common/CMeasureAbstractResource.hpp"
#include <miclib.h>

namespace NLibMeasure {
	template <int SkipMs, int Variant>
	class CMeasureMIC : public CMeasureAbstractResource {
		private:
			struct mic_device *mpMicDevice;
			struct mic_core_util *mpMicUtilization;
			
		public:
			CMeasureMIC(CLogger& rLogger);
			~CMeasureMIC();
		
		private:
			void init(void);
			void destroy(void);
			
			void micGetPower(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void micGetTemperature(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void micGetFrequency(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void micGetUtil(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void micGetMemory(MEASUREMENT *pMeasurement, int32_t &rThreadNum);
		
		public:
			void measure(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void read_memory_total(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			int getVariant();
	};
}

#include "CMeasureMIC.cpp"

#endif /* __CMEASUREMIC_HPP__ */
