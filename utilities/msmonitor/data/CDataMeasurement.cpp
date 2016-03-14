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
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 */

#include "CDataMeasurement.hpp"

#include <iostream>

namespace NData {
	CDataMeasurement::CDataMeasurement(CDataSettings& rSettings) :
		mrSettings(rSettings),
		mNumberOfTicks(mrSettings.mNumberOfTicks)
		{
		
		allocRingBuffers();
		
		reset();
	}
	
	CDataMeasurement::~CDataMeasurement(void) {
		freeRingBuffers();
	}
	
	void CDataMeasurement::allocRingBuffers(void) {
		mpX				= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		
		mpYPowerCpu0	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYPowerCpu1	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYPowerGpu		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYPowerFpga	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYPowerMic		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYPowerSystem	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		
		mpYTempCpu0		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYTempCpu1		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYTempGpu		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYTempFpgaM	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYTempFpgaI	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYTempMicDie	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYTempSystem	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		
		mpYClockCpu0	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYClockCpu1	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYClockGpuCore	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYClockGpuMem	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYClockMicCore	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYClockMicMem	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		
		mpYUtilCpu		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYUtilGpuCore	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYUtilGpuMem	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYUtilFpga		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYUtilMic		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		
		mpYMemoryCpu	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYSwapCpu		= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYMemoryGpu	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
		mpYMemoryMic	= new NCommon::CRingBuffer<double>(mNumberOfTicks);
	}
	
	void CDataMeasurement::freeRingBuffers(void) {
		delete mpX;
		
		delete mpYPowerCpu0;
		delete mpYPowerCpu1;
		delete mpYPowerGpu;
		delete mpYPowerFpga;
		delete mpYPowerMic;
		delete mpYPowerSystem;
		
		delete mpYTempCpu0;
		delete mpYTempCpu1;
		delete mpYTempGpu;
		delete mpYTempFpgaM;
		delete mpYTempFpgaI;
		delete mpYTempMicDie;
		delete mpYTempSystem;
		
		delete mpYClockCpu0;
		delete mpYClockCpu1;
		delete mpYClockGpuCore;
		delete mpYClockGpuMem;
		delete mpYClockMicCore;
		delete mpYClockMicMem;
		
		delete mpYUtilCpu;
		delete mpYUtilGpuCore;
		delete mpYUtilGpuMem;
		delete mpYUtilFpga;
		delete mpYUtilMic;
		
		delete mpYMemoryCpu;
		delete mpYSwapCpu;
		delete mpYMemoryGpu;
		delete mpYMemoryMic;
	}
	
	void CDataMeasurement::reset(void) {
		mNumberOfTicks	= mrSettings.mNumberOfTicks;
		
		freeRingBuffers();
		allocRingBuffers();
		
		mpX->reset();
		
		mpYPowerCpu0->reset();
		mpYPowerCpu1->reset();
		mpYPowerGpu->reset();
		mpYPowerFpga->reset();
		mpYPowerMic->reset();
		mpYPowerSystem->reset();
		
		mpYTempCpu0->reset();
		mpYTempCpu1->reset();
		mpYTempGpu->reset();
		mpYTempFpgaM->reset();
		mpYTempFpgaI->reset();
		mpYTempMicDie->reset();
		mpYTempSystem->reset();
		
		mpYClockCpu0->reset();
		mpYClockCpu1->reset();
		mpYClockGpuCore->reset();
		mpYClockGpuMem->reset();
		mpYClockMicCore->reset();
		mpYClockMicMem->reset();
		
		mpYUtilCpu->reset();
		mpYUtilGpuCore->reset();
		mpYUtilGpuMem->reset();
		mpYUtilFpga->reset();
		mpYUtilMic->reset();
		
		mpYMemoryCpu->reset();
		mpYSwapCpu->reset();
		mpYMemoryGpu->reset();
		mpYMemoryMic->reset();
		
		mMaxelerActiveProcessPid = 0;
		mMaxelerActiveProcessName.clear();
		mMaxelerActiveProcessUser.clear();
			
		mNVMLActiveProcessesCount = 0;
		for (int i = 0; i < MAX_PROCESS_COUNT; i++) {
			mNVMLActiveProcessesPid[i] = 0;
			mNVMLActiveProcessesName[i].clear();
		}
	}
		
	double CDataMeasurement::getTime(void) {
		return mpX->getLast();
	}
}
