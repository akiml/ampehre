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
 * created: 7/28/14
 * version: 0.1.0 - initial implementation
 *          0.1.10 - add child execution time measurement
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.0 - MIC integration into libmeasure
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.6.1 - add json printer to hettime
 *          0.7.0 - modularized measurement struct
 *          0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 *          0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/times.h>

#include "hettime.h"
#include "printer.h"

#include "../../include/ushmsmonitor.h"

static int exec_app(ARGUMENTS *settings, MS_SYSTEM *ms, EXEC_TIME *time);
static void init_measuring_system(ARGUMENTS *settings, MS_SYSTEM **ms, MS_LIST **m);
static void start_measuring_system(MS_SYSTEM *ms);
static void stop_measuring_system(MS_SYSTEM *ms);
static void fini_measuring_system(MS_SYSTEM **ms, MS_LIST **m);
static void print(ARGUMENTS *settings, MS_LIST *m, EXEC_TIME *time);

int run(ARGUMENTS *settings) {
	int status_child	= 1;
	MS_SYSTEM *ms			= NULL;
	MS_LIST *m		= NULL;
	EXEC_TIME *time		= NULL;
	
	alloc_exec_time(&time);
	init_measuring_system(settings, &ms, &m);
	
	status_child = exec_app(settings, ms, time);
	
	calc_exec_time_diff(time);
	
	print(settings, m, time);
	
	fini_measuring_system(&ms, &m);
	free_exec_time(&time);
	
	return status_child;
}

static void init_measuring_system(ARGUMENTS *settings, MS_SYSTEM **ms, MS_LIST **m) {
	// Initialize library and measuring system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	*ms	= ms_init(&version, settings->cpu_gov, settings->cpu_freq_min, settings->cpu_freq_max, settings->gpu_freq, settings->timeout_setting, settings->skip_ms, settings->variant);
	if (NULL == *ms) {
		exit(1);
	}
	
	// Forcing FPGA to idle if desired
	if (settings->force_idle_fpga) {
		ms_init_fpga_force_idle(*ms);
	}
	
	// Allocate and initialize list of measurement structs
	*m	= ms_alloc_measurement(*ms);
	
	// Set timer for measurement m
	ms_set_timer(*m, CPU   , settings->sample_rate_cpu /1000, (settings->sample_rate_cpu %1000) * 1000000, settings->check_for_exit_interrupts_cpu);
	ms_set_timer(*m, GPU   , settings->sample_rate_gpu /1000, (settings->sample_rate_gpu %1000) * 1000000, settings->check_for_exit_interrupts_gpu);
	ms_set_timer(*m, FPGA  , settings->sample_rate_fpga/1000, (settings->sample_rate_fpga%1000) * 1000000, settings->check_for_exit_interrupts_fpga);
	ms_set_timer(*m, MIC   , settings->sample_rate_mic/1000, (settings->sample_rate_mic%1000) * 1000000, settings->check_for_exit_interrupts_mic);
	ms_set_timer(*m, SYSTEM, settings->sample_rate_sys /1000, (settings->sample_rate_sys %1000) * 1000000, settings->check_for_exit_interrupts_sys);
	ms_init_measurement(*ms, *m, ALL);
}

static void start_measuring_system(MS_SYSTEM *ms) {
	// Start measuring system
	ms_start_measurement(ms);
}

static void stop_measuring_system(MS_SYSTEM *ms) {
	// Stop all measuring procedures
	ms_stop_measurement(ms);
	
	// Join measurement threads and remove thread objects
	ms_join_measurement(ms);
	ms_fini_measurement(ms);
}

static void fini_measuring_system(MS_SYSTEM **ms, MS_LIST **m) {
	// Cleanup the environment before exiting the program
	ms_free_measurement(*m);
	ms_fini(*ms);
	
	*ms	= NULL;
	*m	= NULL;
}

static int exec_app(ARGUMENTS *settings, MS_SYSTEM *ms, EXEC_TIME *time) {
	pid_t new_pid, child_pid, parent_pid;
	int status				= 0;
	int status_child		= 1;
	USHMS_CLIENT *client	= NULL;
	
	sem_t *sync = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sync == MAP_FAILED) {
		LOG_ERROR("Cannot allocate shared memory.");
		exit(EXIT_FAILURE);
	}
	
	status = sem_init(sync, 1, 0);
	if (status < 0) {
		LOG_ERROR("Cannot init semaphore.");
		exit(EXIT_FAILURE);
	}
	
	if (settings->ush_client) {
		ushms_client_init(&client);
	}
	
	new_pid = fork();
	if (new_pid == 0) {
		// I am the child process.
		//sem_wait(sync);
		
		// Permanently drop priviledges
		if (seteuid(getuid()) != 0) {
			LOG_ERROR("Couldn't drop privileges before forking child.");
			exit(EXIT_FAILURE);
		}
		
		sem_wait(sync);
		fprintf(stderr, "=== CHILD'S STANDARD I/O STREAMS ===\n");
		time->start = times(&(time->time_start_tms));
		get_current_time(&(time->time_start));
		execvp(settings->child_filename, settings->child_args);
		
		// These lines of code should not been executed if execvp were successful!
		LOG_ERROR("Child couldn't be executed successfully.");
		exit(EXIT_FAILURE);
	} else if (new_pid > 0) {
		// I am the parent process.
		parent_pid	= getpid();
		child_pid	= new_pid;
		
		fprintf(stderr, ">>> 'hettime' parent PID: %i\n", parent_pid);
		fprintf(stderr, ">>> 'hettime' child  PID: %i\n", child_pid);
		
		if (settings->ush_client) {
			ushms_client_start(client);
			ushms_client_comm(client, 0, 1);
			ushms_client_stop(client);
		}
		
		start_measuring_system(ms);
		sem_post(sync);
		wait(&status_child);
		get_current_time(&(time->time_stop));
		time->stop = times(&(time->time_stop_tms));
		fprintf(stderr, "====================================\n");
		stop_measuring_system(ms);
		
		if (settings->ush_client) {
			ushms_client_start(client);
			ushms_client_comm(client, 0, 0);
			ushms_client_stop(client);
		}
		
		fprintf(stderr, "<<< 'hettime' child terminated with exit status %i.\n", status_child);
	} else {
		// Error: my_pid < 0
		LOG_ERROR("Cannot fork process.");
		exit(EXIT_FAILURE);
	}
	
	if (settings->ush_client) {
		ushms_client_fini(&client);
	}
	
	sem_destroy(sync);
	munmap(sync, sizeof(sem_t));
	
	return status_child;
}

static void print(ARGUMENTS *settings, MS_LIST *m, EXEC_TIME *time) {
	if (NULL != settings->csv_filename) {
		FILE *csv = fopen(settings->csv_filename, "a");
		if (NULL == csv) {
			LOG_ERROR("Cannot open file to write.");
			return;
		}
		
		print_csv(csv, settings, m, time);
		
		fflush(csv);
		fclose(csv);
	}
	
	if (NULL != settings->json_filename) {
		FILE *json = fopen(settings->json_filename, "w");
		if (NULL == json) {
			LOG_ERROR("Cannot open file to write.");
			return;
		}
		
		print_json(json, settings, m, time);
		
		fflush(json);
		fclose(json);
	}
	
	if (NULL == settings->ostream_filename) {
		print_ostream(stdout, settings, m, time);
	} else {
		FILE *file = fopen(settings->ostream_filename, "a");
		if (NULL == file) {
			LOG_ERROR("Cannot open file to write.");
			return;
		}
		
		print_ostream(file, settings, m, time);
		
		fflush(file);
		fclose(file);
	}
}
