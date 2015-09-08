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
 * created: 10/27/14
 * version: 0.1.19 - add a hettime based idle power measurement tool
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "hetpowidle.h"
#include "list.h"
#include "printer.h"

static void exec_dummy_app(ARGUMENTS *settings, MSYSTEM *ms, MEASUREMENT *m);
static void init_measuring_system(ARGUMENTS *settings, MSYSTEM **ms, MEASUREMENT **m);
static void start_measuring_system(MSYSTEM *ms, MEASUREMENT *m);
static void stop_measuring_system(MSYSTEM *ms, MEASUREMENT *m);
static void fini_measuring_system(MSYSTEM **ms, MEASUREMENT **m);
static void print(ARGUMENTS *settings, IDLE_POWER *ip_list, IDLE_POWER *ip_median);

void run(ARGUMENTS *settings) {
	MSYSTEM *ms				= NULL;
	MEASUREMENT *m			= NULL;
	
	struct timespec time_sleep;
	time_sleep.tv_sec		= 20;
	time_sleep.tv_nsec		= 0;
	
	IDLE_POWER *ip_list		= NULL;
	IDLE_POWER *ip_median	= NULL;
	
	int i;
	for (i=0; i<MEASURE_IDLE_ITERATIONS; ++i) {
		init_measuring_system(settings, &ms, &m);
		
		fprintf(stderr, "||| 'hetpowidle': iteration %i of %i, run phase\n", i+1, MEASURE_IDLE_ITERATIONS);
		
		exec_dummy_app(settings, ms, m);
		append_list_create_element(&ip_list, settings, m);
		
		fini_measuring_system(&ms, &m);
		
		fprintf(stderr, "||| 'hetpowidle': iteration %i of %i, idle phase\n", i+1, MEASURE_IDLE_ITERATIONS);
		
		nanosleep(&time_sleep, NULL);
	}
	
	ip_median = get_median(&ip_list, settings);
	
	print(settings, ip_list, ip_median);
	
	delete_list(&ip_list);
}

static void init_measuring_system(ARGUMENTS *settings, MSYSTEM **ms, MEASUREMENT **m) {
	// Initialize library and measuring system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	*ms	= ms_init(&version, settings->cpu_gov, settings->cpu_freq_min, settings->cpu_freq_max, settings->gpu_freq);
	
	// Allocate and initialize measurement structs
	*m	= ms_alloc_measurement();
	
	uint32_t active_measures = 0;
	
	// Set timer for measurement m
	if (settings->idle_measurements & MEASURE_IDLE_CPU) {
		ms_set_timer(*m, CPU   , settings->sample_rate_cpu /1000, (settings->sample_rate_cpu %1000) * 1000000);
		active_measures |= CPU;
	}
	if (settings->idle_measurements & MEASURE_IDLE_GPU) {
		ms_set_timer(*m, GPU   , settings->sample_rate_gpu /1000, (settings->sample_rate_gpu %1000) * 1000000);
		active_measures |= GPU;
	}
	if (settings->idle_measurements & MEASURE_IDLE_FPGA) {
		ms_set_timer(*m, FPGA  , settings->sample_rate_fpga/1000, (settings->sample_rate_fpga%1000) * 1000000);
		active_measures |= FPGA;
	}
	if (settings->idle_measurements & MEASURE_IDLE_MIC) {
		ms_set_timer(*m, MIC   , settings->sample_rate_mic /1000, (settings->sample_rate_mic %1000) * 1000000);
		active_measures |= MIC;
	}
	if (settings->idle_measurements & MEASURE_IDLE_SYS) {
		ms_set_timer(*m, SYSTEM, settings->sample_rate_sys /1000, (settings->sample_rate_sys %1000) * 1000000);
		active_measures |= SYSTEM;
	}
	
	ms_init_measurement(*ms, *m, active_measures);
}

static void start_measuring_system(MSYSTEM *ms, MEASUREMENT *m) {
	// Start measuring system
	ms_start_measurement(ms, m);
}

static void stop_measuring_system(MSYSTEM *ms, MEASUREMENT *m) {
	// Stop all measuring procedures
	ms_stop_measurement(ms, m);
	
	// Join measurement threads and remove thread objects
	ms_join_measurement(ms, m);
	ms_fini_measurement(ms, m);
}

static void fini_measuring_system(MSYSTEM **ms, MEASUREMENT **m) {
	// Cleanup the environment before exiting the program
	ms_free_measurement(*m);
	ms_fini(*ms);
	
	*ms	= NULL;
	*m	= NULL;
}

static void exec_dummy_app(ARGUMENTS *settings, MSYSTEM *ms, MEASUREMENT *m) {
	start_measuring_system(ms, m);
	fprintf(stderr, ">>> 'hetpowidle': measuring system initialized.\n");
	
	struct timespec time_sleep;
	time_sleep.tv_sec	= 10;
	time_sleep.tv_nsec	= 0;
	
	nanosleep(&time_sleep, NULL);
	
	stop_measuring_system(ms, m);
	fprintf(stderr, "<<< 'hetpowidle': measuring system stopped.\n");
}

static void print(ARGUMENTS *settings, IDLE_POWER *ip_list, IDLE_POWER *ip_median) {
	fprintf(stderr, "||| 'hetpowidle': writing json file '%s'...", settings->jsonfile);
	fflush(stderr);
	
	FILE *file = fopen(settings->jsonfile, "w");
	if (NULL == file) {
		LOG_ERROR("Cannot open json file.");
		exit(EXIT_FAILURE);
	}
	
	print_json(file, settings, ip_list, ip_median);
	
	fclose(file);
	
	fprintf(stderr, " done!\n");
	fflush(stderr);
}
