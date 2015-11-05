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
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
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
										  mrSettings.mGpuFrequency,
										  IOC_SET_AND_LOCK_IPMI_TIMEOUT);
		
		mpMSMeasurement			= ms_alloc_measurement();
		
		ms_set_timer(mpMSMeasurement, CPU   , mrSettings.mCPUSamplingRate   /1000,
					 (mrSettings.mCPUSamplingRate   %1000) * 1000000, mrSettings.mCPUSkipMsRate);
		ms_set_timer(mpMSMeasurement, GPU   , mrSettings.mGPUSamplingRate   /1000,
					 (mrSettings.mGPUSamplingRate   %1000) * 1000000, mrSettings.mGPUSkipMsRate);
		ms_set_timer(mpMSMeasurement, FPGA  , mrSettings.mFPGASamplingRate  /1000,
					 (mrSettings.mFPGASamplingRate  %1000) * 1000000, mrSettings.mFPGASkipMsRate);
		ms_set_timer(mpMSMeasurement, MIC , mrSettings.mMICSamplingRate     /1000,
					 (mrSettings.mMICSamplingRate   %1000) * 1000000, mrSettings.mMICSkipMsRate);
		ms_set_timer(mpMSMeasurement, SYSTEM, mrSettings.mSystemSamplingRate/1000,
					 (mrSettings.mSystemSamplingRate%1000) * 1000000, mrSettings.mSystemSkipMsRate);
		
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
		std::ofstream csv_file;
		double clockCpu0_temp;
		double clockCpu1_temp;
		
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		if(mrSettings.mWriteResultsToCsv) {
			char *home_directory = getenv("HOME");
			
			std::ostringstream csv_filename;
			csv_filename << home_directory;
			csv_filename << "/plot_";
			csv_filename << sNumberOfCSVs++;
			csv_filename << ".csv";
			
			csv_file.open(csv_filename.str().c_str());
			
			printHeader(csv_file);
		}
		
		mMutexTimer.lock();
		
		// initialize some values
		clock_gettime(CLOCK_REALTIME, &mTimeStart);
		mTimeCurrent.tv_sec		= mTimeStart.tv_sec;
		mTimeCurrent.tv_nsec	= mTimeStart.tv_nsec;
		mTimeDiff.tv_sec		= 0;
		mTimeDiff.tv_nsec		= 0;
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			// store current values
			mrMeasurement.mpX->push_back(calcTimeSinceStart());
			
			mrMeasurement.mpYPowerCpu0->push_back((mpMSMeasurement->msr_power_cur[0][PKG]
													+ mpMSMeasurement->msr_power_cur[0][DRAM])/1000.0);
			mrMeasurement.mpYPowerCpu1->push_back((mpMSMeasurement->msr_power_cur[1][PKG]
													+ mpMSMeasurement->msr_power_cur[1][DRAM])/1000.0);
			mrMeasurement.mpYPowerGpu->push_back(mpMSMeasurement->nvml_power_cur/1000.0);
			mrMeasurement.mpYPowerFpga->push_back(mpMSMeasurement->maxeler_power_cur[POWER]/1000.0);
			mrMeasurement.mpYPowerMic->push_back(mpMSMeasurement->mic_power_cur[MIC_POWER]/1000.0);
			mrMeasurement.mpYPowerSystem->push_back(mpMSMeasurement->ipmi_power_server_cur);
			
			mrMeasurement.mpYTempCpu0->push_back(mpMSMeasurement->msr_temperature_pkg_cur[0]);
			mrMeasurement.mpYTempCpu1->push_back(mpMSMeasurement->msr_temperature_pkg_cur[1]);
			mrMeasurement.mpYTempGpu->push_back(mpMSMeasurement->nvml_temperature_cur);
			mrMeasurement.mpYTempFpgaM->push_back(mpMSMeasurement->maxeler_temperature_cur[MTEMP]);
			mrMeasurement.mpYTempFpgaI->push_back(mpMSMeasurement->maxeler_temperature_cur[ITEMP]);
			mrMeasurement.mpYTempMicDie->push_back(mpMSMeasurement->mic_temperature_cur[MIC_DIE_TEMP]);
			mrMeasurement.mpYTempSystem->push_back(mpMSMeasurement->ipmi_temperature_sysboard_cur);
			
			clockCpu0_temp = 0.0;
			clockCpu1_temp = 0.0;
			for (i=0; i<CORES; ++i) {
				clockCpu0_temp	+= mpMSMeasurement->msr_freq_core_eff_cur[0][i];
				clockCpu1_temp	+= mpMSMeasurement->msr_freq_core_eff_cur[1][i];
			}
			mrMeasurement.mpYClockCpu0->push_back(clockCpu0_temp / CORES);		
			mrMeasurement.mpYClockCpu1->push_back(clockCpu1_temp / CORES);
			mrMeasurement.mpYClockGpuCore->push_back(mpMSMeasurement->nvml_clock_graphics_cur);
			mrMeasurement.mpYClockGpuMem->push_back(mpMSMeasurement->nvml_clock_mem_cur);
			mrMeasurement.mpYClockMicCore->push_back(mpMSMeasurement->mic_freq_core_cur);
			mrMeasurement.mpYClockMicMem->push_back(mpMSMeasurement->mic_freq_mem_cur);
			
			mrMeasurement.mpYUtilCpu->push_back(mpMSMeasurement->measure_util_avg_cur);
			mrMeasurement.mpYUtilGpuCore->push_back(mpMSMeasurement->nvml_util_gpu_cur);
			mrMeasurement.mpYUtilGpuMem->push_back(mpMSMeasurement->nvml_util_mem_cur);
			mrMeasurement.mpYUtilFpga->push_back(mpMSMeasurement->maxeler_util_comp_cur);
			mrMeasurement.mpYUtilMic->push_back(mpMSMeasurement->mic_util_avg_cur);
			
			mrMeasurement.mpYMemoryCpu->push_back(mpMSMeasurement->measure_memory_cur[CPU_MEM_RAM_USED]>>10);
			mrMeasurement.mpYSwapCpu->push_back(mpMSMeasurement->measure_memory_cur[CPU_MEM_SWAP_USED]>>10);
			mrMeasurement.mpYMemoryGpu->push_back(mpMSMeasurement->nvml_memory_used_cur>>10);
			mrMeasurement.mpYMemoryMic->push_back(mpMSMeasurement->mic_memory_used_cur>>10);
			
			if(mrSettings.mWriteResultsToCsv) {
				printValues(csv_file);
			}
		}
		
		if(mrSettings.mWriteResultsToCsv) {
			csv_file.close();
		}
		
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
		rCsvFile
		
		<< mrMeasurement.mpX->getLast() << ";"
		
		<< mrMeasurement.mpYPowerCpu0->getLast() << ";"
		<< mrMeasurement.mpYPowerCpu1->getLast() << ";"
		<< mrMeasurement.mpYPowerGpu->getLast() << ";"
		<< mrMeasurement.mpYPowerFpga->getLast() << ";"
		<< mrMeasurement.mpYPowerMic->getLast() << ";"
		<< mrMeasurement.mpYPowerSystem->getLast() << ";"
		
		<< mrMeasurement.mpYTempCpu0->getLast() << ";"
		<< mrMeasurement.mpYTempCpu1->getLast() << ";"
		<< mrMeasurement.mpYTempGpu->getLast() << ";"
		<< mrMeasurement.mpYTempFpgaM->getLast() << ";"
		<< mrMeasurement.mpYTempFpgaI->getLast() << ";"
		<< mrMeasurement.mpYTempMicDie->getLast() << ";"
		<< mrMeasurement.mpYTempSystem->getLast() << ";"
		
		<< mrMeasurement.mpYClockCpu0->getLast() << ";"
		<< mrMeasurement.mpYClockCpu1->getLast() << ";"
		<< mrMeasurement.mpYClockGpuCore->getLast() << ";"
		<< mrMeasurement.mpYClockGpuMem->getLast() << ";"
		<< mrMeasurement.mpYClockMicCore->getLast() << ";"
		<< mrMeasurement.mpYClockMicMem->getLast() << ";"
		
		<< mrMeasurement.mpYUtilCpu->getLast() << ";"
		<< mrMeasurement.mpYUtilGpuCore->getLast() << ";"
		<< mrMeasurement.mpYUtilGpuMem->getLast() << ";"
		<< mrMeasurement.mpYUtilFpga->getLast() << ";"
		<< mrMeasurement.mpYUtilMic->getLast() << ";"
		
		<< mrMeasurement.mpYMemoryCpu->getLast() << ";"
		<< mrMeasurement.mpYSwapCpu->getLast() << ";"
		<< mrMeasurement.mpYMemoryGpu->getLast() << ";"
		<< mrMeasurement.mpYMemoryMic->getLast() << ";"
		
		<< std::endl;
	}
}
