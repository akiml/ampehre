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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularized measurement struct 
 */

#include <iomanip>
#include "../../include/ms_driver.h"

#define UINT64_MAX (0xffffffffffffffff)

namespace NLibMeasure {
	template <int TVariant>
	CMeasureMSRThread<TVariant>::CMeasureMSRThread(CLogger& rLogger, CSemaphore& rStartSem, void *pMsMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		mThreadType = "msr";
		mTimer.setThreadName("msr timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_CPU*)pMsMeasurement)->msr_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	template <int TVariant>
	CMeasureMSRThread<TVariant>::~CMeasureMSRThread() {
		// nothing todo
	}
	
	template <int TVariant>
	void CMeasureMSRThread<TVariant>::run(void) {
		MS_MEASUREMENT_CPU *pMsMeasurementCpu = (MS_MEASUREMENT_CPU*) mpMsMeasurement;
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'msr thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     sampling rate: " << mTimer.getTimerHertz() / pMsMeasurementCpu->msr_check_for_exit_interrupts << " Hz / "
				<< mTimer.getTimerMillisecond() * pMsMeasurementCpu->msr_check_for_exit_interrupts << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		pMsMeasurementCpu->msr_time_runtime	= 0.0;
		for (int i=0; i<CPUS; ++i) {
			for (int j=0; j<3; ++j) {
				pMsMeasurementCpu->msr_energy_acc[i][j]	= 0.0;
				pMsMeasurementCpu->msr_power_cur[i][j]	= 0.0;
			}
			
			pMsMeasurementCpu->msr_voltage_cur[i]		= 0.0;
			pMsMeasurementCpu->msr_freq_cpu_eff_cur[i]	= 0.0;
			pMsMeasurementCpu->msr_v2freq_acc[i]		= 0.0;
			pMsMeasurementCpu->msr_pstate_cur[i]		= 0;
			pMsMeasurementCpu->msr_pstate_acc[i]		= 0;
			
			pMsMeasurementCpu->msr_temperature_pkg_max[i]	= 0.0;
			for (int j=0; j<CORES; ++j) {
				pMsMeasurementCpu->msr_temperature_core_max[i][j]	= 0.0;
				pMsMeasurementCpu->msr_freq_core_acc[i][j]			= 0.0;
				pMsMeasurementCpu->msr_freq_core_eff_acc[i][j]		= 0.0;
			}
		}
		for (int i=0; i<CPUSTATS; ++i) {
			pMsMeasurementCpu->measure_util_acc[i]	= 0;
		}
		pMsMeasurementCpu->measure_util_avg			= 0.0;
		pMsMeasurementCpu->measure_util_active_cur	= 0.0;
		pMsMeasurementCpu->measure_util_idle_cur	= 0.0;
		pMsMeasurementCpu->measure_util_avg_cur		= 0.0;
		
		pMsMeasurementCpu->measure_memory_free_max 	= 0;
		pMsMeasurementCpu->measure_memory_used_max 	= 0;
		pMsMeasurementCpu->measure_swap_free_max 	= 0;
		pMsMeasurementCpu->measure_swap_used_max 	= 0;
				
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(pMsMeasurementCpu->internal.msr_time_cur));
		pMsMeasurementCpu->internal.msr_time_temp.tv_sec	= pMsMeasurementCpu->internal.msr_time_cur.tv_sec;
		pMsMeasurementCpu->internal.msr_time_temp.tv_nsec	= pMsMeasurementCpu->internal.msr_time_cur.tv_nsec;
		for (int i=0; i<CPUS; ++i) {
			for (int j=0; j<3; ++j) {
				pMsMeasurementCpu->internal.msr_energy_temp[i][j]			= pMsMeasurementCpu->msr_energy_cur[i][j];
			}
			for (int k=0; k<CORES; ++k) {
				pMsMeasurementCpu->internal.msr_timestamp_core_temp[i][k]	= pMsMeasurementCpu->internal.msr_timestamp_core_cur[i][k];
				pMsMeasurementCpu->internal.msr_aperf_core_temp[i][k]		= pMsMeasurementCpu->internal.msr_aperf_core_cur[i][k];
				pMsMeasurementCpu->internal.msr_mperf_core_temp[i][k]		= pMsMeasurementCpu->internal.msr_mperf_core_cur[i][k];
			}
		}
		for (int i=0; i<CPUSTATS; ++i) {
			pMsMeasurementCpu->internal.measure_util_temp[i]	= pMsMeasurementCpu->internal.measure_util_cur[i];
		}
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(!(skip_ms_cnt++ % pMsMeasurementCpu->msr_check_for_exit_interrupts)){
				mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
			}
			
			// calculated diff time (use internal struct for that)
			calcTimeDiff(&(pMsMeasurementCpu->internal.msr_time_cur), &(pMsMeasurementCpu->internal.msr_time_temp), &(pMsMeasurementCpu->internal.msr_time_diff), &(pMsMeasurementCpu->internal.msr_time_diff_double));
			
			// result: runtime
			pMsMeasurementCpu->msr_time_runtime += pMsMeasurementCpu->internal.msr_time_diff_double;
			
			if(TVariant == VARIANT_FULL) {
				// result: frequency
				for (int i=0; i<CPUS; ++i) {
					for (int j=0; j<CORES; ++j) {
						// Divide by 1000000 -> frequency in MHz (instead of Hz)
						pMsMeasurementCpu->msr_freq_core_cur[i][j]	=
							(pMsMeasurementCpu->internal.msr_timestamp_core_cur[i][j] - pMsMeasurementCpu->internal.msr_timestamp_core_temp[i][j]) /
							(pMsMeasurementCpu->internal.msr_time_diff_double * 1000000);
						
						pMsMeasurementCpu->msr_freq_core_acc[i][j]	+=
							pMsMeasurementCpu->msr_freq_core_cur[i][j] * pMsMeasurementCpu->internal.msr_time_diff_double;
						
						pMsMeasurementCpu->internal.msr_timestamp_core_temp[i][j]	= pMsMeasurementCpu->internal.msr_timestamp_core_cur[i][j];
					}
				}
				
				// result: performance
				for (int i=0; i<CPUS; ++i) {
					double msr_freq_cpu_eff_max	= 0.0;
					
					for (int j=0; j<CORES; ++j) {
						uint64_t diff_aperf = 0;
						uint64_t diff_mperf = 0;
						
						if (pMsMeasurementCpu->internal.msr_aperf_core_cur[i][j] > pMsMeasurementCpu->internal.msr_aperf_core_temp[i][j]) {
							diff_aperf	= pMsMeasurementCpu->internal.msr_aperf_core_cur[i][j] - pMsMeasurementCpu->internal.msr_aperf_core_temp[i][j];
						} else {
							diff_aperf	= UINT64_MAX - (pMsMeasurementCpu->internal.msr_aperf_core_temp[i][j] - pMsMeasurementCpu->internal.msr_aperf_core_cur[i][j]);
						}
						
						if (pMsMeasurementCpu->internal.msr_mperf_core_cur[i][j] > pMsMeasurementCpu->internal.msr_mperf_core_temp[i][j]) {
							diff_mperf	= pMsMeasurementCpu->internal.msr_mperf_core_cur[i][j] - pMsMeasurementCpu->internal.msr_mperf_core_temp[i][j];
						} else {
							diff_mperf	= UINT64_MAX - (pMsMeasurementCpu->internal.msr_mperf_core_temp[i][j] - pMsMeasurementCpu->internal.msr_mperf_core_cur[i][j]);
						}
						
						pMsMeasurementCpu->msr_freq_core_eff_cur[i][j]	= pMsMeasurementCpu->msr_freq_core_cur[i][j] * diff_aperf / diff_mperf;
						pMsMeasurementCpu->msr_freq_core_eff_acc[i][j]	+=
							pMsMeasurementCpu->msr_freq_core_eff_cur[i][j] * pMsMeasurementCpu->internal.msr_time_diff_double;
						
						pMsMeasurementCpu->internal.msr_aperf_core_temp[i][j]	= pMsMeasurementCpu->internal.msr_aperf_core_cur[i][j];
						pMsMeasurementCpu->internal.msr_mperf_core_temp[i][j]	= pMsMeasurementCpu->internal.msr_mperf_core_cur[i][j];
						
						if (msr_freq_cpu_eff_max < pMsMeasurementCpu->msr_freq_core_eff_cur[i][j]) {
							msr_freq_cpu_eff_max = pMsMeasurementCpu->msr_freq_core_eff_cur[i][j];
						}
					}
					
					pMsMeasurementCpu->msr_freq_cpu_eff_cur[i]	= msr_freq_cpu_eff_max;
				}
				
				// result: voltage2freq
				for (int i=0; i<CPUS; ++i) {
					pMsMeasurementCpu->msr_v2freq_acc[i]	+=
						pMsMeasurementCpu->msr_voltage_cur[i] * pMsMeasurementCpu->msr_voltage_cur[i] *
						pMsMeasurementCpu->msr_freq_cpu_eff_cur[i] *
						pMsMeasurementCpu->internal.msr_time_diff_double;
					pMsMeasurementCpu->msr_pstate_acc[i]	+=
						pMsMeasurementCpu->msr_pstate_cur[i] *
						pMsMeasurementCpu->internal.msr_time_diff_double;
				}
			}
			
			// result: energy consumption
			for (int i=0; i<CPUS; ++i) {
				for (int j=0; j<3; ++j) {
					if (pMsMeasurementCpu->msr_energy_cur[i][j] >= pMsMeasurementCpu->internal.msr_energy_temp[i][j]) {
						pMsMeasurementCpu->msr_energy_acc[i][j]	+=
							pMsMeasurementCpu->msr_energy_cur[i][j] - pMsMeasurementCpu->internal.msr_energy_temp[i][j];
					} else {
						pMsMeasurementCpu->msr_energy_acc[i][j]	+=
							pMsMeasurementCpu->internal.msr_energy_max + (pMsMeasurementCpu->msr_energy_cur[i][j] - pMsMeasurementCpu->internal.msr_energy_temp[i][j]);
					}
					
					pMsMeasurementCpu->msr_power_cur[i][j]	=
						(pMsMeasurementCpu->msr_energy_cur[i][j] - pMsMeasurementCpu->internal.msr_energy_temp[i][j])/
						pMsMeasurementCpu->internal.msr_time_diff_double;
					
					pMsMeasurementCpu->internal.msr_energy_temp[i][j]	= pMsMeasurementCpu->msr_energy_cur[i][j];
				}
			}
			
			if(TVariant == VARIANT_FULL) {
				// result: maximum temperatures
				for (int i=0; i<CPUS; ++i) {
					pMsMeasurementCpu->msr_temperature_pkg_max[i]	=
						(pMsMeasurementCpu->msr_temperature_pkg_cur[i] > pMsMeasurementCpu->msr_temperature_pkg_max[i]) ?
						pMsMeasurementCpu->msr_temperature_pkg_cur[i] : pMsMeasurementCpu->msr_temperature_pkg_max[i];
					for (int j=0; j<CORES; ++j) {
						pMsMeasurementCpu->msr_temperature_core_max[i][j]	=
							(pMsMeasurementCpu->msr_temperature_core_cur[i][j] > pMsMeasurementCpu->msr_temperature_core_max[i][j]) ?
							pMsMeasurementCpu->msr_temperature_core_cur[i][j] : pMsMeasurementCpu->msr_temperature_core_max[i][j];
					}
				}
			}
			
			// result: utilization
			for (int i=0; i<CPUSTATS; ++i) {
				pMsMeasurementCpu->measure_util_cur[i]	=
					pMsMeasurementCpu->internal.measure_util_cur[i] - pMsMeasurementCpu->internal.measure_util_temp[i];
				
				pMsMeasurementCpu->measure_util_acc[i]	+= pMsMeasurementCpu->measure_util_cur[i];
				
				pMsMeasurementCpu->internal.measure_util_temp[i]	= pMsMeasurementCpu->internal.measure_util_cur[i];
				
				pMsMeasurementCpu->measure_util_active_cur = ((double)(
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_USER] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_NICE] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_SYSTEM] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_IRQ] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_SOFTIRQ] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_STEAL] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_GUEST]) /
					(double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
				pMsMeasurementCpu->measure_util_idle_cur = ((double)(
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_IDLE] +
					pMsMeasurementCpu->measure_util_cur[CPUSTAT_IOWAIT]) /
					(double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
					
				pMsMeasurementCpu->measure_util_avg_cur =
					(pMsMeasurementCpu->measure_util_active_cur * 100.0) /
					(pMsMeasurementCpu->measure_util_active_cur + pMsMeasurementCpu->measure_util_idle_cur);
			}
			
			if(TVariant == VARIANT_FULL) {
				// result: memory usage
				pMsMeasurementCpu->measure_memory_used_max	=
					(pMsMeasurementCpu->measure_memory_cur[CPU_MEM_RAM_USED]>pMsMeasurementCpu->measure_memory_used_max) ?
					pMsMeasurementCpu->measure_memory_cur[CPU_MEM_RAM_USED] : pMsMeasurementCpu->measure_memory_used_max;
					
				pMsMeasurementCpu->measure_memory_free_max	=
					(pMsMeasurementCpu->measure_memory_cur[CPU_MEM_RAM_FREE]>pMsMeasurementCpu->measure_memory_free_max) ?
					pMsMeasurementCpu->measure_memory_cur[CPU_MEM_RAM_FREE] : pMsMeasurementCpu->measure_memory_free_max;
					
				// result: swap usage
				pMsMeasurementCpu->measure_swap_used_max	=
					(pMsMeasurementCpu->measure_memory_cur[CPU_MEM_SWAP_USED]>pMsMeasurementCpu->measure_swap_used_max) ?
					pMsMeasurementCpu->measure_memory_cur[CPU_MEM_SWAP_USED] : pMsMeasurementCpu->measure_swap_used_max;
					
				pMsMeasurementCpu->measure_swap_free_max	=
					(pMsMeasurementCpu->measure_memory_cur[CPU_MEM_SWAP_FREE]>pMsMeasurementCpu->measure_swap_free_max) ?
					pMsMeasurementCpu->measure_memory_cur[CPU_MEM_SWAP_FREE] : pMsMeasurementCpu->measure_swap_free_max;
			}
			
#if 0
			mrLog.lock();
			mrLog() << "t_cpu0core0: " << pMsMeasurementCpu->msr_temperature_core_cur[0][0] << std::endl;
			mrLog() << "t_cpu0: " << pMsMeasurementCpu->msr_temperature_pkg_cur[0] << ", t_cpu1: " << pMsMeasurementCpu->msr_temperature_pkg_cur[1] << std::endl;
			mrLog() << "e_cpu0PKG: " << pMsMeasurementCpu->msr_energy_cur[0][PKG] << ", e_cpu0PP0: " << pMsMeasurementCpu->msr_energy_cur[0][PP0] << ", e_cpu0DRAM: " << pMsMeasurementCpu->msr_energy_cur[0][DRAM] <<std::endl;
			mrLog.unlock();
#endif
		}
		
		// result: average power consumption
		for (int i=0; i<CPUS; ++i) {
			for (int j=0; j<3; ++j) {
				pMsMeasurementCpu->msr_power_avg[i][j]		=
					pMsMeasurementCpu->msr_energy_acc[i][j] / pMsMeasurementCpu->msr_time_runtime;
			}
		}
		
		if(TVariant == VARIANT_FULL) {
			// result: average frequency
			for (int i=0; i<CPUS; ++i) {
				for (int k=0; k<CORES; ++k) {
					pMsMeasurementCpu->msr_freq_core_avg[i][k]	=
						pMsMeasurementCpu->msr_freq_core_acc[i][k] / pMsMeasurementCpu->msr_time_runtime;
				}
			}
			
			// result: effective frequency
			for (int i=0; i<CPUS; ++i) {
				for (int k=0; k<CORES; ++k) {
					pMsMeasurementCpu->msr_freq_core_eff_avg[i][k]	=
						pMsMeasurementCpu->msr_freq_core_eff_acc[i][k] / pMsMeasurementCpu->msr_time_runtime;
				}
			}
		}
		
		// result average utilization
		pMsMeasurementCpu->measure_util_active_total =
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_USER] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_NICE] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_SYSTEM] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_IRQ] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_SOFTIRQ] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_STEAL] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_GUEST];
		pMsMeasurementCpu->measure_util_idle_total =
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_IDLE] +
			pMsMeasurementCpu->measure_util_acc[CPUSTAT_IOWAIT];
		
		pMsMeasurementCpu->measure_util_active_avg =
			((double)pMsMeasurementCpu->measure_util_active_total / (double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
		pMsMeasurementCpu->measure_util_idle_avg =
			((double)pMsMeasurementCpu->measure_util_idle_total / (double)sysconf(_SC_CLK_TCK)) / (CPUS * CORES);
		
		uint64_t util_time_total =
			pMsMeasurementCpu->measure_util_active_total +
			pMsMeasurementCpu->measure_util_idle_total;
		uint64_t util_time_idle =
			pMsMeasurementCpu->measure_util_idle_total;
		
		pMsMeasurementCpu->measure_util_avg =
			((util_time_total - util_time_idle) * 100.0) / util_time_total;
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'msr thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period                 : " << pMsMeasurementCpu->msr_time_runtime << " s" << std::endl;
		for (int i=0; i<CPUS; ++i) {
			mrLog()
			<< "     energy cpu " << i << " domain pkg  acc: " << pMsMeasurementCpu->msr_energy_acc[i][PKG] << " mWs" << std::endl
			<< "     energy cpu " << i << " domain pp0  acc: " << pMsMeasurementCpu->msr_energy_acc[i][PP0] << " mWs" << std::endl
			<< "     energy cpu " << i << " domain dram acc: " << pMsMeasurementCpu->msr_energy_acc[i][DRAM] << " mWs" << std::endl
			<< "     power  cpu " << i << " domain pkg  avg: " << pMsMeasurementCpu->msr_power_avg[i][PKG] << " mW" << std::endl
			<< "     power  cpu " << i << " domain pp0  avg: " << pMsMeasurementCpu->msr_power_avg[i][PP0] << " mW" << std::endl
			<< "     power  cpu " << i << " domain dram avg: " << pMsMeasurementCpu->msr_power_avg[i][DRAM] << " mW" << std::endl
			<< "     temp   cpu " << i << " pkg         max: " << pMsMeasurementCpu->msr_temperature_pkg_max[i] << " \u00b0C" << std::endl;
			for (int j=0; j<CORES; ++j) {
				mrLog()
				<< "     temp   cpu " << i << " core " << j << "      max: " << pMsMeasurementCpu->msr_temperature_core_max[i][j] << " \u00b0C" << std::endl
				<< "     freq   cpu " << i << " core " << j << "      avg: " << pMsMeasurementCpu->msr_freq_core_avg[i][j] << " MHz" <<
				std::endl
				<< "     freq   cpu " << i << " core " << j << "      eff: " << pMsMeasurementCpu->msr_freq_core_eff_avg[i][j] << " MHz" <<
				std::endl;
			}
			mrLog()
			<< "     util   cpu all           avg: " << pMsMeasurementCpu->measure_util_avg << " %" << std::endl;
		}
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}
