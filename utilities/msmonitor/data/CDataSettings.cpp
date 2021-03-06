/*
 * CDataSettings.cpp
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
 *          0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#include "CDataSettings.hpp"

namespace NData {
	CDataSettings::CDataSettings(void) :
		CMutex(),
		
		mCPUSamplingRate(30),
		mGPUSamplingRate(50),
		mFPGASamplingRate(60),
		mMICSamplingRate(60),
		mSystemSamplingRate(100),
				
		mCPUCheckForExitInterrupts(1),
		mGPUCheckForExitInterrupts(1),
		mFPGACheckForExitInterrupts(1),
		mMICCheckForExitInterrupts(1),
		mSystemCheckForExitInterrupts(1),
		
		mSkipMsRate(SKIP_NEVER),
		
		mDataSamplingRate(30),
		mHeatmapSamplingRate(150),
		mGUIRefreshRate(500),
		
		mWriteResultsToCsv(false),
		
		mTimeToBufferData(180000),
		mTimeToShowData(60000),
		mNumberOfTicks(mTimeToBufferData/mDataSamplingRate),
		
		mImageSizeX(1024),
		mImageSizeY(786),
		
		mYAxisPowerMin(0),
		mYAxisPowerMax(450),
		mYAxisTempMin(0),
		mYAxisTempMax(90),
		mYAxisClockMin(0),
		mYAxisClockMax(2700),
		mYAxisUtilMin(0),
		mYAxisUtilMax(110),
		mYAxisMemoryMin(0),
		mYAxisMemoryMax(32768),
		
		mHeatmapTempCPUMinY(20),
		mHeatmapTempCPUMaxY(60),
		mHeatmapTempGPUMinY(30),
		mHeatmapTempGPUMaxY(70),
		mHeatmapTempFPGAMinY(30),
		mHeatmapTempFPGAMaxY(70),
		mHeatmapTempMICMinY(40),
		mHeatmapTempMICMaxY(140),
		mHeatmapTempSYSMinY(20),
		mHeatmapTempSYSMaxY(40),
		
		mCpuGovernor(CPU_GOVERNOR_ONDEMAND),
		mCpuFrequencyMinimum(0),
		mCpuFrequencyMaximum(0),
		mGpuFrequency(GPU_FREQUENCY_CUR),
		
		mLibVariant(VARIANT_FULL)
		{
		
		// nothing todo
	}
	
	CDataSettings::~CDataSettings(void) {
		// nothing todo
	}
}
