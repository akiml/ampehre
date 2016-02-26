/*
 * CMeasureNVMLThread.cpp
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
 * created: 3/04/14
 * version: 0.1.0 - initial implementation
 *          0.1.2 - add GPU frequency measurements
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.8 - add GPU utilization measurements
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and show them in msmonitor
 */

namespace NLibMeasure {
	template <int TVariant>
	CMeasureNVMLThread<TVariant>::CMeasureNVMLThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		mThreadType = "nvml";
		mTimer.setThreadName("nvml timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_GPU *)pMsMeasurement)->nvml_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	template <int TVariant>
	CMeasureNVMLThread<TVariant>::~CMeasureNVMLThread() {
		// nothing todo
	}
	
	template <int TVariant>
	void CMeasureNVMLThread<TVariant>::run(void) {
		MS_MEASUREMENT_GPU *pMsMeasurementGpu = (MS_MEASUREMENT_GPU *) mpMsMeasurement;
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'nvml thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     sampling rate: " << mTimer.getTimerHertz() / pMsMeasurementGpu->nvml_check_for_exit_interrupts << " Hz / "
				<< mTimer.getTimerMillisecond() * pMsMeasurementGpu->nvml_check_for_exit_interrupts << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		pMsMeasurementGpu->nvml_time_runtime				= 0.0;
		pMsMeasurementGpu->nvml_energy_acc					= 0.0;
		pMsMeasurementGpu->nvml_temperature_max				= 0;
		pMsMeasurementGpu->nvml_clock_graphics_acc			= 0.0;
		pMsMeasurementGpu->nvml_clock_sm_acc				= 0.0;
		pMsMeasurementGpu->nvml_clock_mem_acc				= 0.0;
		pMsMeasurementGpu->nvml_util_gpu_acc				= 0.0;
		pMsMeasurementGpu->nvml_util_mem_acc				= 0.0;
		pMsMeasurementGpu->nvml_memory_used_max				= 0.0;
		pMsMeasurementGpu->nvml_memory_free_max				= 0.0;
		pMsMeasurementGpu->nvml_active_processes_count_cur	= 0;
		pMsMeasurementGpu->nvml_active_processes_count_max	= 0;
		
		mrMeasureResource.read_memory_total(mpMsMeasurement, mThreadNum);
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(pMsMeasurementGpu->internal.nvml_time_cur));
		pMsMeasurementGpu->internal.nvml_time_temp.tv_sec	= pMsMeasurementGpu->internal.nvml_time_cur.tv_sec;
		pMsMeasurementGpu->internal.nvml_time_temp.tv_nsec	= pMsMeasurementGpu->internal.nvml_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(!(skip_ms_cnt++ % pMsMeasurementGpu->nvml_check_for_exit_interrupts)){
				mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
			}
			
			// calculated diff time
			calcTimeDiff(&(pMsMeasurementGpu->internal.nvml_time_cur), &(pMsMeasurementGpu->internal.nvml_time_temp), &(pMsMeasurementGpu->internal.nvml_time_diff), &(pMsMeasurementGpu->internal.nvml_time_diff_double));
			
			// result: runtime
			pMsMeasurementGpu->nvml_time_runtime		+= pMsMeasurementGpu->internal.nvml_time_diff_double;
			
			// result: energy consumption
			pMsMeasurementGpu->nvml_energy_acc			+= pMsMeasurementGpu->nvml_power_cur * pMsMeasurementGpu->internal.nvml_time_diff_double;
			
			if(TVariant == VARIANT_FULL) {
				// result: memory usage
				pMsMeasurementGpu->nvml_memory_used_max		=
					(pMsMeasurementGpu->nvml_memory_used_cur>pMsMeasurementGpu->nvml_memory_used_max) ?
					pMsMeasurementGpu->nvml_memory_used_cur : pMsMeasurementGpu->nvml_memory_used_max;
					
				pMsMeasurementGpu->nvml_memory_free_max		=
					(pMsMeasurementGpu->nvml_memory_free_cur>pMsMeasurementGpu->nvml_memory_free_max) ?
					pMsMeasurementGpu->nvml_memory_free_cur : pMsMeasurementGpu->nvml_memory_free_max;
				
				// result: maximum temperature
				pMsMeasurementGpu->nvml_temperature_max		=
					(pMsMeasurementGpu->nvml_temperature_cur>pMsMeasurementGpu->nvml_temperature_max) ?
					pMsMeasurementGpu->nvml_temperature_cur : pMsMeasurementGpu->nvml_temperature_max;
				
				// result: "accumulated frequencies"
				pMsMeasurementGpu->nvml_clock_graphics_acc	+= pMsMeasurementGpu->nvml_clock_graphics_cur * pMsMeasurementGpu->internal.nvml_time_diff_double;
				pMsMeasurementGpu->nvml_clock_sm_acc		+= pMsMeasurementGpu->nvml_clock_sm_cur * pMsMeasurementGpu->internal.nvml_time_diff_double;
				pMsMeasurementGpu->nvml_clock_mem_acc		+= pMsMeasurementGpu->nvml_clock_mem_cur * pMsMeasurementGpu->internal.nvml_time_diff_double;
				
				// result: average clock speeds
				pMsMeasurementGpu->nvml_clock_graphics_avg	= (pMsMeasurementGpu->nvml_clock_graphics_acc)/pMsMeasurementGpu->nvml_time_runtime;
				pMsMeasurementGpu->nvml_clock_sm_avg		= (pMsMeasurementGpu->nvml_clock_sm_acc)/pMsMeasurementGpu->nvml_time_runtime;
				pMsMeasurementGpu->nvml_clock_mem_avg		= (pMsMeasurementGpu->nvml_clock_mem_acc)/pMsMeasurementGpu->nvml_time_runtime;
				
				// result: "accumulated utilization"
				pMsMeasurementGpu->nvml_util_gpu_acc		+= pMsMeasurementGpu->nvml_util_gpu_cur * pMsMeasurementGpu->internal.nvml_time_diff_double;
				pMsMeasurementGpu->nvml_util_mem_acc		+= pMsMeasurementGpu->nvml_util_mem_cur * pMsMeasurementGpu->internal.nvml_time_diff_double;
				
				// result: average utilization
				pMsMeasurementGpu->nvml_util_gpu_avg		= (pMsMeasurementGpu->nvml_util_gpu_acc)/pMsMeasurementGpu->nvml_time_runtime;
				pMsMeasurementGpu->nvml_util_mem_avg		= (pMsMeasurementGpu->nvml_util_mem_acc)/pMsMeasurementGpu->nvml_time_runtime;
			}
			
#if 0
			mrLog() << "t: " << pMsMeasurementGpu->internal.nvml_time_diff_double << ", P: " << pMsMeasurementGpu->nvml_power_cur << ", S: " << pMsMeasurementGpu->internal.nvml_power_state << std::endl;
#endif
#if 0
			mrLog() << "CLK_graph: " << pMsMeasurementGpu->nvml_clock_graphics_cur << " MHz, CLK_sm: " << pMsMeasurementGpu->nvml_clock_sm_cur << " MHz, CLK_mem: " << pMsMeasurementGpu->nvml_clock_mem_cur << " MHz" << std::endl;
#endif
		}
		
		// result: average power consumption
		pMsMeasurementGpu->nvml_power_avg 			= (pMsMeasurementGpu->nvml_energy_acc)/pMsMeasurementGpu->nvml_time_runtime;
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'nvml thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period    : " << pMsMeasurementGpu->nvml_time_runtime << " s" << std::endl
		<< "     consumed energy: " << pMsMeasurementGpu->nvml_energy_acc << " mWs" << std::endl
		<< "     average power  : " << pMsMeasurementGpu->nvml_power_avg << " mW" << std::endl
		<< "     max temperature: " << pMsMeasurementGpu->nvml_temperature_max << " \u00b0C" << std::endl
		<< "     avg graphics f : " << pMsMeasurementGpu->nvml_clock_graphics_avg << " MHz" << std::endl
		<< "     avg util gpu   : " << pMsMeasurementGpu->nvml_util_gpu_avg << " %%" << std::endl
		<< "     avg util memory: " << pMsMeasurementGpu->nvml_util_mem_avg << " %%" << std::endl;
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}
