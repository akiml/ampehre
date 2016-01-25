/*
 * ms_taskwrapper.h
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
 * created: 8/05/14
 * version: 0.1.0 - initial implementation
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.7.0 - modularized measurement struct
 */

#ifndef __MS_TASKWRAPPER_H__
#define __MS_TASKWRAPPER_H__

#include "ms_measurement.h"

// Please define the relevant measurements in this struct!
typedef struct __mrel_measurement {
	// CPU
	double acc_cpu0_energy_total_pkg;
	double acc_cpu0_energy_total_dram;
	double acc_cpu1_energy_total_pkg;
	double acc_cpu1_energy_total_dram;
	
	double cur_cpu0_energy_total_pkg;
	double cur_cpu0_energy_total_dram;
	double cur_cpu1_energy_total_pkg;
	double cur_cpu1_energy_total_dram;
	
	// GPU
	double acc_gpu_energy_total;
	
	double cur_gpu_energy_total;
	
	// FPGA
	double acc_fpga_energy_total;
	
	double cur_fpga_energy_total;
	
	// MIC
	double acc_mic_energy_total;
	
	double cur_mic_energy_total;
	
	// System
	double acc_sysboard_energy_total;
	double acc_server_energy_total;
	
	double cur_sysboard_energy_total;
	double cur_server_energy_total;
} MREL_MEASUREMENT;

// This struct is the container for task related measurements
typedef struct __mtask {
	int resources;
	int measurements_per_resource;
	MREL_MEASUREMENT **rm;
} MTASK;

/*
 * Init measuring system. Call the function like this (if you want to measure CPU, GPU and FPGA)
 * mstw_init(CPU | GPU | FPGA, CPU_GOVERNOR_ONDEMAND, 2000, 2500, GPU_FREQUENCY_CUR);
 */
void mstw_init(MS_VERSION *version, int resources, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
			   enum gpu_frequency gpu_freq, enum ipmi_timeout_setting timeout_setting, enum skip_ms_rate skip_ms, enum lib_variant variant);
// Start measuring system
void mstw_start(void);
// Stop measuring system
void mstw_stop(void);
// Clean the measuring environment
void mstw_free(void);

// Install signal handler for SIGUSR1 (measuring system start)
void mstw_reg_sighandler_start(void(*signal_handler)(int));
// Install signal handler for SIGUSR2 (measuring system stop)
void mstw_reg_sighandler_stop(void(*signal_handler)(int));

/*
 * Register a task for measuring
 * resources are something like CPU | GPU | FPGA
 * measurements_per_resource should be 3 if want to measure init, compute, and free phases
 */
void mstw_reg_task(MTASK **mtask, int measurements_per_resource);
// Unregister a task for measuring
void mstw_unreg_task(MTASK **mtask);

// Start measuring a single task. The results are stored resource-wise and phase-wise.
void mstw_start_measurement(MTASK *mtask, int resource, int phase);
// Stop measuring a single task.
void mstw_stop_measurement(MTASK *mtask, int resource, int phase);

#endif /* __MS_TASKWRAPPER_H__*/
