/*
 * CMeasureMICThread.cpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 4/7/15
 * version: 0.4.0 - MIC integration into libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#include "CMeasureMICThread.hpp"

namespace NLibMeasure {
	CMeasureMICThread::CMeasureMICThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes):
		CMeasureAbstractThread(rLogger, rStartSem, pMeasurement, rMeasureRes)
		{
		mThreadType = "mic";
		mTimer.setThreadName("mic timer");
		mTimer.setTimer(&(pMeasurement->mic_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	CMeasureMICThread::~CMeasureMICThread() {
		// nothing todo
	}
	
	void CMeasureMICThread::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'mic thread' (thread #" << mThreadNum << "): init" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		//initialize some values
		mpMeasurement->mic_time_runtime = 0.0;
		for (int i=0; i<MIC_NUM_POWER; i++) {
			mpMeasurement->mic_energy_acc[i] = 0.0;
		}
		for (int i=0; i<MIC_NUM_TEMPERATURE; i++){
			mpMeasurement->mic_temperature_max[i] = 0;
		}
		for (int i=0; i<MIC_NUM_UTIL; i++){
			mpMeasurement->mic_util_acc[i] = 	0;
		}
		mpMeasurement->mic_util_avg = 			0.0;
		mpMeasurement->mic_util_active_cur = 	0.0;
		mpMeasurement->mic_util_idle_cur = 		0.0;
		mpMeasurement->mic_util_avg_cur = 		0.0;
		mpMeasurement->mic_freq_core_acc =		0.0;
		mpMeasurement->mic_freq_mem_acc = 		0.0;
		mpMeasurement->mic_memory_free_max = 	0;
		mpMeasurement->mic_memory_used_max =	0;
		
		(static_cast<NLibMeasure::CMeasureMIC&>(mrMeasureResource)).read_memory_total(mpMeasurement, mThreadNum);
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(mpMeasurement->internal.mic_time_cur));
		mpMeasurement->internal.mic_time_temp.tv_sec = mpMeasurement->internal.mic_time_cur.tv_sec;
		mpMeasurement->internal.mic_time_temp.tv_nsec = mpMeasurement->internal.mic_time_cur.tv_nsec;
		
		for (int i=0; i<MIC_NUM_UTIL; i++){
			mpMeasurement->internal.mic_util_temp[i] = mpMeasurement->internal.mic_util_cur[i];
		}
		
		while(!mThreadStateStop) {
			mMutexTimer.lock();
			
#ifndef LIGHT
			mrMeasureResource.measure(mpMeasurement, mThreadNum);
			
			// calculated diff time
			calcTimeDiff(&(mpMeasurement->internal.mic_time_cur), &(mpMeasurement->internal.mic_time_temp), &(mpMeasurement->internal.mic_time_diff), &(mpMeasurement->internal.mic_time_diff_double));
			
			// result: runtime
			mpMeasurement->mic_time_runtime += mpMeasurement->internal.mic_time_diff_double;
			
			// result: "accumulated frequencies"
			mpMeasurement->mic_freq_core_acc += mpMeasurement->mic_freq_core_cur * mpMeasurement->internal.mic_time_diff_double;
			mpMeasurement->mic_freq_mem_acc += mpMeasurement->mic_freq_mem_cur * mpMeasurement->internal.mic_time_diff_double;
			
			// result: energy consumption
			for (int i=0; i<MIC_NUM_POWER; i++) {
				mpMeasurement->mic_energy_acc[i] += (double)mpMeasurement->mic_power_cur[i] * mpMeasurement->internal.mic_time_diff_double;
			}
			
			// result: maximum temperatures
			for (int i=0; i<MIC_NUM_TEMPERATURE; i++) {
				if (mpMeasurement->mic_temperature_cur[i] > mpMeasurement->mic_temperature_max[i]) {
					mpMeasurement->mic_temperature_max[i] = mpMeasurement->mic_temperature_cur[i];
				}
			}
			
			// result: utilization
			for (int i=0; i<MIC_NUM_UTIL; i++){
				mpMeasurement->mic_util_cur[i] = mpMeasurement->internal.mic_util_cur[i] - mpMeasurement->internal.mic_util_temp[i];
				mpMeasurement->mic_util_acc[i] += mpMeasurement->mic_util_cur[i];
				
				mpMeasurement->internal.mic_util_temp[i] = mpMeasurement->internal.mic_util_cur[i];
			}
			
			mpMeasurement->mic_util_active_cur = (double)(
				mpMeasurement->mic_util_cur[SYS_SUM] +
				mpMeasurement->mic_util_cur[NICE_SUM] +
				mpMeasurement->mic_util_cur[USER_SUM]);
			mpMeasurement->mic_util_idle_cur = ((double)
				mpMeasurement->mic_util_cur[IDLE_SUM]);
			mpMeasurement->mic_util_avg_cur =
				(mpMeasurement->mic_util_active_cur * 100.0) /
				(mpMeasurement->mic_util_active_cur + mpMeasurement->mic_util_idle_cur);
				
			// result: memory usage
			mpMeasurement->mic_memory_used_max		=
				(mpMeasurement->mic_memory_used_cur>mpMeasurement->mic_memory_used_max) ?
				mpMeasurement->mic_memory_used_cur : mpMeasurement->mic_memory_used_max;
				
			mpMeasurement->mic_memory_free_max		=
				(mpMeasurement->mic_memory_free_cur>mpMeasurement->mic_memory_free_max) ?
				mpMeasurement->mic_memory_free_cur : mpMeasurement->mic_memory_free_max;
#endif /* LIGHT */
		}
		
