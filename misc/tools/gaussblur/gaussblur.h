/*
 * gaussblur.h
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

#ifndef __GAUSSBLUR_H__
#define __GAUSSBLUR_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "../../../include/measurement.h"

#undef LOG_ERROR
#undef LOG_WARN

#define LOG_ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)
#define LOG_WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)

typedef struct __arguments {
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_sys;
	uint32_t skip_ms_rate_cpu;
	uint32_t skip_ms_rate_gpu;
	uint32_t skip_ms_rate_fpga;
	uint32_t skip_ms_rate_sys;
	enum gpu_frequency gpu_freq;
	enum cpu_governor cpu_gov;
	uint32_t cpu_freq_min;
	uint32_t cpu_freq_max;
	uint32_t checkpoints;
	uint32_t size;
	uint32_t energy_loops;
	uint64_t ipmi_timeout_setting;
} ARGUMENTS;

#define PIXEL_CHANNELS 4

typedef uint16_t DATA;

void run(ARGUMENTS *settings);

void print_ostream(FILE *file, ARGUMENTS* settings, MEASUREMENT *m);

#endif /* __GAUSSBLUR_H__ */
