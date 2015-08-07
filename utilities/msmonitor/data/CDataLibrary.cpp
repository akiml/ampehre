/*
 * CDataLibrary.cpp
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
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.0 - add cpu, gpu and mic memory information
 */

#include "CDataLibrary.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace NData {
	uint32_t CDataLibrary::sNumberOfCSVs = 0;
	
	CDataLibrary::CDataLibrary(CDataSettings& rSettings, CDataMeasurement& rMeasurement) :
		CThread(),
		mThreadID(0),
		mThreadStateRun(false),
		mThreadStateStop(true),
		mrMeasurement(rMeasurement),
		mrSettings(rSettings),
		mMutexTimer(),
		mTimer(mrSettings.mDataSamplingRate, &mMutexTimer),
		mpMSVersion(0),
		mpMSSystem(0),
		mpMSMeasurement(0)
		{
		
		initMSSystem();
		startMSSystem();
	}
	
	CDataLibrary::~CDataLibrary(void) {
		stopMSSystem();
		freeMSSystem();
		
		delete mpMSVersion;
	}
	
	void CDataLibrary::initMSSystem(void) {
		mpMSVersion				= new MS_VERSION;
		mpMSVersion->major		= MS_MAJOR_VERSION;
		mpMSVersion->minor		= MS_MINOR_VERSION;
		mpMSVersion->revision	= MS_REVISION_VERSION;
		
		mpMSSystem				= ms_init(mpMSVersion,
										  mrSettings.mCpuGovernor,
										  mrSettings.mCpuFrequencyMaximum,
										  mrSettings.mCpuFrequencyMaximum,
										  mrSettings.mGpuFrequency);
		
		mpMSMeasurement			= ms_alloc_measurement();
		
		ms_set_timer(mpMSMeasurement, CPU   , mrSettings.mCPUSamplingRate   /1000,
					 (mrSettings.mCPUSamplingRate   %1000) * 1000000);
		ms_set_timer(mpMSMeasurement, GPU   , mrSettings.mGPUSamplingRate   /1000,
					 (mrSettings.mGPUSamplingRate   %1000) * 1000000);
		ms_set_timer(mpMSMeasurement, FPGA  , mrSettings.mFPGASamplingRate  /1000,
					 (mrSettings.mFPGASamplingRate  %1000) * 1000000);
		ms_set_timer(mpMSMeasurement, MIC , mrSettings.mMICSamplingRate     /1000,
					 (mrSettings.mMICSamplingRate   %1000) * 1000000);
		ms_set_timer(mpMSMeasurement, SYSTEM, mrSettings.mSystemSamplingRate/1000,
					 (mrSettings.mSystemSamplingRate%1000) * 1000000);
		
		ms_init_measurement(mpMSSystem, mpMSMeasurement, ALL);
	}
	
	void CDataLibrary::freeMSSystem(void) {
		ms_free_measurement(mpMSMeasurement);
		ms_fini(mpMSSystem);
	}
	
	void CDataLibrary::startMSSystem(void) {
		ms_start_measurement(mpMSSystem, mpMSMeasurement);
	}
	
	void CDataLibrary::stopMSSystem(void) {
		ms_stop_measurement(mpMSSystem, mpMSMeasurement);
		
		ms_join_measurement(mpMSSystem, mpMSMeasurement);
		ms_fini_measurement(mpMSSystem, mpMSMeasurement);
	}
	
	void CDataLibrary::start(void) {
		pthread_create(&mThreadID, NULL, &startThread, (void *)this);
		mTimer.start();
	}
	
	void *CDataLibrary::startThread(void *pThreadObject) {
		CDataLibrary* thread = static_cast<CDataLibrary*>(pThreadObject);
		
		thread->run();
		
		return 0;
	}
	
	void CDataLibrary::stop(void) {
		mTimer.stop();
		mThreadStateStop = true;
		mMutexTimer.unlock();
	}
	
	void CDataLibrary::exit(void) {
		mThreadStateRun = false;
		pthread_exit(NULL);
	}
	
	void CDataLibrary::join(void) {
		mTimer.join();
		pthread_join(mThreadID, NULL);
	}
	
	void CDataLibrary::run(void) {
		int i;
		
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		char *home_directory = getenv("HOME");
		
		std::ostringstream csv_filename;
		csv_filename << home_directory;
		csv_filename << "/plot_";
		csv_filename << sNumberOfCSVs++;
		csv_filename << ".csv";
		
		std::ofstream csv_file(csv_filename.str().c_str());
		
		printHeader(csv_file);
		
		mMutexTimer.lock();
		
		// initialize some values
		uint32_t tick_first		= 0;
		uint32_t tick_second	= mrSettings.mNumberOfTicks;
		
		clock_gettime(CLOCK_REALTIME, &mTimeStart);
		mTimeCurrent.tv_sec		= mTimeStart.tv_sec;
		mTimeCurrent.tv_nsec	= mTimeStart.tv_nsec;
		mTimeDiff.tv_sec		= 0;
		mTimeDiff.tv_nsec		= 0;
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			// get next graph tick
			tick_first	= mrMeasurement.getTick();
			tick_second	= mrSettings.mNumberOfTicks + tick_first;
			
			// store current values
			mrMeasurement.mpX[tick_first]				= calcTimeSinceStart();
			
			mrMeasurement.mpYPowerCpu0[tick_first]		= (mpMSMeasurement->msr_power_cur[0][PKG]
														  + mpMSMeasurement->msr_power_cur[0][DRAM])/1000.0;
			mrMeasurement.mpYPowerCpu1[tick_first]		= (mpMSMeasurement->msr_power_cur[1][PKG]
														  + mpMSMeasurement->msr_power_cur[1][DRAM])/1000.0;
			mrMeasurement.mpYPowerGpu[tick_first]		= mpMSMeasurement->nvml_power_cur/1000.0;
			mrMeasurement.mpYPowerFpga[tick_first]		= mpMSMeasurement->maxeler_power_cur[POWER]/1000.0;
			mrMeasurement.mpYPowerMic[tick_first]		= mpMSMeasurement->mic_power_cur[MIC_POWER]/1000.0;
			mrMeasurement.mpYPowerSystem[tick_first]	= mpMSMeasurement->ipmi_power_server_cur;
			
			mrMeasurement.mpYTempCpu0[tick_first]		= mpMSMeasurement->msr_temperature_pkg_cur[0];
			mrMeasurement.mpYTempCpu1[tick_first]		= mpMSMeasurement->msr_temperature_pkg_cur[1];
			mrMeasurement.mpYTempGpu[tick_first]		= mpMSMeasurement->nvml_temperature_cur;
			mrMeasurement.mpYTempFpgaM[tick_first]		= mpMSMeasurement->maxeler_temperature_cur[MTEMP];
			mrMeasurement.mpYTempFpgaI[tick_first]		= mpMSMeasurement->maxeler_temperature_cur[ITEMP];
			mrMeasurement.mpYTempMicDie[tick_first]		= mpMSMeasurement->mic_temperature_cur[MIC_DIE_TEMP];
			mrMeasurement.mpYTempSystem[tick_first]		= mpMSMeasurement->ipmi_temperature_sysboard_cur;
			
			mrMeasurement.mpYClockCpu0[tick_first]		= 0.0;
			mrMeasurement.mpYClockCpu1[tick_first]		= 0.0;
			for (i=0; i<CORES; ++i) {
				mrMeasurement.mpYClockCpu0[tick_first]	+= mpMSMeasurement->msr_freq_core_eff_cur[0][i];
				mrMeasurement.mpYClockCpu1[tick_first]	+= mpMSMeasurement->msr_freq_core_eff_cur[1][i];
			}
			mrMeasurement.mpYClockCpu0[tick_first]		/= CORES;
			mrMeasurement.mpYClockCpu1[tick_first]		/= CORES;
			mrMeasurement.mpYClockGpuCore[tick_first]	= mpMSMeasurement->nvml_clock_graphics_cur;
			mrMeasurement.mpYClockGpuMem[tick_first]	= mpMSMeasurement->nvml_clock_mem_cur;
			mrMeasurement.mpYClockMicCore[tick_first]	= mpMSMeasurement->mic_freq_core_cur;
			mrMeasurement.mpYClockMicMem[tick_first]	= mpMSMeasurement->mic_freq_mem_cur;
			
			mrMeasurement.mpYUtilCpu[tick_first]		= mpMSMeasurement->measure_util_avg_cur;
			mrMeasurement.mpYUtilGpuCore[tick_first]	= mpMSMeasurement->nvml_util_gpu_cur;
			mrMeasurement.mpYUtilGpuMem[tick_first]		= mpMSMeasurement->nvml_util_mem_cur;
			mrMeasurement.mpYUtilFpga[tick_first]		= mpMSMeasurement->maxeler_util_comp_cur;
			mrMeasurement.mpYUtilMic[tick_first]		= mpMSMeasurement->mic_util_avg_cur;
			
			mrMeasurement.mpYMemoryCpu[tick_first]		= mpMSMeasurement->measure_memory_cur[CPU_MEM_RAM_USED]>>10;
			mrMeasurement.mpYSwapCpu[tick_first]		= mpMSMeasurement->measure_memory_cur[CPU_MEM_SWAP_USED]>>10;
			mrMeasurement.mpYMemoryGpu[tick_first]		= mpMSMeasurement->nvml_memory_used_cur>>10;
			mrMeasurement.mpYMemoryMic[tick_first]		= mpMSMeasurement->mic_memory_used_cur>>10;
			
			// store it a second time for plotting
			mrMeasurement.mpX[tick_second]				= mrMeasurement.mpX[tick_first];
			
			mrMeasurement.mpYPowerCpu0[tick_second]		= mrMeasurement.mpYPowerCpu0[tick_first];
			mrMeasurement.mpYPowerCpu1[tick_second]		= mrMeasurement.mpYPowerCpu1[tick_first];
			mrMeasurement.mpYPowerGpu[tick_second]		= mrMeasurement.mpYPowerGpu[tick_first];
			mrMeasurement.mpYPowerFpga[tick_second]		= mrMeasurement.mpYPowerFpga[tick_first];
			mrMeasurement.mpYPowerMic[tick_second]		= mrMeasurement.mpYPowerMic[tick_first];
			mrMeasurement.mpYPowerSystem[tick_second]	= mrMeasurement.mpYPowerSystem[tick_first];
			
			mrMeasurement.mpYTempCpu0[tick_second]		= mrMeasurement.mpYTempCpu0[tick_first];
			mrMeasurement.mpYTempCpu1[tick_second]		= mrMeasurement.mpYTempCpu1[tick_first];
			mrMeasurement.mpYTempGpu[tick_second]		= mrMeasurement.mpYTempGpu[tick_first];
			mrMeasurement.mpYTempFpgaM[tick_second]		= mrMeasurement.mpYTempFpgaM[tick_first];
			mrMeasurement.mpYTempFpgaI[tick_second]		= mrMeasurement.mpYTempFpgaI[tick_first];
			mrMeasurement.mpYTempMicDie[tick_second]	= mrMeasurement.mpYTempMicDie[tick_first];
			mrMeasurement.mpYTempSystem[tick_second]	= mrMeasurement.mpYTempSystem[tick_first];
			
			mrMeasurement.mpYClockCpu0[tick_second]		= mrMeasurement.mpYClockCpu0[tick_first];
			mrMeasurement.mpYClockCpu1[tick_second]		= mrMeasurement.mpYClockCpu1[tick_first];
			mrMeasurement.mpYClockGpuCore[tick_second]	= mrMeasurement.mpYClockGpuCore[tick_first];
			mrMeasurement.mpYClockGpuMem[tick_second]	= mrMeasurement.mpYClockGpuMem[tick_first];
			mrMeasurement.mpYClockMicCore[tick_second]	= mrMeasurement.mpYClockMicCore[tick_first];
			mrMeasurement.mpYClockMicMem[tick_second]	= mrMeasurement.mpYClockMicMem[tick_first];
			
			mrMeasurement.mpYUtilCpu[tick_second]		= mrMeasurement.mpYUtilCpu[tick_first];
			mrMeasurement.mpYUtilGpuCore[tick_second]	= mrMeasurement.mpYUtilGpuCore[tick_first];
			mrMeasurement.mpYUtilGpuMem[tick_second]	= mrMeasurement.mpYUtilGpuMem[tick_first];
			mrMeasurement.mpYUtilFpga[tick_second]		= mrMeasurement.mpYUtilFpga[tick_first];
			mrMeasurement.mpYUtilMic[tick_second]		= mrMeasurement.mpYUtilMic[tick_first];
			
			mrMeasurement.mpYMemoryCpu[tick_second]		= mrMeasurement.mpYMemoryCpu[tick_first];
			mrMeasurement.mpYSwapCpu[tick_second]		= mrMeasurement.mpYSwapCpu[tick_first];
			mrMeasurement.mpYMemoryGpu[tick_second]		= mrMeasurement.mpYMemoryGpu[tick_first];
			mrMeasurement.mpYMemoryMic[tick_second]		= mrMeasurement.mpYMemoryMic[tick_first];
			
			printValues(csv_file);
			
			// increment graph tick
			mrMeasurement.incTick();
		}
		
		csv_file.close();
		
		exit();
	}
	
	double CDataLibrary::calcTimeSinceStart(void) {
		double time_diff = 0.0;
		
		clock_gettime(CLOCK_REALTIME, &mTimeCurrent);
		if ((mTimeCurrent.tv_nsec - mTimeStart.tv_nsec)<0) {
			mTimeDiff.tv_sec	= mTimeCurrent.tv_sec - mTimeStart.tv_sec - 1;
			mTimeDiff.tv_nsec	= 1000000000 + mTimeCurrent.tv_nsec - mTimeStart.tv_nsec;
		} else {
			mTimeDiff.tv_sec	= mTimeCurrent.tv_sec - mTimeStart.tv_sec;
			mTimeDiff.tv_nsec	= mTimeCurrent.tv_nsec - mTimeStart.tv_nsec;
		}
		
		time_diff	= (double)mTimeDiff.tv_sec + ((double)mTimeDiff.tv_nsec / 1000000000.0);
		
		return time_diff;
	}
	
	void CDataLibrary::printHeader(std::ofstream &rCsvFile) {
		rCsvFile
		
		<< "X" << ";"
		
		<< "PowerCpu0" << ";"
		<< "PowerCpu1" << ";"
		<< "PowerGpu" << ";"
		<< "PowerFpga" << ";"
		<< "PowerMic" << ";"
		<< "PowerSystem" << ";"
		
		<< "TempCpu0" << ";"
		<< "TempCpu1" << ";"
		<< "TempGpu" << ";"
		<< "TempFpgaM" << ";"
		<< "TempFpgaI" << ";"
		<< "TempMic" << ";"
		<< "TempSystem" << ";"
		
		<< "ClockCpu0" << ";"
		<< "ClockCpu1" << ";"
		<< "ClockGpuCore" << ";"
		<< "ClockGpuMem" << ";"
		<< "ClockMicCore" << ";"
		<< "ClockMicMem" << ";"
		
		<< "UtilCpu" << ";"
		<< "UtilGpuCore" << ";"
		<< "UtilGpuMem" << ";"
		<< "UtilFpga" << ";"
		<< "UtilMic" << ";"
		
		<< "MemoryCpu" << ";"
		<< "SwapCpu" << ";"
		<< "MemoryGpu" << ";"
		<< "MemoryMic" << ";"
		
		<< std::endl;
	}
	
	void CDataLibrary::printValues(std::ofstream &rCsvFile) {
		uint32_t i = mrMeasurement.getTick();
		
		rCsvFile
		
		<< mrMeasurement.mpX[i] << ";"
		
		<< mrMeasurement.mpYPowerCpu0[i] << ";"
		<< mrMeasurement.mpYPowerCpu1[i] << ";"
		<< mrMeasurement.mpYPowerGpu[i] << ";"
		<< mrMeasurement.mpYPowerFpga[i] << ";"
		<< mrMeasurement.mpYPowerMic[i] << ";"
		<< mrMeasurement.mpYPowerSystem[i] << ";"
		
		<< mrMeasurement.mpYTempCpu0[i] << ";"
		<< mrMeasurement.mpYTempCpu1[i] << ";"
		<< mrMeasurement.mpYTempGpu[i] << ";"
		<< mrMeasurement.mpYTempFpgaM[i] << ";"
		<< mrMeasurement.mpYTempFpgaI[i] << ";"
		<< mrMeasurement.mpYTempMicDie[i] << ";"
		<< mrMeasurement.mpYTempSystem[i] << ";"
		
		<< mrMeasurement.mpYClockCpu0[i] << ";"
		<< mrMeasurement.mpYClockCpu1[i] << ";"
		<< mrMeasurement.mpYClockGpuCore[i] << ";"
		<< mrMeasurement.mpYClockGpuMem[i] << ";"
		<< mrMeasurement.mpYClockMicCore[i] << ";"
		<< mrMeasurement.mpYClockMicMem[i] << ";"
		
		<< mrMeasurement.mpYUtilCpu[i] << ";"
		<< mrMeasurement.mpYUtilGpuCore[i] << ";"
		<< mrMeasurement.mpYUtilGpuMem[i] << ";"
		<< mrMeasurement.mpYUtilFpga[i] << ";"
		<< mrMeasurement.mpYUtilMic[i] << ";"
		
		<< mrMeasurement.mpYMemoryCpu[i] << ";"
		<< mrMeasurement.mpYSwapCpu[i] << ";"
		<< mrMeasurement.mpYMemoryGpu[i] << ";"
		<< mrMeasurement.mpYMemoryMic[i] << ";"
		
		<< std::endl;
	}
}
