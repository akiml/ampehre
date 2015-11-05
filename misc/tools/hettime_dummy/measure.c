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
 * created: 10/13/14
 * version: 0.1.17 - add a hettime dummy executable for some hetsched measurements
 *          0.1.18 - hettime dummy tool can be used similar to ordinary "sleep" command
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

#include "hettime.h"

void signal_int_handler(int signal_number);

static void exec_dummy_app(ARGUMENTS *settings, MSYSTEM *ms, MEASUREMENT *m);
static void init_measuring_system(ARGUMENTS *settings, MSYSTEM **ms, MEASUREMENT **m);
static void start_measuring_system(MSYSTEM *ms, MEASUREMENT *m);
static void stop_measuring_system(MSYSTEM *ms, MEASUREMENT *m);
static void fini_measuring_system(MSYSTEM **ms, MEASUREMENT **m);
static void print(ARGUMENTS *settings, MEASUREMENT *m);

static pthread_mutex_t mutex;

void signal_int_handler(int signal_number) {
	if (signal_number == SIGINT) {
		fprintf(stderr, "||| 'hettime dummy': signal SIGINT catched.\n");
		pthread_mutex_unlock(&mutex);
	}
}

void run(ARGUMENTS *settings) {
	MSYSTEM *ms			= NULL;
	MEASUREMENT *m		= NULL;
	
	if (settings->runtime == 0) {
		signal(SIGINT, &signal_int_handler);
		
		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_lock(&mutex);
	}
	
	init_measuring_system(settings, &ms, &m);
	
	exec_dummy_app(settings, ms, m);
	
	print(settings, m);
	
	fini_measuring_system(&ms, &m);
	
	if (settings->runtime == 0) {
		pthread_mutex_destroy(&mutex);
	}
}

static void init_measuring_system(ARGUMENTS *settings, MSYSTEM **ms, MEASUREMENT **m) {
	// Initialize library and measuring system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	*ms	= ms_init(&version, settings->cpu_gov, settings->cpu_freq_min, settings->cpu_freq_max, settings->gpu_freq, settings->ipmi_timeout_setting);
	
	// Allocate and initialize measurement structs
	*m	= ms_alloc_measurement();
	
	// Set timer for measurement m
	ms_set_timer(*m, CPU   , settings->sample_rate_cpu /1000, (settings->sample_rate_cpu %1000) * 1000000, settings->skip_ms_rate_cpu);
	ms_set_timer(*m, GPU   , settings->sample_rate_gpu /1000, (settings->sample_rate_gpu %1000) * 1000000, settings->skip_ms_rate_gpu);
	ms_set_timer(*m, FPGA  , settings->sample_rate_fpga/1000, (settings->sample_rate_fpga%1000) * 1000000, settings->skip_ms_rate_fpga);
	ms_set_timer(*m, SYSTEM, settings->sample_rate_sys /1000, (settings->sample_rate_sys %1000) * 1000000, settings->skip_ms_rate_sys);
	ms_init_measurement(*ms, *m, ALL);
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
	fprintf(stderr, ">>> 'hettime dummy': measuring system initialized.\n");
	
	if (settings->runtime == 0) {
		pthread_mutex_lock(&mutex);
		pthread_mutex_unlock(&mutex);
	} else {
		struct timespec time_sleep;
		time_sleep.tv_sec	=  settings->runtime/1000;
		time_sleep.tv_nsec	= (settings->runtime%1000)*1000000;
		
		fprintf(stderr, "    'hettime dummy': will sleep for %.3lf s.\n", settings->runtime/1000.0);
		
		if (nanosleep(&time_sleep, NULL)) {
			LOG_WARN("Couldn't finish nanosleep successfully.");
		}
	}
	
	stop_measuring_system(ms, m);
	fprintf(stderr, "<<< 'hettime dummy':  measuring system stopped.\n");
}

static void print(ARGUMENTS *settings, MEASUREMENT *m) {
	print_ostream(stdout, settings, m);
}
