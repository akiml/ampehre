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
 */

#ifndef __CMEASUREMAXELER_HPP__
#define __CMEASUREMAXELER_HPP__

#include "../common/CMeasureAbstractResource.hpp"

namespace NLibMeasure {
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
			void measurePower(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void measureTemperature(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void measureUtilization(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			
		public:
			void measure(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			const std::string& getPowerName(enum maxeler_power name) const;
			const std::string& getTemperatureName(enum maxeler_temperature name) const;
			void forceIdle(void);
	};
}

#endif /* __CMEASUREMAXELER_HPP__ */