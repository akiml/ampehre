/*
 * hettime.h
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
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.0 - MIC integration into libmeasure
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.6.1 - add json printer to hettime
 *          0.7.0 - modularized measurement struct
 *          0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 *          0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#ifndef __HETTIME_H__
#define __HETTIME_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>

#include "../../include/ms_measurement.h"

#define LOG_ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)
#define LOG_WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)

typedef struct __exec_time {
	struct timespec time_start;
	struct timespec time_stop;
	struct timespec time_diff;
	double exec_time_diff;
	struct tms time_start_tms;
	struct tms time_stop_tms;
	clock_t start;
	clock_t stop;
	double real;
	double user;
	double sys;
} EXEC_TIME;

typedef struct __arguments {
	char *child_filename;
	uint32_t child_num_of_args;
	char **child_args;
	char *csv_filename;
	char *json_filename;
	char *ostream_filename;
	int force_idle_fpga;
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_sys;
	uint32_t sample_rate_mic;
	uint32_t check_for_exit_interrupts_cpu;
	uint32_t check_for_exit_interrupts_gpu;
	uint32_t check_for_exit_interrupts_fpga;
	uint32_t check_for_exit_interrupts_sys;
	uint32_t check_for_exit_interrupts_mic;
	enum gpu_frequency gpu_freq;
	enum cpu_governor cpu_gov;
	uint64_t cpu_freq_min;
	uint64_t cpu_freq_max;
	int ush_client;
	enum ipmi_timeout_setting timeout_setting;
	enum skip_ms_rate skip_ms;
	enum lib_variant variant;
} ARGUMENTS;

int run(ARGUMENTS *settings);

void alloc_exec_time(EXEC_TIME **time);
void free_exec_time(EXEC_TIME **time);
void get_current_time(struct timespec *time_cur);
void calc_exec_time_diff(EXEC_TIME *time);

#endif /* __HETTIME_H__ */
