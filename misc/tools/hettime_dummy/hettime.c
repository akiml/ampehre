/*
 * hettime.c
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

#include "hettime.h"

int main(int argc, char **argv);
static void print_help(char **argv, ARGUMENTS *settings);
static void init_settings(ARGUMENTS **settings);
static void free_settings(ARGUMENTS **settings);
static int check_string_is_number(char *argument);

int main(int argc, char **argv) {
	ARGUMENTS *settings = NULL;
	init_settings(&settings);
	
	int32_t opt;
	
	while ((opt = getopt(argc, argv, "h?t:")) != -1) {
		switch (opt) {
			case 't':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 't' is not a number.");
					print_help(argv, settings);
					free_settings(&settings);
					return EXIT_FAILURE;
				}
				settings->runtime = atoi(optarg);
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
	if (settings->sample_rate_sys  < 100) {
		LOG_WARN("System sampling rate should be >= 100ms.");
	}
	
	run(settings);
	
	free_settings(&settings);
	
	return EXIT_SUCCESS;
}

static void print_help(char **argv, ARGUMENTS *settings) {
	fprintf(stderr,
			"Usage:\n"
			"%s [-h|-?|-t RUNTIME]\n\n"
			"-h             | Print this help message.\n"
			"-?             | Print this help message.\n"
			"-t RUNTIME     | Run %s for \'RUNTIME\' ms.\n"
			"\n",
			argv[0],
			argv[0]
	);
}

static void init_settings(ARGUMENTS **settings) {
 	*settings = (ARGUMENTS *)malloc(sizeof(ARGUMENTS));
	if (NULL == settings) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	// Please, set the default settings here
	(*settings)->sample_rate_cpu	= 30;
	(*settings)->sample_rate_gpu	= 50;
	(*settings)->sample_rate_fpga	= 60;
	(*settings)->sample_rate_sys	= 100;
	(*settings)->gpu_freq			= GPU_FREQUENCY_CUR;
	(*settings)->cpu_gov			= CPU_GOVERNOR_ONDEMAND;
	(*settings)->cpu_freq_min		= 0;
	(*settings)->cpu_freq_max		= 0;
	(*settings)->runtime			= 0;
}

static void free_settings(ARGUMENTS **settings) {
	free(*settings);
	settings = NULL;
}

static int check_string_is_number(char *argument) {
	int i;
	
	if (NULL == argument || strlen(argument) == 0) {
		return 0;
	}
	
	for (i=0; i<strlen(argument); ++i) {
		if (!isdigit(argument[i])) {
			return 0;
		}
	}
	
	return 1;
}
