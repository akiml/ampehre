/*
 * CDataMeasurement.cpp
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
 * created: 1/27/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.0 - add cpu, gpu and mic memory information
 */

#include "CDataMeasurement.hpp"

#include <iostream>

namespace NData {
	CDataMeasurement::CDataMeasurement(CDataSettings& rSettings) :
		mrSettings(rSettings),
		mNumberOfTicks(mrSettings.mNumberOfTicks),
		mTick(0)
		{
		
		allocArrays();
		
		reset();
	}
	
	CDataMeasurement::~CDataMeasurement(void) {
		freeArrays();
	}
	
	void CDataMeasurement::allocArrays(void) {
		mpX				= new double[mNumberOfTicks*2];
		
		mpYPowerCpu0	= new double[mNumberOfTicks*2];
		mpYPowerCpu1	= new double[mNumberOfTicks*2];
		mpYPowerGpu		= new double[mNumberOfTicks*2];
		mpYPowerFpga	= new double[mNumberOfTicks*2];
		mpYPowerMic		= new double[mNumberOfTicks*2];
		mpYPowerSystem	= new double[mNumberOfTicks*2];
		
		mpYTempCpu0		= new double[mNumberOfTicks*2];
		mpYTempCpu1		= new double[mNumberOfTicks*2];
		mpYTempGpu		= new double[mNumberOfTicks*2];
		mpYTempFpgaM	= new double[mNumberOfTicks*2];
		mpYTempFpgaI	= new double[mNumberOfTicks*2];
		mpYTempMicDie	= new double[mNumberOfTicks*2];
		mpYTempSystem	= new double[mNumberOfTicks*2];
		
		mpYClockCpu0	= new double[mNumberOfTicks*2];
		mpYClockCpu1	= new double[mNumberOfTicks*2];
		mpYClockGpuCore	= new double[mNumberOfTicks*2];
		mpYClockGpuMem	= new double[mNumberOfTicks*2];
		mpYClockMicCore	= new double[mNumberOfTicks*2];
		mpYClockMicMem	= new double[mNumberOfTicks*2];
		
		mpYUtilCpu		= new double[mNumberOfTicks*2];
		mpYUtilGpuCore	= new double[mNumberOfTicks*2];
		mpYUtilGpuMem	= new double[mNumberOfTicks*2];
		mpYUtilFpga		= new double[mNumberOfTicks*2];
		mpYUtilMic		= new double[mNumberOfTicks*2];
		
		mpYMemoryCpu	= new double[mNumberOfTicks*2];
		mpYSwapCpu		= new double[mNumberOfTicks*2];
		mpYMemoryGpu	= new double[mNumberOfTicks*2];
		mpYMemoryMic	= new double[mNumberOfTicks*2];
	}
	
	void CDataMeasurement::freeArrays(void) {
		delete[] mpX;
		
		delete[] mpYPowerCpu0;
		delete[] mpYPowerCpu1;
		delete[] mpYPowerGpu;
		delete[] mpYPowerFpga;
		delete[] mpYPowerMic;
		delete[] mpYPowerSystem;
		
		delete[] mpYTempCpu0;
		delete[] mpYTempCpu1;
		delete[] mpYTempGpu;
		delete[] mpYTempFpgaM;
		delete[] mpYTempFpgaI;
		delete[] mpYTempMicDie;
		delete[] mpYTempSystem;
		
		delete[] mpYClockCpu0;
		delete[] mpYClockCpu1;
		delete[] mpYClockGpuCore;
		delete[] mpYClockGpuMem;
		delete[] mpYClockMicCore;
		delete[] mpYClockMicMem;
		
		delete[] mpYUtilCpu;
		delete[] mpYUtilGpuCore;
		delete[] mpYUtilGpuMem;
		delete[] mpYUtilFpga;
		delete[] mpYUtilMic;
		
		delete[] mpYMemoryCpu;
		delete[] mpYSwapCpu;
		delete[] mpYMemoryGpu;
		delete[] mpYMemoryMic;
	}
	
	void CDataMeasurement::reset(void) {
		mTick			= 0;
		mNumberOfTicks	= mrSettings.mNumberOfTicks;
		
		freeArrays();
		allocArrays();
		
		uint32_t i;
		for (i=0; i<mNumberOfTicks*2; ++i) {
			mpX[i]				= 0.0;
			
			mpYPowerCpu0[i]		= 0.0;
			mpYPowerCpu1[i]		= 0.0;
			mpYPowerGpu[i]		= 0.0;
			mpYPowerFpga[i]		= 0.0;
			mpYPowerMic[i]		= 0.0;
			mpYPowerSystem[i]	= 0.0;
			
			mpYTempCpu0[i]		= 0.0;
			mpYTempCpu1[i]		= 0.0;
			mpYTempGpu[i]		= 0.0;
			mpYTempFpgaM[i]		= 0.0;
			mpYTempFpgaI[i]		= 0.0;
			mpYTempMicDie[i]	= 0.0;
			mpYTempSystem[i]	= 0.0;
			
			mpYClockCpu0[i]		= 0.0;
			mpYClockCpu1[i]		= 0.0;
			mpYClockGpuCore[i]	= 0.0;
			mpYClockGpuMem[i]	= 0.0;
			mpYClockMicCore[i]	= 0.0;
			mpYClockMicMem[i]	= 0.0;
			
			mpYUtilCpu[i]		= 0.0;
			mpYUtilGpuCore[i]	= 0.0;
			mpYUtilGpuMem[i]	= 0.0;
			mpYUtilFpga[i]		= 0.0;
			mpYUtilMic[i]		= 0.0;
			
			mpYMemoryCpu[i]		= 0.0;
			mpYSwapCpu[i]		= 0.0;
			mpYMemoryGpu[i]		= 0.0;
			mpYMemoryMic[i]		= 0.0;
		}
	}
	
	uint32_t CDataMeasurement::getTick(void) {
		return mTick;
	}

	void CDataMeasurement::incTick(void) {
		mTick = (mTick+1 == mNumberOfTicks) ? 0 : mTick+1;
	}
	
	double CDataMeasurement::getTime(void) {
		if (mTick == 0) {
			return mpX[mNumberOfTicks-1];
		}
		
		return mpX[mTick-1];
	}
}
