/*
 * hettime.c
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 10/27/14
 * version: 0.1.19 - add a hettime based idle power measurement tool
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>

#include "hetpowidle.h"

int main(int argc, char **argv);
static void print_help(char **argv, ARGUMENTS *settings);
static void init_settings(ARGUMENTS **settings);
static void free_settings(ARGUMENTS **settings);

int main(int argc, char **argv) {
	ARGUMENTS *settings = NULL;
	init_settings(&settings);
	
	int32_t opt;
	size_t optarg_len;
	
	while ((opt = getopt(argc, argv, "h?cgfmsao:")) != -1) {
		switch (opt) {
			case 'c':
				settings->idle_measurements |= MEASURE_IDLE_CPU;
				break;
			case 'g':
				settings->idle_measurements |= MEASURE_IDLE_GPU;
				break;
			case 'f':
				settings->idle_measurements |= MEASURE_IDLE_FPGA;
				break;
			case 'm':
				settings->idle_measurements |= MEASURE_IDLE_MIC;
				break;
			case 's':
				settings->idle_measurements |= MEASURE_IDLE_CPU;
				settings->idle_measurements |= MEASURE_IDLE_GPU;
				settings->idle_measurements |= MEASURE_IDLE_FPGA;
				settings->idle_measurements |= MEASURE_IDLE_MIC;
				settings->idle_measurements |= MEASURE_IDLE_SYS;
				break;
			case 'a':
				settings->idle_measurements |= MEASURE_IDLE_ALL;
				break;
			case 'o':
				optarg_len = strlen(optarg);
				if (optarg_len > settings->jsonfile_size) {
					LOG_ERROR("Filename of result json file is too long.");
					free_settings(&settings);
					return EXIT_FAILURE;
				}
				memset(settings->jsonfile, 0, settings->jsonfile_size+1);
				strncpy(settings->jsonfile, optarg, settings->jsonfile_size);
				break;
			case '?':
			case 'h':
			default:
				print_help(argv, settings);
				free_settings(&settings);
				return EXIT_FAILURE;
		}
	}
	
	if (settings->sample_rate_cpu  <  20) {
		LOG_WARN("CPU sampling rate should be >= 20ms.");
	}
	if (settings->sample_rate_gpu  <  30) {
		LOG_WARN("GPU sampling rate should be >= 30ms.");
	}
	if (settings->sample_rate_fpga <  50) {
		LOG_WARN("FPGA sampling rate should be >= 50ms.");
	}
	if (settings->sample_rate_mic <  50) {
		LOG_WARN("MIC sampling rate should be >= 50ms.");
	}
	if (settings->sample_rate_sys  < 100) {
		LOG_WARN("System sampling rate should be >= 100ms.");
	}
	
	if (settings->idle_measurements != MEASURE_IDLE_NOTHING) {
		run(settings);
	} else {
		LOG_ERROR("Either option -c, -g, -f, -m, -s, or -a is required.");
		print_help(argv, settings);
	}
	
	free_settings(&settings);
	
	return EXIT_SUCCESS;
}

static void print_help(char **argv, ARGUMENTS *settings) {
	fprintf(stderr,
			"Usage:\n"
			"%s [-h|-?|-c|-g|-f|-m|-s|-a|-o RESULT_FILE]\n\n"
			"-h             | Print this help message.\n"
			"-?             | Print this help message.\n"
			"-c             | Measure idle power of CPU.\n"
			"-g             | Measure idle power of GPU.\n"
			"-f             | Measure idle power of FPGA.\n"
			"-m             | Measure idle power of MIC.\n"
			"-s             | Measure idle power of SYSTEM.\n"
			"               | Note that the SYSTEM measurement enables CPU, GPU, MIC, and FPGA\n"
			"               | measurements (same as -a option).\n"
			"-a             | Measure idle power of all devices (CPU, GPU, FPGA, MIC, and SYSTEM).\n"
			"-o RESULT_FILE | Print the measuring results to RESULT_FILE as a json string.\n"
			"               | Default: %s\n"
			"\n"
			"IMPORTANT INFORMATION:\n"
			"The median result fields in the json file depends on accumulated power values.\n"
			"For instance, if you enable CPU and GPU measurements the result will be\n"
			"the median of the sum of P_CPU and P_GPU.\n"
			"\n",
			argv[0],
			settings->jsonfile
	);
}

static void init_settings(ARGUMENTS **settings) {
 	*settings = (ARGUMENTS *)malloc(sizeof(ARGUMENTS));
	if (NULL == settings) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	// Please, set the default settings here
	(*settings)->sample_rate_cpu		= 30;
	(*settings)->sample_rate_gpu		= 40;
	(*settings)->sample_rate_fpga		= 50;
	(*settings)->sample_rate_mic		= 100;
	(*settings)->sample_rate_sys		= 100;
	(*settings)->skip_ms_rate_cpu		= 1;
	(*settings)->skip_ms_rate_gpu		= 1;
	(*settings)->skip_ms_rate_fpga		= 1;
	(*settings)->skip_ms_rate_mic		= 1;
	(*settings)->skip_ms_rate_sys		= 1;
	(*settings)->gpu_freq				= GPU_FREQUENCY_CUR;
	(*settings)->cpu_gov				= CPU_GOVERNOR_ONDEMAND;
	(*settings)->ipmi_timeout_setting 	= IOC_SET_IPMI_TIMEOUT;
	(*settings)->cpu_freq_min			= 0;
	(*settings)->cpu_freq_max			= 0;
	(*settings)->skip_ms				= LOW;
	(*settings)->variant				= LIGHT;
	(*settings)->hostname				= NULL;
	(*settings)->hostname_size			= 127;
	(*settings)->jsonfile				= NULL;
	(*settings)->jsonfile_size			= 255;
	(*settings)->idle_measurements		= MEASURE_IDLE_NOTHING;
	
	(*settings)->hostname	= malloc(((*settings)->hostname_size)+1);
	if (NULL == (*settings)->hostname) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	memset((*settings)->hostname, 0, ((*settings)->hostname_size)+1);
	gethostname((*settings)->hostname, (*settings)->hostname_size);
	
	(*settings)->jsonfile	= malloc(((*settings)->jsonfile_size)+1);
	if (NULL == (*settings)->jsonfile) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	memset((*settings)->jsonfile, 0, ((*settings)->jsonfile_size)+1);
	sprintf((*settings)->jsonfile, "%s%s%s%s", "/usr/ampehre/share/data/", "idle_power_", (*settings)->hostname, ".json");
}

static void free_settings(ARGUMENTS **settings) {
	if ((*settings)->hostname) {
		free((*settings)->hostname);
		(*settings)->hostname		= NULL;
		(*settings)->hostname_size	= 0;
	}
	if ((*settings)->jsonfile) {
		free((*settings)->jsonfile);
		(*settings)->jsonfile		= NULL;
		(*settings)->jsonfile_size	= 0;
	}
	
	free(*settings);
	settings = NULL;
}
