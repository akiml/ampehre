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
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#include "CMeasureNVMLThread.hpp"

#include <iostream>
#include <ctime>

namespace NLibMeasure {
	CMeasureNVMLThread::CMeasureNVMLThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMeasurement, rMeasureRes)
		{
		mThreadType = "nvml";
		mTimer.setThreadName("nvml timer");
		mTimer.setTimer(&(pMeasurement->nvml_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	CMeasureNVMLThread::~CMeasureNVMLThread() {
		// nothing todo
	}
		
	void CMeasureNVMLThread::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'nvml thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     effective sampling rate: " << mTimer.getTimerHertz() / mpMeasurement->nvml_skip_ms_rate << " Hz / "
				<< mTimer.getTimerMillisecond() * mpMeasurement->nvml_skip_ms_rate << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		mpMeasurement->nvml_time_runtime		= 0.0;
		mpMeasurement->nvml_energy_acc			= 0.0;
		mpMeasurement->nvml_temperature_max		= 0;
		mpMeasurement->nvml_clock_graphics_acc	= 0.0;
		mpMeasurement->nvml_clock_sm_acc		= 0.0;
		mpMeasurement->nvml_clock_mem_acc		= 0.0;
		mpMeasurement->nvml_util_gpu_acc		= 0.0;
		mpMeasurement->nvml_util_mem_acc		= 0.0;
		mpMeasurement->nvml_memory_used_max		= 0.0;
		mpMeasurement->nvml_memory_free_max		= 0.0;
		
		mrMeasureResource.read_memory_total(mpMeasurement, mThreadNum);
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(mpMeasurement->internal.nvml_time_cur));
		mpMeasurement->internal.nvml_time_temp.tv_sec	= mpMeasurement->internal.nvml_time_cur.tv_sec;
		mpMeasurement->internal.nvml_time_temp.tv_nsec	= mpMeasurement->internal.nvml_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(!(skip_ms_cnt % mpMeasurement->nvml_skip_ms_rate)){
				mrMeasureResource.measure(mpMeasurement, mThreadNum);
			}
			
			if(skip_ms_cnt == UINT64_MAX){
				skip_ms_cnt = 0;
			} else {
				skip_ms_cnt++;
			}
			
			// calculated diff time
			calcTimeDiff(&(mpMeasurement->internal.nvml_time_cur), &(mpMeasurement->internal.nvml_time_temp), &(mpMeasurement->internal.nvml_time_diff), &(mpMeasurement->internal.nvml_time_diff_double));
			
			// result: runtime
			mpMeasurement->nvml_time_runtime		+= mpMeasurement->internal.nvml_time_diff_double;
			
			// result: energy consumption
			mpMeasurement->nvml_energy_acc			+= mpMeasurement->nvml_power_cur * mpMeasurement->internal.nvml_time_diff_double;
			
#ifndef LIGHT
			// result: memory usage
			mpMeasurement->nvml_memory_used_max		=
				(mpMeasurement->nvml_memory_used_cur>mpMeasurement->nvml_memory_used_max) ?
				mpMeasurement->nvml_memory_used_cur : mpMeasurement->nvml_memory_used_max;
				
			mpMeasurement->nvml_memory_free_max		=
				(mpMeasurement->nvml_memory_free_cur>mpMeasurement->nvml_memory_free_max) ?
				mpMeasurement->nvml_memory_free_cur : mpMeasurement->nvml_memory_free_max;
			
			// result: maximum temperature
			mpMeasurement->nvml_temperature_max		=
				(mpMeasurement->nvml_temperature_cur>mpMeasurement->nvml_temperature_max) ?
				mpMeasurement->nvml_temperature_cur : mpMeasurement->nvml_temperature_max;
			
			// result: "accumulated frequencies"
			mpMeasurement->nvml_clock_graphics_acc	+= mpMeasurement->nvml_clock_graphics_cur * mpMeasurement->internal.nvml_time_diff_double;
			mpMeasurement->nvml_clock_sm_acc		+= mpMeasurement->nvml_clock_sm_cur * mpMeasurement->internal.nvml_time_diff_double;
			mpMeasurement->nvml_clock_mem_acc		+= mpMeasurement->nvml_clock_mem_cur * mpMeasurement->internal.nvml_time_diff_double;
			
			// result: average clock speeds
			mpMeasurement->nvml_clock_graphics_avg	= (mpMeasurement->nvml_clock_graphics_acc)/mpMeasurement->nvml_time_runtime;
			mpMeasurement->nvml_clock_sm_avg		= (mpMeasurement->nvml_clock_sm_acc)/mpMeasurement->nvml_time_runtime;
			mpMeasurement->nvml_clock_mem_avg		= (mpMeasurement->nvml_clock_mem_acc)/mpMeasurement->nvml_time_runtime;
			
			// result: "accumulated utilization"
			mpMeasurement->nvml_util_gpu_acc		+= mpMeasurement->nvml_util_gpu_cur * mpMeasurement->internal.nvml_time_diff_double;
			mpMeasurement->nvml_util_mem_acc		+= mpMeasurement->nvml_util_mem_cur * mpMeasurement->internal.nvml_time_diff_double;
			
			// result: average utilization
			mpMeasurement->nvml_util_gpu_avg		= (mpMeasurement->nvml_util_gpu_acc)/mpMeasurement->nvml_time_runtime;
			mpMeasurement->nvml_util_mem_avg		= (mpMeasurement->nvml_util_mem_acc)/mpMeasurement->nvml_time_runtime;
#endif /* LIGHT */
#if 0
			mrLog() << "t: " << mpMeasurement->internal.nvml_time_diff_double << ", P: " << mpMeasurement->nvml_power_cur << ", S: " << mpMeasurement->internal.nvml_power_state << std::endl;
#endif
#if 0
			mrLog() << "CLK_graph: " << mpMeasurement->nvml_clock_graphics_cur << " MHz, CLK_sm: " << mpMeasurement->nvml_clock_sm_cur << " MHz, CLK_mem: " << mpMeasurement->nvml_clock_mem_cur << " MHz" << std::endl;
#endif
		}
		
		// result: average power consumption
		mpMeasurement->nvml_power_avg 			= (mpMeasurement->nvml_energy_acc)/mpMeasurement->nvml_time_runtime;
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'nvml thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period    : " << mpMeasurement->nvml_time_runtime << " s" << std::endl
		<< "     consumed energy: " << mpMeasurement->nvml_energy_acc << " mWs" << std::endl
		<< "     average power  : " << mpMeasurement->nvml_power_avg << " mW" << std::endl
		<< "     max temperature: " << mpMeasurement->nvml_temperature_cur << " \u00b0C" << std::endl
		<< "     avg graphics f : " << mpMeasurement->nvml_clock_graphics_avg << " MHz" << std::endl
		<< "     avg util gpu   : " << mpMeasurement->nvml_util_gpu_avg << " %%" << std::endl
		<< "     avg util memory: " << mpMeasurement->nvml_util_mem_avg << " %%" << std::endl;
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}
