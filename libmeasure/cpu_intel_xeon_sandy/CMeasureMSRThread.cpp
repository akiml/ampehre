/*
 * CMeasureMSRThread.cpp
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
 * created: 3/21/14
 * version: 0.1.0 - initial implementation
 *          0.1.3 - add CPU frequency measurements
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.5 - add CPU performance counters
 *          0.1.7 - add CPU utilization measurements
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#include "CMeasureMSRThread.hpp"

#include <iomanip>
#include "../../include/ms_driver.h"

#define UINT64_MAX (0xffffffffffffffff)

namespace NLibMeasure {
	CMeasureMSRThread::CMeasureMSRThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMeasurement, rMeasureRes)
		{
		mThreadType = "msr";
		mTimer.setThreadName("msr timer");
		mTimer.setTimer(&(pMeasurement->msr_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	CMeasureMSRThread::~CMeasureMSRThread() {
		// nothing todo
	}
		
	void CMeasureMSRThread::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'msr thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     effective sampling rate: " << mTimer.getTimerHertz() / mpMeasurement->msr_skip_ms_rate << " Hz / "
				<< mTimer.getTimerMillisecond() * mpMeasurement->msr_skip_ms_rate << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		mpMeasurement->msr_time_runtime	= 0.0;
		for (int i=0; i<CPUS; ++i) {
			for (int j=0; j<3; ++j) {
				mpMeasurement->msr_energy_acc[i][j]	= 0.0;
				mpMeasurement->msr_power_cur[i][j]	= 0.0;
			}
			
			mpMeasurement->msr_temperature_pkg_max[i]	= 0.0;
			for (int j=0; j<CORES; ++j) {
				mpMeasurement->msr_temperature_core_max[i][j]	= 0.0;
				mpMeasurement->msr_freq_core_acc[i][j]			= 0.0;
				mpMeasurement->msr_freq_core_eff_acc[i][j]		= 0.0;
			}
		}
		for (int i=0; i<CPUSTATS; ++i) {
			mpMeasurement->measure_util_acc[i]	= 0;
		}
		mpMeasurement->measure_util_avg			= 0.0;
		mpMeasurement->measure_util_active_cur	= 0.0;
		mpMeasurement->measure_util_idle_cur	= 0.0;
		mpMeasurement->measure_util_avg_cur		= 0.0;
		
		mpMeasurement->measure_memory_free_max 	= 0;
		mpMeasurement->measure_memory_used_max 	= 0;
		mpMeasurement->measure_swap_free_max 	= 0;
		mpMeasurement->measure_swap_used_max 	= 0;
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(mpMeasurement->internal.msr_time_cur));
		mpMeasurement->internal.msr_time_temp.tv_sec	= mpMeasurement->internal.msr_time_cur.tv_sec;
		mpMeasurement->internal.msr_time_temp.tv_nsec	= mpMeasurement->internal.msr_time_cur.tv_nsec;
		for (int i=0; i<CPUS; ++i) {
			for (int j=0; j<3; ++j) {
				mpMeasurement->internal.msr_energy_temp[i][j]			= mpMeasurement->msr_energy_cur[i][j];
			}
			for (int k=0; k<CORES; ++k) {
				mpMeasurement->internal.msr_timestamp_core_temp[i][k]	= mpMeasurement->internal.msr_timestamp_core_cur[i][k];
				mpMeasurement->internal.msr_aperf_core_temp[i][k]		= mpMeasurement->internal.msr_aperf_core_cur[i][k];
				mpMeasurement->internal.msr_mperf_core_temp[i][k]		= mpMeasurement->internal.msr_mperf_core_cur[i][k];
			}
		}
		for (int i=0; i<CPUSTATS; ++i) {
			mpMeasurement->internal.measure_util_temp[i]	= mpMeasurement->internal.measure_util_cur[i];
		}
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(!(skip_ms_cnt % mpMeasurement->msr_skip_ms_rate)){
				mrMeasureResource.measure(mpMeasurement, mThreadNum);
			}
			
			if(skip_ms_cnt == UINT64_MAX){
				skip_ms_cnt = 0;
			} else {
				skip_ms_cnt++;
			}
			
			// calculated diff time (use internal struct for that)
			calcTimeDiff(&(mpMeasurement->internal.msr_time_cur), &(mpMeasurement->internal.msr_time_temp), &(mpMeasurement->internal.msr_time_diff), &(mpMeasurement->internal.msr_time_diff_double));
			
			// result: runtime
			mpMeasurement->msr_time_runtime += mpMeasurement->internal.msr_time_diff_double;
			
#ifndef LIGHT
			// result: frequency
			for (int i=0; i<CPUS; ++i) {
				for (int j=0; j<CORES; ++j) {
					// Divide by 1000000 -> frequency in MHz (instead of Hz)
					mpMeasurement->msr_freq_core_cur[i][j]	=
						(mpMeasurement->internal.msr_timestamp_core_cur[i][j] - mpMeasurement->internal.msr_timestamp_core_temp[i][j]) /
						(mpMeasurement->internal.msr_time_diff_double * 1000000);
					
					mpMeasurement->msr_freq_core_acc[i][j]	+=
						mpMeasurement->msr_freq_core_cur[i][j] * mpMeasurement->internal.msr_time_diff_double;
					
					mpMeasurement->internal.msr_timestamp_core_temp[i][j]	= mpMeasurement->internal.msr_timestamp_core_cur[i][j];
				}
			}
			
			// result: performance
			for (int i=0; i<CPUS; ++i) {
				for (int j=0; j<CORES; ++j) {
					uint64_t diff_aperf = 0;
					uint64_t diff_mperf = 0;
					
					if (mpMeasurement->internal.msr_aperf_core_cur[i][j] > mpMeasurement->internal.msr_aperf_core_temp[i][j]) {
						diff_aperf	= mpMeasurement->internal.msr_aperf_core_cur[i][j] - mpMeasurement->internal.msr_aperf_core_temp[i][j];
					} else {
						diff_aperf	= UINT64_MAX - (mpMeasurement->internal.msr_aperf_core_temp[i][j] - mpMeasurement->internal.msr_aperf_core_cur[i][j]);
					}
					
					if (mpMeasurement->internal.msr_mperf_core_cur[i][j] > mpMeasurement->internal.msr_mperf_core_temp[i][j]) {
						diff_mperf	= mpMeasurement->internal.msr_mperf_core_cur[i][j] - mpMeasurement->internal.msr_mperf_core_temp[i][j];
					} else {
						diff_mperf	= UINT64_MAX - (mpMeasurement->internal.msr_mperf_core_temp[i][j] - mpMeasurement->internal.msr_mperf_core_cur[i][j]);
					}
					
					mpMeasurement->msr_freq_core_eff_cur[i][j]	= mpMeasurement->msr_freq_core_cur[i][j] * diff_aperf / diff_mperf;
					mpMeasurement->msr_freq_core_eff_acc[i][j]	+=
						mpMeasurement->msr_freq_core_eff_cur[i][j] * mpMeasurement->internal.msr_time_diff_double;
					
					mpMeasurement->internal.msr_aperf_core_temp[i][j]	= mpMeasurement->internal.msr_aperf_core_cur[i][j];
					mpMeasurement->internal.msr_mperf_core_temp[i][j]	= mpMeasurement->internal.msr_mperf_core_cur[i][j];
				}
			}
#endif /* LIGHT */
			
			// result: energy consumption
			for (int i=0; i<CPUS; ++i) {
				for (int j=0; j<3; ++j) {
					if (mpMeasurement->msr_energy_cur[i][j] >= mpMeasurement->internal.msr_energy_temp[i][j]) {
						mpMeasurement->msr_energy_acc[i][j]	+=
							mpMeasurement->msr_energy_cur[i][j] - mpMeasurement->internal.msr_energy_temp[i][j];
					} else {
						mpMeasurement->msr_energy_acc[i][j]	+=
							mpMeasurement->internal.msr_energy_max + (mpMeasurement->msr_energy_cur[i][j] - mpMeasurement->internal.msr_energy_temp[i][j]);
					}
					
					mpMeasurement->msr_power_cur[i][j]	=
						(mpMeasurement->msr_energy_cur[i][j] - mpMeasurement->internal.msr_energy_temp[i][j])/
						mpMeasurement->internal.msr_time_diff_double;
					
					mpMeasurement->internal.msr_energy_temp[i][j]	= mpMeasurement->msr_energy_cur[i][j];
				}
			}
			
