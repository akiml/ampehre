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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and show them in msmonitor
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
										  IPMI_SET_AND_LOCK_TIMEOUT,
										  mrSettings.mSkipMsRate,
										  mrSettings.mLibVariant
 										);
		
		mpMSMeasurement			= ms_alloc_measurement(mpMSSystem);
		
		ms_set_timer(mpMSMeasurement, CPU   , mrSettings.mCPUSamplingRate   /1000,
					 (mrSettings.mCPUSamplingRate   %1000) * 1000000, mrSettings.mCPUCheckForExitInterrupts);
		ms_set_timer(mpMSMeasurement, GPU   , mrSettings.mGPUSamplingRate   /1000,
					 (mrSettings.mGPUSamplingRate   %1000) * 1000000, mrSettings.mGPUCheckForExitInterrupts);
		ms_set_timer(mpMSMeasurement, FPGA  , mrSettings.mFPGASamplingRate  /1000,
					 (mrSettings.mFPGASamplingRate  %1000) * 1000000, mrSettings.mFPGACheckForExitInterrupts);
		ms_set_timer(mpMSMeasurement, MIC , mrSettings.mMICSamplingRate     /1000,
					 (mrSettings.mMICSamplingRate   %1000) * 1000000, mrSettings.mMICCheckForExitInterrupts);
		ms_set_timer(mpMSMeasurement, SYSTEM, mrSettings.mSystemSamplingRate/1000,
					 (mrSettings.mSystemSamplingRate%1000) * 1000000, mrSettings.mSystemCheckForExitInterrupts);
		
		ms_init_measurement(mpMSSystem, mpMSMeasurement, ALL);
	}
	
	void CDataLibrary::freeMSSystem(void) {
		ms_free_measurement(mpMSMeasurement);
		ms_fini(mpMSSystem);
	}
	
	void CDataLibrary::startMSSystem(void) {
		ms_start_measurement(mpMSSystem);
	}
	
	void CDataLibrary::stopMSSystem(void) {
		ms_stop_measurement(mpMSSystem);
		
		ms_join_measurement(mpMSSystem);
		ms_fini_measurement(mpMSSystem);
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
		
		MS_MEASUREMENT_CPU *pMsMeasurementCpu = NULL;
		MS_MEASUREMENT_GPU *pMsMeasurementGpu = NULL;
		MS_MEASUREMENT_FPGA *pMsMeasurementFpga = NULL;
		MS_MEASUREMENT_MIC *pMsMeasurementMic = NULL;
		MS_MEASUREMENT_SYS *pMsMeasurementSys = NULL;
		
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		//get available resource specific measurement structs
		getMs_Measurements(&pMsMeasurementCpu, &pMsMeasurementGpu, &pMsMeasurementFpga, &pMsMeasurementMic, &pMsMeasurementSys);
		
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
			
			mrMeasurement.mpYPowerCpu0->push_back((pMsMeasurementCpu->msr_power_cur[0][PKG]
													+ pMsMeasurementCpu->msr_power_cur[0][DRAM])/1000.0);
			mrMeasurement.mpYPowerCpu1->push_back((pMsMeasurementCpu->msr_power_cur[1][PKG]
													+ pMsMeasurementCpu->msr_power_cur[1][DRAM])/1000.0);
			mrMeasurement.mpYPowerGpu->push_back(pMsMeasurementGpu->nvml_power_cur/1000.0);
			mrMeasurement.mpYPowerFpga->push_back(pMsMeasurementFpga->maxeler_power_cur[POWER]/1000.0);
			mrMeasurement.mpYPowerMic->push_back(pMsMeasurementMic->mic_power_cur[MIC_POWER]/1000.0);
			mrMeasurement.mpYPowerSystem->push_back(pMsMeasurementSys->ipmi_power_server_cur);
			
			mrMeasurement.mpYTempCpu0->push_back(pMsMeasurementCpu->msr_temperature_pkg_cur[0]);
			mrMeasurement.mpYTempCpu1->push_back(pMsMeasurementCpu->msr_temperature_pkg_cur[1]);
			mrMeasurement.mpYTempGpu->push_back(pMsMeasurementGpu->nvml_temperature_cur);
			mrMeasurement.mpYTempFpgaM->push_back(pMsMeasurementFpga->maxeler_temperature_cur[MTEMP]);
			mrMeasurement.mpYTempFpgaI->push_back(pMsMeasurementFpga->maxeler_temperature_cur[ITEMP]);
			mrMeasurement.mpYTempMicDie->push_back(pMsMeasurementMic->mic_temperature_cur[MIC_DIE_TEMP]);
			mrMeasurement.mpYTempSystem->push_back(pMsMeasurementSys->ipmi_temperature_sysboard_cur);
			
			clockCpu0_temp = 0.0;
			clockCpu1_temp = 0.0;
			for (i=0; i<CORES; ++i) {
				clockCpu0_temp	+= pMsMeasurementCpu->msr_freq_core_eff_cur[0][i];
				clockCpu1_temp	+= pMsMeasurementCpu->msr_freq_core_eff_cur[1][i];
			}
			mrMeasurement.mpYClockCpu0->push_back(clockCpu0_temp / CORES);		
			mrMeasurement.mpYClockCpu1->push_back(clockCpu1_temp / CORES);
			mrMeasurement.mpYClockGpuCore->push_back(pMsMeasurementGpu->nvml_clock_graphics_cur);
			mrMeasurement.mpYClockGpuMem->push_back(pMsMeasurementGpu->nvml_clock_mem_cur);
			mrMeasurement.mpYClockMicCore->push_back(pMsMeasurementMic->mic_freq_core_cur);
			mrMeasurement.mpYClockMicMem->push_back(pMsMeasurementMic->mic_freq_mem_cur);
			
			mrMeasurement.mpYUtilCpu->push_back(pMsMeasurementCpu->measure_util_avg_cur);
			mrMeasurement.mpYUtilGpuCore->push_back(pMsMeasurementGpu->nvml_util_gpu_cur);
			mrMeasurement.mpYUtilGpuMem->push_back(pMsMeasurementGpu->nvml_util_mem_cur);
			mrMeasurement.mpYUtilFpga->push_back(pMsMeasurementFpga->maxeler_util_comp_cur);
			mrMeasurement.mpYUtilMic->push_back(pMsMeasurementMic->mic_util_avg_cur);
			
			mrMeasurement.mpYMemoryCpu->push_back(pMsMeasurementCpu->measure_memory_cur[CPU_MEM_RAM_USED]>>10);
			mrMeasurement.mpYSwapCpu->push_back(pMsMeasurementCpu->measure_memory_cur[CPU_MEM_SWAP_USED]>>10);
			mrMeasurement.mpYMemoryGpu->push_back(pMsMeasurementGpu->nvml_memory_used_cur>>10);
			mrMeasurement.mpYMemoryMic->push_back(pMsMeasurementMic->mic_memory_used_cur>>10);
			
			mrMeasurement.mMaxelerActiveProcessPid = pMsMeasurementFpga->maxeler_active_process_pid;
			mrMeasurement.mMaxelerActiveProcessName = pMsMeasurementFpga->maxeler_active_process_name;
			mrMeasurement.mMaxelerActiveProcessUser = pMsMeasurementFpga->maxeler_active_process_user;
			
			mrMeasurement.mNVMLActiveProcessesCount = pMsMeasurementGpu->nvml_active_processes_count_cur;
			for(int i = 0; i < mrMeasurement.mNVMLActiveProcessesCount; i++) {
				mrMeasurement.mNVMLActiveProcessesPid[i] = pMsMeasurementGpu->nvml_active_processes_pid[i];
				mrMeasurement.mNVMLActiveProcessesName[i] = pMsMeasurementGpu->nvml_active_processes_name[i];
			}
			
			if(mrSettings.mWriteResultsToCsv) {
				printValues(csv_file);
			}
		}
		
		if(mrSettings.mWriteResultsToCsv) {
			csv_file.close();
		}
		
		deleteUnusedMs_Measurements(&pMsMeasurementCpu, &pMsMeasurementGpu, &pMsMeasurementFpga, &pMsMeasurementMic, &pMsMeasurementSys);
		
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
	
	void CDataLibrary::getMs_Measurements(MS_MEASUREMENT_CPU **pMsMeasurementCpu, MS_MEASUREMENT_GPU **pMsMeasurementGpu,
									MS_MEASUREMENT_FPGA **pMsMeasurementFpga, MS_MEASUREMENT_MIC **pMsMeasurementMic,
									MS_MEASUREMENT_SYS **pMsMeasurementSys) {
		if(mpMSSystem->config->cpu_enabled) {
			*pMsMeasurementCpu = (MS_MEASUREMENT_CPU *) getMeasurement(&mpMSMeasurement, CPU);
		} else {
			*pMsMeasurementCpu = new MS_MEASUREMENT_CPU;
			memset(*pMsMeasurementCpu ,0, sizeof(MS_MEASUREMENT_CPU));
		}
		
		if(mpMSSystem->config->gpu_enabled) {
			*pMsMeasurementGpu = (MS_MEASUREMENT_GPU *) getMeasurement(&mpMSMeasurement, GPU);
		} else {
			*pMsMeasurementGpu = new MS_MEASUREMENT_GPU;
			memset(*pMsMeasurementGpu ,0, sizeof(MS_MEASUREMENT_GPU));
		}
		
		if(mpMSSystem->config->fpga_enabled) {
			*pMsMeasurementFpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&mpMSMeasurement, FPGA);
		} else {
			*pMsMeasurementFpga = new MS_MEASUREMENT_FPGA;
			memset(*pMsMeasurementFpga ,0, sizeof(MS_MEASUREMENT_FPGA));
		}
		
		if(mpMSSystem->config->mic_enabled) {
			*pMsMeasurementMic = (MS_MEASUREMENT_MIC *) getMeasurement(&mpMSMeasurement, MIC);
		} else {
			*pMsMeasurementMic = new MS_MEASUREMENT_MIC;
			memset(*pMsMeasurementMic ,0, sizeof(MS_MEASUREMENT_MIC));
		}
		
		if(mpMSSystem->config->sys_enabled) {
			*pMsMeasurementSys = (MS_MEASUREMENT_SYS *) getMeasurement(&mpMSMeasurement, SYSTEM);
		} else {
			*pMsMeasurementSys = new MS_MEASUREMENT_SYS;
			memset(*pMsMeasurementSys ,0, sizeof(MS_MEASUREMENT_SYS));
		}
	}
	
	void CDataLibrary::deleteUnusedMs_Measurements(MS_MEASUREMENT_CPU **pMsMeasurementCpu, MS_MEASUREMENT_GPU **pMsMeasurementGpu,
											 MS_MEASUREMENT_FPGA **pMsMeasurementFpga, MS_MEASUREMENT_MIC **pMsMeasurementMic,
											 MS_MEASUREMENT_SYS **pMsMeasurementSys) {
		if(!(mpMSSystem->config->cpu_enabled)) {
			delete *pMsMeasurementCpu;
		}
		if(!(mpMSSystem->config->gpu_enabled)) {
			delete *pMsMeasurementGpu;
		}
		if(!(mpMSSystem->config->fpga_enabled)) {
			delete *pMsMeasurementFpga;
		}
		if(!(mpMSSystem->config->mic_enabled)) {
			delete *pMsMeasurementMic;
		}
		if(!(mpMSSystem->config->sys_enabled)) {
			delete *pMsMeasurementSys;
		}
	}
}
