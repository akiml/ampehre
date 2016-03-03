/*
 * CMeasureOdroid.hpp
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 2/24/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#ifndef __CMEASUREODROID_HPP__
#define __CMEASUREODROID_HPP__

#include "../common/CMeasureAbstractResource.hpp"

#include "odroid.h"

namespace NLibMeasure {
	class CMeasureOdroid : public CMeasureAbstractResource {
		private:
			ODROID_SENSOR *mpSensorDevFSARM;
			ODROID_SENSOR *mpSensorDevFSMEM;
			ODROID_SENSOR *mpSensorDevFSG3D;
			ODROID_SENSOR *mpSensorDevFSKFC;
			
			FILE *mpSensorSysFSTemp;
			
			FILE *mpSensorSysFSFreqA7;
			FILE *mpSensorSysFSFreqA15;
			FILE *mpSensorSysFSFreqMali;
			
			FILE *mpFileSysFSUtilMali;
			FILE *mpFileProcFSUtilARM;
			
		public:
			CMeasureOdroid(CLogger& rLogger);
			~CMeasureOdroid();
			
		private:
			void init(void);
			void destroy(void);
			
			ODROID_SENSOR *initSensorPower(const char* pSensorDevFSFileName);
			void destroySensorPower(ODROID_SENSOR *pSensorDevFSStruct);
			double readSensorPower(ODROID_SENSOR *pSensorDevFSStruct);
			
			FILE *initSensorTemp(const char *pSensorSysFSFileName);
			void destroySensorTemp(FILE *pSensorSysFSFile);
			uint32_t readSensorTemp(FILE *pSensorSysFSFile, odroid_temp sensorTempType, int32_t& rThreadNum);
			
			FILE *initSensorFreq(const char *pSensorSysFSFileName);
			void destroySensorFreq(FILE *pSensorSysFSFile);
			uint32_t readSensorFreq(FILE *pSensorSysFSFile, odroid_freq sensorFreqType, int32_t& rThreadNum);
			
			FILE *initFileUtilizationProcFS(const char *pFileProcFSFileName);
			FILE *initFileUtilizationSysFS(const char *pFileSysFSFileName);
			void destroyFileUtilizationProcFS(FILE *pFileProcFSFileName);
			void destroyFileUtilizationSysFS(FILE *pFileSysFSFileName);
			void readFileUtilizationProcFS(FILE *pFileProcFSFileName, odroid_util deviceUtilType, int32_t& rThreadNum, uint64_t *active, uint64_t *inactive);
			double readFileUtilizationSysFS(FILE *pFileSysFSFileName, odroid_util deviceUtilType, int32_t& rThreadNum);
			
			void measurePower(MS_MEASUREMENT_ODROID *pMsMeasurementOdroid, int32_t& rThreadNum);
			void measureTemperature(MS_MEASUREMENT_ODROID *pMsMeasurementOdroid, int32_t& rThreadNum);
			void measureUtilization(MS_MEASUREMENT_ODROID *pMsMeasurementOdroid, int32_t& rThreadNum);
			void measureFrequency(MS_MEASUREMENT_ODROID *pMsMeasurementOdroid, int32_t& rThreadNum);
			
		public:
			void measure(void *pMsMeasurement, int32_t& rThreadNum);
			int getVariant();
	};
}

#include "CMeasureOdroid.cpp"

#endif /* __CMEASUREODROID_HPP__ */
