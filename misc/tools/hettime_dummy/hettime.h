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
 * created: 10/13/14
 * version: 0.1.17 - add a hettime dummy executable for some hetsched measurements
 *          0.1.18 - hettime dummy tool can be used similar to ordinary "sleep" command
 */

#ifndef __HETTIME_H__
#define __HETTIME_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "../../../include/measurement.h"

#define LOG_ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)
#define LOG_WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)

typedef struct __arguments {
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_sys;
	enum gpu_frequency gpu_freq;
	enum cpu_governor cpu_gov;
	uint32_t cpu_freq_min;
	uint32_t cpu_freq_max;
	uint32_t runtime;
	uint64_t ipmi_timeout_setting;
} ARGUMENTS;

void run(ARGUMENTS *settings);

void print_ostream(FILE *file, ARGUMENTS* settings, MEASUREMENT *m);

#endif /* __HETTIME_H__ */
