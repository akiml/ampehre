/*
 * CMeasureMaxeler.hpp
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
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 3/05/14
 * version: 0.1.0 - initial implementation
 *          0.1.1 - add functionality to force FPGA to idle
 *          0.1.9 - add FPGA utilization measurements
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.7.0 - modularised measurement struct
 */

#ifndef __CMEASUREMAXELER_HPP__
#define __CMEASUREMAXELER_HPP__

#define FORCE_IDLE 1
#define POWER_NAME 2
#define TEMP_NAME 3

#include "maxdsd.h"

#include "../../cjson/cJSON.h"
#include "../common/CMeasureAbstractResource.hpp"

namespace NLibMeasure {
	template <int TSkipMs, int TVariant>
	class CMeasureMaxeler : public CMeasureAbstractResource {
		private:
			int mMaxDaemonFildes;
			
			std::pair<std::string, double> mMaxPower[MAX_NUM_POWER];
			std::pair<std::string, double> mMaxTemperature[MAX_NUM_TEMPERATURE];
			
		public:
			CMeasureMaxeler(CLogger& rLogger);
			~CMeasureMaxeler();
			
		private:
			void init(void);
			void destroy(void);
			void measurePower(MS_MEASUREMENT_FPGA *pMsMeasurementFpga, int32_t& rThreadNum);
			void measureTemperature(MS_MEASUREMENT_FPGA *pMsMeasurementFpga, int32_t& rThreadNum);
			void measureUtilization(MS_MEASUREMENT_FPGA *pMsMeasurementFpga, int32_t& rThreadNum);
			
		public:
			void measure(void *pMsMeasurement, int32_t& rThreadNum);
			void trigger_resource_custom(void* pParams);
			int getVariant();
	};
}

#include "CMeasureMaxeler.cpp"

#endif /* __CMEASUREMAXELER_HPP__ */