#ifndef LIGHT
		// result: average power consumption
		for (int i=0; i<MIC_NUM_POWER; i++) {
			mpMeasurement->mic_power_avg[i] = (double) mpMeasurement->mic_energy_acc[i] / mpMeasurement->mic_time_runtime;
		}
		
		//result: average frequencies
		mpMeasurement->mic_freq_core_avg = mpMeasurement->mic_freq_core_acc / mpMeasurement->mic_time_runtime;
		mpMeasurement->mic_freq_mem_avg = mpMeasurement->mic_freq_mem_acc / mpMeasurement->mic_time_runtime;
		
		// result average utilization
		mpMeasurement->mic_util_active_total =
			mpMeasurement->mic_util_acc[SYS_SUM] +
			mpMeasurement->mic_util_acc[NICE_SUM] +
			mpMeasurement->mic_util_acc[USER_SUM];
		mpMeasurement->mic_util_idle_total = mpMeasurement->mic_util_acc[IDLE_SUM];
		
		mpMeasurement->mic_util_active_avg = ((double)mpMeasurement->mic_util_active_total) * S_PER_JIFFY / MIC_CORES / MIC_THREADS;
		mpMeasurement->mic_util_idle_avg = ((double)mpMeasurement->mic_util_idle_total)* S_PER_JIFFY / MIC_CORES / MIC_THREADS;
		
		mpMeasurement->mic_util_avg = (mpMeasurement->mic_util_active_total * 100.0) / 
			(mpMeasurement->mic_util_active_total + mpMeasurement->mic_util_idle_total);
#endif /* LIGHT */
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'mic thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period            : " << mpMeasurement->mic_time_runtime << " s" << std::endl
		<< "     consumed energy (PCIE) : " << mpMeasurement->mic_energy_acc[MIC_PCIE] << " mWs" << std::endl
		<< "     consumed energy (C2X3) : " << mpMeasurement->mic_energy_acc[MIC_C2X3] << " mWs" << std::endl
		<< "     consumed energy (C2X4) : " << mpMeasurement->mic_energy_acc[MIC_C2X4] << " mWs" << std::endl
		<< "     consumed energy (VCCP) : " << mpMeasurement->mic_energy_acc[MIC_VCCP] << " mWs" << std::endl
		<< "     consumed energy (VDDG) : " << mpMeasurement->mic_energy_acc[MIC_VDDG] << " mWs" << std::endl
		<< "     consumed energy (VDDQ) : " << mpMeasurement->mic_energy_acc[MIC_VDDQ] << " mWs" << std::endl
		<< "     consumed energy (TOTAL): " << mpMeasurement->mic_energy_acc[MIC_POWER] << " mWs" << std::endl
		<< "     average power (PCIE)   : " << mpMeasurement->mic_power_avg[MIC_PCIE] << " mW" << std::endl
		<< "     average power (C2X3)   : " << mpMeasurement->mic_power_avg[MIC_C2X3] << " mW" << std::endl
		<< "     average power (C2X4)   : " << mpMeasurement->mic_power_avg[MIC_C2X4] << " mW" << std::endl
		<< "     average power (VCCP)   : " << mpMeasurement->mic_power_avg[MIC_VCCP] << " mW" << std::endl
		<< "     average power (VDDG)   : " << mpMeasurement->mic_power_avg[MIC_VDDG] << " mW" << std::endl
		<< "     average power (VDDQ)   : " << mpMeasurement->mic_power_avg[MIC_VDDQ] << " mW" << std::endl
		<< "     average power (TOTAL)  : " << mpMeasurement->mic_power_avg[MIC_POWER] << " mW" << std::endl
		<< "     max temperature (DIE)  : " << mpMeasurement->mic_temperature_max[MIC_DIE_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (GDDR)  : " << mpMeasurement->mic_temperature_max[MIC_GDDR_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (FAN IN): " << mpMeasurement->mic_temperature_max[MIC_FAN_IN_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (FAN OUT): " << mpMeasurement->mic_temperature_max[MIC_FAN_OUT_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (VCCP)  : " << mpMeasurement->mic_temperature_max[MIC_VCCP_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (VDDG)  : " << mpMeasurement->mic_temperature_max[MIC_VDDG_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (VDDQ)  : " << mpMeasurement->mic_temperature_max[MIC_VDDQ_TEMP] << " \u00b0C" << std::endl
		<< "     average core frequency  : " << mpMeasurement->mic_freq_core_avg << " MHz" << std::endl
		<< "     average memory frequency: " << mpMeasurement->mic_freq_mem_avg << " MHz" << std::endl
		<< "     average utilization     : " << mpMeasurement->mic_util_avg << " %%" << std::endl;
		mrLog.unlock();
#endif /* DEBUG */
	}
}