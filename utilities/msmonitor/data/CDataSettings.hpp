/*
 * CDataSettings.hpp
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
 * created: 1/26/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.7.0 - modularized measurement struct
 *          0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#ifndef __CDATASETTINGS_HPP__
#define __CDATASETTINGS_HPP__

#include "../common/CMutex.hpp"

#include "../../../include/ms_measurement.h"

// TODO: This is a non-C++ hack!
#include <stdint.h>

namespace NData {
	class CDataSettings : public NCommon::CMutex {
		public:
			uint32_t mCPUSamplingRate;
			uint32_t mGPUSamplingRate;
			uint32_t mFPGASamplingRate;
			uint32_t mMICSamplingRate;
			uint32_t mSystemSamplingRate;
			
			uint32_t mCPUCheckForExitInterrupts;
			uint32_t mGPUCheckForExitInterrupts;
			uint32_t mFPGACheckForExitInterrupts;
			uint32_t mMICCheckForExitInterrupts;
			uint32_t mSystemCheckForExitInterrupts;
			
			skip_ms_rate mSkipMsRate;
			
			uint32_t mDataSamplingRate;
			uint32_t mHeatmapSamplingRate;
			uint32_t mGUIRefreshRate;
			
			bool mWriteResultsToCsv;
			
			uint32_t mTimeToBufferData;
			uint32_t mTimeToShowData;
			uint32_t mNumberOfTicks;
			
			uint32_t mImageSizeX;
			uint32_t mImageSizeY;
			
			uint32_t mYAxisPowerMin;
			uint32_t mYAxisPowerMax;
			uint32_t mYAxisTempMin;
			uint32_t mYAxisTempMax;
			uint32_t mYAxisClockMin;
			uint32_t mYAxisClockMax;
			uint32_t mYAxisUtilMin;
			uint32_t mYAxisUtilMax;
			uint32_t mYAxisMemoryMin;
			uint32_t mYAxisMemoryMax;
			
			double mHeatmapTempCPUMinY;
			double mHeatmapTempCPUMaxY;
			double mHeatmapTempGPUMinY;
			double mHeatmapTempGPUMaxY;
			double mHeatmapTempFPGAMinY;
			double mHeatmapTempFPGAMaxY;
			double mHeatmapTempMICMinY;
			double mHeatmapTempMICMaxY;
			double mHeatmapTempSYSMinY;
			double mHeatmapTempSYSMaxY;
			
			enum cpu_governor mCpuGovernor;
			uint64_t mCpuFrequencyMinimum;
			uint64_t mCpuFrequencyMaximum;
			enum gpu_frequency mGpuFrequency;
			
			lib_variant mLibVariant;
			
		public:
			CDataSettings(void);
			~CDataSettings(void);
	};
}

#endif /* __CDATASETTINGS_HPP__ */
