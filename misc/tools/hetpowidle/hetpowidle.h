/*
 * hetpowidle.h
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
 */

#ifndef __HETPOWIDLE_H__
#define __HETPOWIDLE_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../../include/measurement.h"

#define LOG_ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)
#define LOG_WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)

#define MEASURE_IDLE_NOTHING	0x0
#define MEASURE_IDLE_CPU		0x1
#define MEASURE_IDLE_GPU		0x2
#define MEASURE_IDLE_FPGA		0x4
#define MEASURE_IDLE_MIC		0x8
#define MEASURE_IDLE_SYS		0x10
#define MEASURE_IDLE_ALL		(MEASURE_IDLE_CPU | MEASURE_IDLE_GPU | MEASURE_IDLE_FPGA | MEASURE_IDLE_MIC | MEASURE_IDLE_SYS)

#define MEASURE_IDLE_ITERATIONS	11

typedef struct __arguments {
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_mic;
	uint32_t sample_rate_sys;
	uint32_t skip_ms_rate_cpu;
	uint32_t skip_ms_rate_gpu;
	uint32_t skip_ms_rate_fpga;
	uint32_t skip_ms_rate_sys;
	uint32_t skip_ms_rate_mic;
	enum gpu_frequency gpu_freq;
	enum cpu_governor cpu_gov;
	uint64_t ipmi_timeout_setting;
	uint32_t cpu_freq_min;
	uint32_t cpu_freq_max;
	enum skip_ms_freq skip_ms;
	enum lib_variant variant;
	char *hostname;
	size_t hostname_size;
	char *jsonfile;
	size_t jsonfile_size;
	uint32_t idle_measurements;
} ARGUMENTS;

void run(ARGUMENTS *settings);

#endif /* __HETPOWIDLE_H__ */
