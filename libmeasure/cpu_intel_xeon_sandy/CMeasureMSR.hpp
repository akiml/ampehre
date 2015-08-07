/*
 * CMeasureMSR.hpp
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
 * created: 3/20/14
 * version: 0.1.0 - initial implementation
 *          0.1.3 - add CPU frequency measurements
 *          0.1.5 - add CPU performance counters
 *          0.1.7 - add CPU utilization measurements
 *          0.1.15 - make CPU frequency settable
 *          0.2.0 - add support for msr registers to the measure driver
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#ifndef __CMEASUREMSR_HPP__
#define __CMEASUREMSR_HPP__

#include "../common/CMeasureAbstractResource.hpp"
#include "../../include/ms_driver.h"

#define NUM_OF_CORES CPUS*CORES

namespace NLibMeasure {
	class CMeasureMSR : public CMeasureAbstractResource {
		private:
			int32_t mFildesMeasure;
			
			uint64_t mMinTccActivationTemp;
			double mEnergyUnit;
			double mEnergyMax;
			
			cpu_governor mCpuGovernor;
			uint64_t mCpuFrequencyMin;
			uint64_t mCpuFrequencyMax;
			
		public:
			CMeasureMSR(CLogger& rLogger, cpu_governor cpuGovernor, uint64_t cpuFrequencyMin, uint64_t cpuFrequencyMax);
			~CMeasureMSR();
			
		private:
			void init(void);
			void destroy(void);
			
			void showCurrentCpuFreqSettings(void);
			
			void openMeasureDevice(void);
			void closeMeasureDevice(void);
			void readMeasureDevice(int32_t& rThreadNum, int32_t dev, int32_t coreNumber, uint64_t address, uint64_t *pValues, int32_t size);
			void writeMeasureDevice(int32_t& rThreadNum, int32_t dev, int32_t coreNumber, uint64_t address, uint64_t valueToWrite);
			
			void resetMsrPerfCounters(void);
			double msrGetEnergyUnit(int32_t& rThreadNum);
			double msrGetEnergyMax(void);
			uint64_t msrGetMinumimTccActivation(int32_t& rThreadNum);
			uint32_t msrGetTemperature(int32_t& rThreadNum, int32_t coreNumber, int32_t msrRegisterAddr);
			double msrGetEnergy(int32_t& rThreadNum, int32_t coreNumber, int32_t msrRegisterAddr);
			uint64_t msrGetTimeStamp(int32_t& rThreadNum, int32_t coreNumber, int32_t msrRegisterAddr);
			void msrGetPerfCounter(int32_t& rThreadNum, int32_t coreNumber,
								   int32_t msrRegisterAddrAPerf, uint64_t* pResultAPerf,
								   int32_t msrRegisterAddrMPerf, uint64_t* pResultMPerf);
			
			void measureGetUtilization(int32_t& rThreadNum, uint64_t* pMeasureUtilization);
			
			void measureGetMemInfo(int32_t& rThreadNum, uint64_t* pMeasureMemInfo);
			
		public:
			void measure(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
	};
}

#endif /* __CMEASUREMSR_HPP__ */
