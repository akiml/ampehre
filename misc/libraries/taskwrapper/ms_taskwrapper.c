/*
 * ms_taskwrapper.c
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
 *          0.7.0 - modularised measurement struct
 */

#include "ms_taskwrapper_internal.h"

#include <stdio.h>

static MINTERNAL *minternal = NULL;

void mstw_init(MS_VERSION *version, int resources, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
			   enum gpu_frequency gpu_freq, uint64_t ipmi_timeout_setting, enum skip_ms_rate skip_ms, enum lib_variant variant) {
	
	if((version->major != MS_MAJOR_VERSION) || (version->minor != MS_MINOR_VERSION) || (version->revision != MS_REVISION_VERSION)){
		printf("Error in taskwrapper: Wrong version number! taskwrapper version %d.%d.%d is called from tool with version %d.%d.%d.\n", MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, version->major, version->minor, version->revision);
		exit(EXIT_FAILURE);
	}
	
	if (NULL != minternal) {
		LOG_WARN("Measuring system already initialized.");
		return;
	}
	
	minternal = malloc(sizeof(MINTERNAL));
	if (NULL == minternal) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	// Please set the default sampling rates here (in ms)
	minternal->sample_rate_cpu	=  30;
	minternal->sample_rate_gpu	=  40;
	minternal->sample_rate_fpga	=  50;
	minternal->sample_rate_mic	= 100;
	minternal->sample_rate_sys	= 100;
	
	// Please set the default number of skipped measurement points
	minternal->check_for_exit_interrupts_cpu	= 1;
	minternal->check_for_exit_interrupts_gpu	= 1;
	minternal->check_for_exit_interrupts_fpga	= 1;
	minternal->check_for_exit_interrupts_mic	= 1;
	minternal->check_for_exit_interrupts_sys	= 1;
	
	minternal->resources		= resources;
	
	// Initialize library and measuring system
	minternal->global_ms		= ms_init(version, cpu_gov, cpu_freq_min, cpu_freq_max, gpu_freq, ipmi_timeout_setting, skip_ms, variant);
	
	// Forcing FPGA to idle if desired
	
	if(variant == VARIANT_FULL){
		ms_init_fpga_force_idle(minternal->global_ms);
	}
	
	// Allocate and initialize measurement structs
	minternal->global_m		= ms_alloc_measurement(minternal->global_ms);
	
	// Set timer for measurement m
	ms_set_timer(minternal->global_m, CPU   , minternal->sample_rate_cpu /1000, (minternal->sample_rate_cpu %1000) * 1000000, minternal->check_for_exit_interrupts_cpu);
	ms_set_timer(minternal->global_m, GPU   , minternal->sample_rate_gpu /1000, (minternal->sample_rate_gpu %1000) * 1000000, minternal->check_for_exit_interrupts_gpu);
	ms_set_timer(minternal->global_m, FPGA  , minternal->sample_rate_fpga/1000, (minternal->sample_rate_fpga%1000) * 1000000, minternal->check_for_exit_interrupts_fpga);
	ms_set_timer(minternal->global_m, MIC   , minternal->sample_rate_mic /1000, (minternal->sample_rate_mic %1000) * 1000000, minternal->check_for_exit_interrupts_mic);
	ms_set_timer(minternal->global_m, SYSTEM, minternal->sample_rate_sys /1000, (minternal->sample_rate_sys %1000) * 1000000, minternal->check_for_exit_interrupts_sys);
	ms_init_measurement(minternal->global_ms, minternal->global_m, resources);
}

void mstw_free(void) {
	// Cleanup the environment before exiting the program
	ms_free_measurement(minternal->global_m);
	ms_fini(minternal->global_ms);
	free(minternal);
	
	minternal = NULL;
}

void mstw_start(void) {
	// Start measuring system
	ms_start_measurement(minternal->global_ms);
}

void mstw_stop(void) {
	// Stop all measuring procedures
	ms_stop_measurement(minternal->global_ms);
	
	// Join measurement threads and remove thread objects
	ms_join_measurement(minternal->global_ms);
	ms_fini_measurement(minternal->global_ms);
}

void mstw_reg_sighandler_start(void(*signal_handler)(int)) {
	ms_reg_sighandler_start(minternal->global_ms, signal_handler);
}

void mstw_reg_sighandler_stop(void(*signal_handler)(int)) {
	ms_reg_sighandler_stop(minternal->global_ms, signal_handler);
}

void mstw_reg_task(MTASK **mtask, int measurements_per_resource) {
	*mtask = malloc(sizeof(MTASK));
	if (NULL == *mtask) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	(*mtask)->measurements_per_resource	= measurements_per_resource;
	
	(*mtask)->rm = malloc(NUM_OF_RESOURCES * sizeof(MREL_MEASUREMENT *));
	if (NULL == (*mtask)->rm) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	int i, j;
	for (i=0; i<NUM_OF_RESOURCES; ++i) {
		if ((minternal->resources >> i) & 0x1) {
			(*mtask)->rm[i] = malloc(measurements_per_resource * sizeof(MREL_MEASUREMENT));
			if (NULL == (*mtask)->rm[i]) {
				LOG_ERROR("Cannot allocate memory.");
				exit(EXIT_FAILURE);
			}
			
			for (j=0; j<measurements_per_resource; ++j) {
				mstw_init_rel_measurement(&((*mtask)->rm[i][j]));
			}
		} else {
			(*mtask)->rm[i] = NULL;
		}
	}
}

void mstw_unreg_task(MTASK **mtask) {
	int i;
	for (i=0; i<NUM_OF_RESOURCES; ++i) {
		if (NULL != (*mtask)->rm[i]) {
			free((*mtask)->rm[i]);
		}
	}
	free((*mtask)->rm);
	
	free(*mtask);
	
	*mtask = NULL;
}

void mstw_start_measurement(MTASK *mtask, int resource, int phase) {
	MREL_MEASUREMENT *m = NULL;
	
	m = mstw_get_rel_measurement(mtask, resource, phase);
	
	mstw_capture_start_measure(minternal, m);
}

void mstw_stop_measurement(MTASK *mtask, int resource, int phase) {
	MREL_MEASUREMENT *m = NULL;
	
	m = mstw_get_rel_measurement(mtask, resource, phase);
	
	mstw_capture_stop_measure(minternal, m);
}
