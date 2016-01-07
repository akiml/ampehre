/*
 * measure.c
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
 * created: 1/11/15
 * version: 0.2.5 - add gaussblur example application
 *          0.7.0 - modularised measurement struct
 */

#include "measure.h"

void init_measuring_system(ARGUMENTS *settings, MS_SYSTEM **ms, MS_LIST **m) {
	// Initialize library and measuring system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	*ms	= ms_init(&version, settings->cpu_gov, settings->cpu_freq_min, settings->cpu_freq_max, settings->gpu_freq, settings->ipmi_timeout_setting, settings->skip_ms, settings->variant);
	
	// Allocate and initialize measurement structs
	*m	= ms_alloc_measurement(*ms);
	
	// Set timer for measurement m
	ms_set_timer(*m, CPU   , settings->sample_rate_cpu /1000, (settings->sample_rate_cpu %1000) * 1000000, settings->check_for_exit_interrupts_cpu);
	ms_set_timer(*m, GPU   , settings->sample_rate_gpu /1000, (settings->sample_rate_gpu %1000) * 1000000, settings->check_for_exit_interrupts_gpu);
	ms_set_timer(*m, FPGA  , settings->sample_rate_fpga/1000, (settings->sample_rate_fpga%1000) * 1000000, settings->check_for_exit_interrupts_fpga);
	ms_set_timer(*m, SYSTEM, settings->sample_rate_sys /1000, (settings->sample_rate_sys %1000) * 1000000, settings->check_for_exit_interrupts_sys);
	
	ms_init_measurement(*ms, *m, CPU|GPU|FPGA);
}

void start_measuring_system(MS_SYSTEM *ms) {
	// Start measuring system
	ms_start_measurement(ms);
}

void stop_measuring_system(MS_SYSTEM *ms) {
	// Stop all measuring procedures
	ms_stop_measurement(ms);
	
	// Join measurement threads and remove thread objects
	ms_join_measurement(ms);
	ms_fini_measurement(ms);
}

void fini_measuring_system(MS_SYSTEM **ms, MS_LIST **m) {
	// Cleanup the environment before exiting the program
	ms_free_measurement(*m);
	ms_fini(*ms);
	
	*ms	= NULL;
	*m	= NULL;
}
