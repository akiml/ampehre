/*
 * main.c
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

#include "gaussblur.h"
#include "measure.h"

#define GAUSS_T					 256
#define GAUSS_T_ENERGY_LOOPS	  64
#define GAUSS_N					2048

int main(int argc, char **argv);
static void print_help(char **argv, ARGUMENTS *settings);
static void init_settings(ARGUMENTS **settings);
static void free_settings(ARGUMENTS **settings);
static int check_string_is_number(char *argument);

int main(int argc, char **argv) {
	ARGUMENTS *settings = NULL;
	init_settings(&settings);
	
	int32_t opt;
	
	while ((opt = getopt(argc, argv, "c:e:s:h?")) != -1) {
		switch (opt) {
			case 'i':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 'c' is not a number.");
					print_help(argv, settings);
					free_settings(&settings);
					return EXIT_FAILURE;
				}
				settings->checkpoints = atoi(optarg);
				break;
			case 'e':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 'e' is not a number.");
					print_help(argv, settings);
					free_settings(&settings);
					return EXIT_FAILURE;
				}
				settings->energy_loops = atoi(optarg);
				break;
			case 's':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 's' is not a number.");
					print_help(argv, settings);
					free_settings(&settings);
					return EXIT_FAILURE;
				}
				settings->size = atoi(optarg);
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
	
	if (settings->size != 0 && settings->checkpoints != 0 && settings->energy_loops) {
		run(settings);
	} else {
		LOG_ERROR("You must set the 'e', 'c', and 's' flag.");
		print_help(argv, settings);
		return EXIT_FAILURE;
	}
	
	free_settings(&settings);
	
	return EXIT_SUCCESS;
}

static void print_help(char **argv, ARGUMENTS *settings) {
	fprintf(stderr,
			"Usage:\n"
			"%s [-h|-?] -e ENERGY_LOOPS -i ITERATIONS -s SIZE]\n\n"
			"-e ENERGY_LOOPS | Number of energy loops.\n"
			"-c CHECKPOINTS  | Number of outer loop iterations. This is similar to checkpoint\n"
			"                | counts in applications handled by the user space scheduler.\n"
			"-s SIZE         | Size of input data.\n"
			"-h              | Print this help message.\n"
			"-?              | Print this help message.\n"
			"\n",
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
	(*settings)->gpu_freq			= GPU_FREQUENCY_MAX;
	(*settings)->cpu_gov			= CPU_GOVERNOR_ONDEMAND;
	(*settings)->cpu_freq_min		= 0;
	(*settings)->cpu_freq_max		= 0;
	(*settings)->checkpoints		= GAUSS_T;
	(*settings)->size				= GAUSS_N;
	(*settings)->energy_loops		= GAUSS_T_ENERGY_LOOPS;
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
