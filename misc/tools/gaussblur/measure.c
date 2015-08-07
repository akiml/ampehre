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
 */

#include "measure.h"

void init_measuring_system(ARGUMENTS *settings, MSYSTEM **ms, MEASUREMENT **m) {
	// Initialize library and measuring system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	*ms	= ms_init(&version, settings->cpu_gov, settings->cpu_freq_min, settings->cpu_freq_max, settings->gpu_freq);
	
	// Allocate and initialize measurement structs
	*m	= ms_alloc_measurement();
	
	// Set timer for measurement m
	ms_set_timer(*m, CPU   , settings->sample_rate_cpu /1000, (settings->sample_rate_cpu %1000) * 1000000);
	ms_set_timer(*m, GPU   , settings->sample_rate_gpu /1000, (settings->sample_rate_gpu %1000) * 1000000);
	ms_set_timer(*m, FPGA  , settings->sample_rate_fpga/1000, (settings->sample_rate_fpga%1000) * 1000000);
	ms_set_timer(*m, SYSTEM, settings->sample_rate_sys /1000, (settings->sample_rate_sys %1000) * 1000000);
	
	ms_init_measurement(*ms, *m, CPU|GPU|FPGA);
}

void start_measuring_system(MSYSTEM *ms, MEASUREMENT *m) {
	// Start measuring system
	ms_start_measurement(ms, m);
}

void stop_measuring_system(MSYSTEM *ms, MEASUREMENT *m) {
	// Stop all measuring procedures
	ms_stop_measurement(ms, m);
	
	// Join measurement threads and remove thread objects
	ms_join_measurement(ms, m);
	ms_fini_measurement(ms, m);
}

void fini_measuring_system(MSYSTEM **ms, MEASUREMENT **m) {
	// Cleanup the environment before exiting the program
	ms_free_measurement(*m);
	ms_fini(*ms);
	
	*ms	= NULL;
	*m	= NULL;
}
