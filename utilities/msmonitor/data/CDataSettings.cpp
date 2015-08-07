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
		
		mDataSamplingRate(30),
		mGUIRefreshRate(250),
		
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
		
		mCpuGovernor(CPU_GOVERNOR_ONDEMAND),
		mCpuFrequencyMinimum(0),
		mCpuFrequencyMaximum(0),
		mGpuFrequency(GPU_FREQUENCY_CUR)
		{
		
		// nothing todo
	}
	
	CDataSettings::~CDataSettings(void) {
		// nothing todo
	}
}
