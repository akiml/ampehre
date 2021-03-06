/*
 * ms_pythonwrapper.c
 * 
 * Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 7/02/17
 * version: 0.7.5 - add a python wrapper for simple measuring by python scripts
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "../../../include/ms_measurement.h"

MS_SYSTEM *ms	= NULL;
MS_LIST *ml		= NULL;

void ampehre_init(void) {
	// Initialize library and measurement system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	ms = ms_init(&version, CPU_GOVERNOR_ONDEMAND, 1500000, 2500000, GPU_FREQUENCY_CUR, IPMI_SET_TIMEOUT, SKIP_PERIODIC, VARIANT_FULL);
	
	// Allocate measurement list
	ml = ms_alloc_measurement(ms);
	
	// Set timer for ml. Measurements perform every (10ms/30ms)*10 = 100ms/300ms.
	ms_set_timer(ml, CPU   , 0,  30000000, 1);
	ms_set_timer(ml, GPU   , 0,  40000000, 1);
	ms_set_timer(ml, FPGA  , 0,  50000000, 1);
	ms_set_timer(ml, SYSTEM, 0, 100000000, 1);
	ms_set_timer(ml, MIC   , 0, 100000000, 1);
	ms_init_measurement(ms, ml, CPU|GPU|FPGA);
}

void ampehre_start(void) {
	// Start measurements
	ms_start_measurement(ms);
}

void ampehre_stop(void) {
	// Stop all measurement procedures.
	ms_stop_measurement(ms);
	
	// Join measurement threads and remove thread objects.
	ms_join_measurement(ms);
	ms_fini_measurement(ms);
}

void ampehre_fini(void) {
	// Cleanup the environment before exiting the program
	ms_free_measurement(ml);
	ms_fini(ms);
}

double ampehre_get_energy_total(void) {
	double e_total	= 0.0;
	
	// CPU 0
	e_total	+= cpu_energy_total_pkg(ml, 0) + cpu_energy_total_dram(ml, 0);
	// CPU 1
	e_total	+= cpu_energy_total_pkg(ml, 1) + cpu_energy_total_dram(ml, 1);
	// GPU
	e_total	+= gpu_energy_total(ml);
	// FPGA
	e_total	+= fpga_energy_total_power_usage(ml);
	
	return e_total;
}

double ampehre_get_energy_cpu(void) {
	double e_total	= 0.0;
	
	// CPU 0
	e_total	+= cpu_energy_total_pkg(ml, 0) + cpu_energy_total_dram(ml, 0);
	// CPU 1
	e_total	+= cpu_energy_total_pkg(ml, 1) + cpu_energy_total_dram(ml, 1);
	
	return e_total;
}

double ampehre_get_energy_gpu(void) {
	double e_total	= 0.0;
	
	// GPU
	e_total	+= gpu_energy_total(ml);
	
	return e_total;
}

double ampehre_get_energy_fpga(void) {
	double e_total	= 0.0;
	
	// FPGA
	e_total	+= fpga_energy_total_power_usage(ml);
	
	return e_total;
}

double ampehre_get_energy_mic(void) {
	double e_total	= 0.0;
	
	// MIC
	e_total	+= mic_energy_total_power_usage(ml);
	
	return e_total;
}

double ampehre_get_energy_sys(void) {
	double e_total	= 0.0;
	
	// SYSTEM
	e_total	+= system_energy_total(ml);
	
	return e_total;
}

#if 0
double ampehre_get_v2freq_total(void) {
	double v2freq	= 0.0;
	
	// CPU 0
	v2freq	+= cpu_v2freq_total(ml, 0);
	// CPU 1
	v2freq	+= cpu_v2freq_total(ml, 1);
	
	return v2freq;
}

uint64_t ampehre_get_pstate_total(void) {
	uint64_t pstate	= 0.0;
	
	// CPU 0
	pstate	+= cpu_pstate_total(ml, 0);
	// CPU 1
	pstate	+= cpu_pstate_total(ml, 1);
	
	return pstate;
}
#endif

