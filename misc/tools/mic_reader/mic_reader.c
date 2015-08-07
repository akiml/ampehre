/*
 * mic_reader.c
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
 * created: 3/18/15
 * version: 0.3.4 - add a tool for first intergration tests of mic
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <ctype.h>

#include "mic.h"

int main(int argc, char **argv);
static void print_help(char **argv, int32_t num_of_measurements_default);
static int check_string_is_number(char *argument);

int main(int argc, char **argv) {
	int32_t opt;
	
	int32_t num_of_measurements = 1;
	
	while ((opt = getopt(argc, argv, "h?n:")) != -1) {
		switch (opt) {
			case 'n':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 'n' is not a positive number.");
					print_help(argv, 1);
					return EXIT_FAILURE;
				}
				num_of_measurements = atoi(optarg);
				if (num_of_measurements < 1) {
					LOG_ERROR("The argument of option 'n' must be > 0.");
					print_help(argv, 1);
					return EXIT_FAILURE;
				}
				break;
			case '?':
			case 'h':
			default:
				print_help(argv, 1);
				return EXIT_FAILURE;
		}
	}
	
	mic_call(num_of_measurements);
	
	return EXIT_SUCCESS;
}

static void print_help(char **argv, int32_t num_of_measurements_default) {
	fprintf(stderr,
			"Usage:\n"
			"%s [-h|-?|-n MEASUREMENTS]\n\n"
			"-h              | Print this help message.\n"
			"-?              | Print this help message.\n"
			"-n MEASUREMENTS | Number of measurements.\n"
			"                | Default: %i.\n"
			"\n",
			argv[0], num_of_measurements_default
	);
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
