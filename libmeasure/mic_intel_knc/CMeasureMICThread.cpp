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
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library.
 *          0.7.0 - modularised measurement struct 
 */

namespace NLibMeasure {
	template <int Variant>
	CMeasureMICThread<Variant>::CMeasureMICThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes):
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		mThreadType = "mic";
		mTimer.setThreadName("mic timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_MIC *)pMsMeasurement)->mic_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	template <int Variant>
	CMeasureMICThread<Variant>::~CMeasureMICThread() {
		// nothing todo
	}
	
	template <int Variant>
	void CMeasureMICThread<Variant>::run(void) {
		MS_MEASUREMENT_MIC *pMsMeasurementMic = (MS_MEASUREMENT_MIC *) mpMsMeasurement;
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'mic thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     effective sampling rate: " << mTimer.getTimerHertz() / pMsMeasurementMic->mic_skip_ms_rate << " Hz / "
				<< mTimer.getTimerMillisecond() * pMsMeasurementMic->mic_skip_ms_rate << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		//initialize some values
		pMsMeasurementMic->mic_time_runtime = 0.0;
		for (int i=0; i<MIC_NUM_POWER; i++) {
			pMsMeasurementMic->mic_energy_acc[i] = 0.0;
		}
		for (int i=0; i<MIC_NUM_TEMPERATURE; i++){
			pMsMeasurementMic->mic_temperature_max[i] = 0;
		}
		for (int i=0; i<MIC_NUM_UTIL; i++){
			pMsMeasurementMic->mic_util_acc[i] = 	0;
		}
		pMsMeasurementMic->mic_util_avg = 			0.0;
		pMsMeasurementMic->mic_util_active_cur = 	0.0;
		pMsMeasurementMic->mic_util_idle_cur = 		0.0;
		pMsMeasurementMic->mic_util_avg_cur = 		0.0;
		pMsMeasurementMic->mic_freq_core_acc =		0.0;
		pMsMeasurementMic->mic_freq_mem_acc = 		0.0;
		pMsMeasurementMic->mic_memory_free_max = 	0;
		pMsMeasurementMic->mic_memory_used_max =	0;
		
		mrMeasureResource.read_memory_total(mpMsMeasurement, mThreadNum);
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(pMsMeasurementMic->internal.mic_time_cur));
		pMsMeasurementMic->internal.mic_time_temp.tv_sec = pMsMeasurementMic->internal.mic_time_cur.tv_sec;
		pMsMeasurementMic->internal.mic_time_temp.tv_nsec = pMsMeasurementMic->internal.mic_time_cur.tv_nsec;
		
		for (int i=0; i<MIC_NUM_UTIL; i++){
			pMsMeasurementMic->internal.mic_util_temp[i] = pMsMeasurementMic->internal.mic_util_cur[i];
		}
		
		while(!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(Variant == FULL) {			
				if(!(skip_ms_cnt++ % pMsMeasurementMic->mic_skip_ms_rate)){
					mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
				}
				
				// calculated diff time
				calcTimeDiff(&(pMsMeasurementMic->internal.mic_time_cur), &(pMsMeasurementMic->internal.mic_time_temp), &(pMsMeasurementMic->internal.mic_time_diff), &(pMsMeasurementMic->internal.mic_time_diff_double));
				
				// result: runtime
				pMsMeasurementMic->mic_time_runtime += pMsMeasurementMic->internal.mic_time_diff_double;
				
				// result: "accumulated frequencies"
				pMsMeasurementMic->mic_freq_core_acc += pMsMeasurementMic->mic_freq_core_cur * pMsMeasurementMic->internal.mic_time_diff_double;
				pMsMeasurementMic->mic_freq_mem_acc += pMsMeasurementMic->mic_freq_mem_cur * pMsMeasurementMic->internal.mic_time_diff_double;
				
				// result: energy consumption
				for (int i=0; i<MIC_NUM_POWER; i++) {
					pMsMeasurementMic->mic_energy_acc[i] += (double)pMsMeasurementMic->mic_power_cur[i] * pMsMeasurementMic->internal.mic_time_diff_double;
				}
				
				// result: maximum temperatures
				for (int i=0; i<MIC_NUM_TEMPERATURE; i++) {
					if (pMsMeasurementMic->mic_temperature_cur[i] > pMsMeasurementMic->mic_temperature_max[i]) {
						pMsMeasurementMic->mic_temperature_max[i] = pMsMeasurementMic->mic_temperature_cur[i];
					}
				}
				
				// result: utilization
				for (int i=0; i<MIC_NUM_UTIL; i++){
					pMsMeasurementMic->mic_util_cur[i] = pMsMeasurementMic->internal.mic_util_cur[i] - pMsMeasurementMic->internal.mic_util_temp[i];
					pMsMeasurementMic->mic_util_acc[i] += pMsMeasurementMic->mic_util_cur[i];
					
					pMsMeasurementMic->internal.mic_util_temp[i] = pMsMeasurementMic->internal.mic_util_cur[i];
				}
				
				pMsMeasurementMic->mic_util_active_cur = (double)(
					pMsMeasurementMic->mic_util_cur[SYS_SUM] +
					pMsMeasurementMic->mic_util_cur[NICE_SUM] +
					pMsMeasurementMic->mic_util_cur[USER_SUM]);
				pMsMeasurementMic->mic_util_idle_cur = ((double)
					pMsMeasurementMic->mic_util_cur[IDLE_SUM]);
				pMsMeasurementMic->mic_util_avg_cur =
					(pMsMeasurementMic->mic_util_active_cur * 100.0) /
					(pMsMeasurementMic->mic_util_active_cur + pMsMeasurementMic->mic_util_idle_cur);
					
				// result: memory usage
				pMsMeasurementMic->mic_memory_used_max		=
					(pMsMeasurementMic->mic_memory_used_cur>pMsMeasurementMic->mic_memory_used_max) ?
					pMsMeasurementMic->mic_memory_used_cur : pMsMeasurementMic->mic_memory_used_max;
					
				pMsMeasurementMic->mic_memory_free_max		=
					(pMsMeasurementMic->mic_memory_free_cur>pMsMeasurementMic->mic_memory_free_max) ?
					pMsMeasurementMic->mic_memory_free_cur : pMsMeasurementMic->mic_memory_free_max;
			}
		}
		
		if(Variant == FULL) {
			// result: average power consumption
			for (int i=0; i<MIC_NUM_POWER; i++) {
				pMsMeasurementMic->mic_power_avg[i] = (double) pMsMeasurementMic->mic_energy_acc[i] / pMsMeasurementMic->mic_time_runtime;
			}
			
			//result: average frequencies
			pMsMeasurementMic->mic_freq_core_avg = pMsMeasurementMic->mic_freq_core_acc / pMsMeasurementMic->mic_time_runtime;
			pMsMeasurementMic->mic_freq_mem_avg = pMsMeasurementMic->mic_freq_mem_acc / pMsMeasurementMic->mic_time_runtime;
			
			// result average utilization
			pMsMeasurementMic->mic_util_active_total =
				pMsMeasurementMic->mic_util_acc[SYS_SUM] +
				pMsMeasurementMic->mic_util_acc[NICE_SUM] +
				pMsMeasurementMic->mic_util_acc[USER_SUM];
			pMsMeasurementMic->mic_util_idle_total = pMsMeasurementMic->mic_util_acc[IDLE_SUM];
			
			pMsMeasurementMic->mic_util_active_avg = ((double)pMsMeasurementMic->mic_util_active_total) * S_PER_JIFFY / MIC_CORES / MIC_THREADS;
			pMsMeasurementMic->mic_util_idle_avg = ((double)pMsMeasurementMic->mic_util_idle_total)* S_PER_JIFFY / MIC_CORES / MIC_THREADS;
			
			pMsMeasurementMic->mic_util_avg = (pMsMeasurementMic->mic_util_active_total * 100.0) / 
				(pMsMeasurementMic->mic_util_active_total + pMsMeasurementMic->mic_util_idle_total);
		}
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'mic thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period            : " << pMsMeasurementMic->mic_time_runtime << " s" << std::endl
		<< "     consumed energy (PCIE) : " << pMsMeasurementMic->mic_energy_acc[MIC_PCIE] << " mWs" << std::endl
		<< "     consumed energy (C2X3) : " << pMsMeasurementMic->mic_energy_acc[MIC_C2X3] << " mWs" << std::endl
		<< "     consumed energy (C2X4) : " << pMsMeasurementMic->mic_energy_acc[MIC_C2X4] << " mWs" << std::endl
		<< "     consumed energy (VCCP) : " << pMsMeasurementMic->mic_energy_acc[MIC_VCCP] << " mWs" << std::endl
		<< "     consumed energy (VDDG) : " << pMsMeasurementMic->mic_energy_acc[MIC_VDDG] << " mWs" << std::endl
		<< "     consumed energy (VDDQ) : " << pMsMeasurementMic->mic_energy_acc[MIC_VDDQ] << " mWs" << std::endl
		<< "     consumed energy (TOTAL): " << pMsMeasurementMic->mic_energy_acc[MIC_POWER] << " mWs" << std::endl
		<< "     average power (PCIE)   : " << pMsMeasurementMic->mic_power_avg[MIC_PCIE] << " mW" << std::endl
		<< "     average power (C2X3)   : " << pMsMeasurementMic->mic_power_avg[MIC_C2X3] << " mW" << std::endl
		<< "     average power (C2X4)   : " << pMsMeasurementMic->mic_power_avg[MIC_C2X4] << " mW" << std::endl
		<< "     average power (VCCP)   : " << pMsMeasurementMic->mic_power_avg[MIC_VCCP] << " mW" << std::endl
		<< "     average power (VDDG)   : " << pMsMeasurementMic->mic_power_avg[MIC_VDDG] << " mW" << std::endl
		<< "     average power (VDDQ)   : " << pMsMeasurementMic->mic_power_avg[MIC_VDDQ] << " mW" << std::endl
		<< "     average power (TOTAL)  : " << pMsMeasurementMic->mic_power_avg[MIC_POWER] << " mW" << std::endl
		<< "     max temperature (DIE)  : " << pMsMeasurementMic->mic_temperature_max[MIC_DIE_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (GDDR)  : " << pMsMeasurementMic->mic_temperature_max[MIC_GDDR_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (FAN IN): " << pMsMeasurementMic->mic_temperature_max[MIC_FAN_IN_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (FAN OUT): " << pMsMeasurementMic->mic_temperature_max[MIC_FAN_OUT_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (VCCP)  : " << pMsMeasurementMic->mic_temperature_max[MIC_VCCP_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (VDDG)  : " << pMsMeasurementMic->mic_temperature_max[MIC_VDDG_TEMP] << " \u00b0C" << std::endl
		<< "     max temperature (VDDQ)  : " << pMsMeasurementMic->mic_temperature_max[MIC_VDDQ_TEMP] << " \u00b0C" << std::endl
		<< "     average core frequency  : " << pMsMeasurementMic->mic_freq_core_avg << " MHz" << std::endl
		<< "     average memory frequency: " << pMsMeasurementMic->mic_freq_mem_avg << " MHz" << std::endl
		<< "     average utilization     : " << pMsMeasurementMic->mic_util_avg << " %%" << std::endl;
		mrLog.unlock();
#endif /* DEBUG */
	}
}