#ifndef LIGHT
			// result: maximum temperatures
			for (int i=0; i<CPUS; ++i) {
				mpMeasurement->msr_temperature_pkg_max[i]	=
					(mpMeasurement->msr_temperature_pkg_cur[i] > mpMeasurement->msr_temperature_pkg_max[i]) ?
					mpMeasurement->msr_temperature_pkg_cur[i] : mpMeasurement->msr_temperature_pkg_max[i];
				for (int j=0; j<CORES; ++j) {
					mpMeasurement->msr_temperature_core_max[i][j]	=
						(mpMeasurement->msr_temperature_core_cur[i][j] > mpMeasurement->msr_temperature_core_max[i][j]) ?
						mpMeasurement->msr_temperature_core_cur[i][j] : mpMeasurement->msr_temperature_core_max[i][j];
				}
			}
#endif /* LIGHT */
			
			// result: utilization
			for (int i=0; i<CPUSTATS; ++i) {
				mpMeasurement->measure_util_cur[i]	=
					mpMeasurement->internal.measure_util_cur[i] - mpMeasurement->internal.measure_util_temp[i];
				
				mpMeasurement->measure_util_acc[i]	+= mpMeasurement->measure_util_cur[i];
				
				mpMeasurement->internal.measure_util_temp[i]	= mpMeasurement->internal.measure_util_cur[i];
				
				mpMeasurement->measure_util_active_cur = ((double)(
					mpMeasurement->measure_util_cur[CPUSTAT_USER] +
					mpMeasurement->measure_util_cur[CPUSTAT_NICE] +
					mpMeasurement->measure_util_cur[CPUSTAT_SYSTEM] +
					mpMeasurement->measure_util_cur[CPUSTAT_IRQ] +
					mpMeasurement->measure_util_cur[CPUSTAT_SOFTIRQ] +
					mpMeasurement->measure_util_cur[CPUSTAT_STEAL] +
					mpMeasurement->measure_util_cur[CPUSTAT_GUEST]) /
					(double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
				mpMeasurement->measure_util_idle_cur = ((double)(
					mpMeasurement->measure_util_cur[CPUSTAT_IDLE] +
					mpMeasurement->measure_util_cur[CPUSTAT_IOWAIT]) /
					(double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
					
				mpMeasurement->measure_util_avg_cur =
					(mpMeasurement->measure_util_active_cur * 100.0) /
					(mpMeasurement->measure_util_active_cur + mpMeasurement->measure_util_idle_cur);
			}
			
#ifndef LIGHT
			// result: memory usage
			mpMeasurement->measure_memory_used_max	=
				(mpMeasurement->measure_memory_cur[CPU_MEM_RAM_USED]>mpMeasurement->measure_memory_used_max) ?
				mpMeasurement->measure_memory_cur[CPU_MEM_RAM_USED] : mpMeasurement->measure_memory_used_max;
				
			mpMeasurement->measure_memory_free_max	=
				(mpMeasurement->measure_memory_cur[CPU_MEM_RAM_FREE]>mpMeasurement->measure_memory_free_max) ?
				mpMeasurement->measure_memory_cur[CPU_MEM_RAM_FREE] : mpMeasurement->measure_memory_free_max;
				
			// result: swap usage
			mpMeasurement->measure_swap_used_max	=
				(mpMeasurement->measure_memory_cur[CPU_MEM_SWAP_USED]>mpMeasurement->measure_swap_used_max) ?
				mpMeasurement->measure_memory_cur[CPU_MEM_SWAP_USED] : mpMeasurement->measure_swap_used_max;
				
			mpMeasurement->measure_swap_free_max	=
				(mpMeasurement->measure_memory_cur[CPU_MEM_SWAP_FREE]>mpMeasurement->measure_swap_free_max) ?
				mpMeasurement->measure_memory_cur[CPU_MEM_SWAP_FREE] : mpMeasurement->measure_swap_free_max;
				
#endif /* LIGHT */
			
#if 0
			mrLog.lock();
			mrLog() << "t_cpu0core0: " << mpMeasurement->msr_temperature_core_cur[0][0] << std::endl;
			mrLog() << "t_cpu0: " << mpMeasurement->msr_temperature_pkg_cur[0] << ", t_cpu1: " << mpMeasurement->msr_temperature_pkg_cur[1] << std::endl;
			mrLog() << "e_cpu0PKG: " << mpMeasurement->msr_energy_cur[0][PKG] << ", e_cpu0PP0: " << mpMeasurement->msr_energy_cur[0][PP0] << ", e_cpu0DRAM: " << mpMeasurement->msr_energy_cur[0][DRAM] <<std::endl;
			mrLog.unlock();
#endif
		}
		
		// result: average power consumption
		for (int i=0; i<CPUS; ++i) {
			for (int j=0; j<3; ++j) {
				mpMeasurement->msr_power_avg[i][j]		=
					mpMeasurement->msr_energy_acc[i][j] / mpMeasurement->msr_time_runtime;
			}
		}
		
#ifndef LIGHT
		// result: average frequency
		for (int i=0; i<CPUS; ++i) {
			for (int k=0; k<CORES; ++k) {
				mpMeasurement->msr_freq_core_avg[i][k]	=
					mpMeasurement->msr_freq_core_acc[i][k] / mpMeasurement->msr_time_runtime;
			}
		}
		
		// result: effective frequency
		for (int i=0; i<CPUS; ++i) {
			for (int k=0; k<CORES; ++k) {
				mpMeasurement->msr_freq_core_eff_avg[i][k]	=
					mpMeasurement->msr_freq_core_eff_acc[i][k] / mpMeasurement->msr_time_runtime;
			}
		}
#endif /* LIGHT */
		
		// result average utilization
		mpMeasurement->measure_util_active_total =
			mpMeasurement->measure_util_acc[CPUSTAT_USER] +
			mpMeasurement->measure_util_acc[CPUSTAT_NICE] +
			mpMeasurement->measure_util_acc[CPUSTAT_SYSTEM] +
			mpMeasurement->measure_util_acc[CPUSTAT_IRQ] +
			mpMeasurement->measure_util_acc[CPUSTAT_SOFTIRQ] +
			mpMeasurement->measure_util_acc[CPUSTAT_STEAL] +
			mpMeasurement->measure_util_acc[CPUSTAT_GUEST];
		mpMeasurement->measure_util_idle_total =
			mpMeasurement->measure_util_acc[CPUSTAT_IDLE] +
			mpMeasurement->measure_util_acc[CPUSTAT_IOWAIT];
		
		mpMeasurement->measure_util_active_avg =
			((double)mpMeasurement->measure_util_active_total / (double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
		mpMeasurement->measure_util_idle_avg =
			((double)mpMeasurement->measure_util_idle_total / (double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
		
		uint64_t util_time_total =
			mpMeasurement->measure_util_active_total +
			mpMeasurement->measure_util_idle_total;
		uint64_t util_time_idle =
			mpMeasurement->measure_util_idle_total;
		
		mpMeasurement->measure_util_avg =
			((util_time_total - util_time_idle) * 100.0) / util_time_total;
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'msr thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period                 : " << mpMeasurement->msr_time_runtime << " s" << std::endl;
		for (int i=0; i<CPUS; ++i) {
			mrLog()
			<< "     energy cpu " << i << " domain pkg  acc: " << mpMeasurement->msr_energy_acc[i][PKG] << " mWs" << std::endl
			<< "     energy cpu " << i << " domain pp0  acc: " << mpMeasurement->msr_energy_acc[i][PP0] << " mWs" << std::endl
			<< "     energy cpu " << i << " domain dram acc: " << mpMeasurement->msr_energy_acc[i][DRAM] << " mWs" << std::endl
			<< "     power  cpu " << i << " domain pkg  avg: " << mpMeasurement->msr_power_avg[i][PKG] << " mW" << std::endl
			<< "     power  cpu " << i << " domain pp0  avg: " << mpMeasurement->msr_power_avg[i][PP0] << " mW" << std::endl
			<< "     power  cpu " << i << " domain dram avg: " << mpMeasurement->msr_power_avg[i][DRAM] << " mW" << std::endl
			<< "     temp   cpu " << i << " pkg         max: " << mpMeasurement->msr_temperature_pkg_max[i] << " \u00b0C" << std::endl;
			for (int j=0; j<CORES; ++j) {
				mrLog()
				<< "     temp   cpu " << i << " core " << j << "      max: " << mpMeasurement->msr_temperature_core_max[i][j] << " \u00b0C" << std::endl
				<< "     freq   cpu " << i << " core " << j << "      avg: " << mpMeasurement->msr_freq_core_avg[i][j] << " MHz" <<
				std::endl
				<< "     freq   cpu " << i << " core " << j << "      eff: " << mpMeasurement->msr_freq_core_eff_avg[i][j] << " MHz" <<
				std::endl;
			}
			mrLog()
			<< "     util   cpu all           avg: " << mpMeasurement->measure_util_avg << " %" << std::endl;
		}
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}
