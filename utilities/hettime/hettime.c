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
 * created: 7/28/14
 * version: 0.1.0 - initial implementation
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <regex.h>

#include "hettime.h"

int main(int argc, char **argv);
static void print_help(char **argv, ARGUMENTS *std_settings);
static void init_settings(ARGUMENTS **std_settings, ARGUMENTS **cur_settings);
static void free_settings(ARGUMENTS **std_settings, ARGUMENTS **cur_settings);
static void copy_str_to_newstr(char **target, char *source, int size);
static void copy_str_to_newarr(ARGUMENTS *settings, char *source);
static int check_string_args(char *argument, const char **valid_arguments, int size);
static int check_string_is_number(char *argument);
static int parse_sampling_rates(char* argument, uint32_t *sampple_rate, uint32_t *skip_ms_rate);

int main(int argc, char **argv) {
	int status_child		= 1;
	ARGUMENTS *std_settings = NULL;
	ARGUMENTS *cur_settings = NULL;
	init_settings(&std_settings, &cur_settings);
	
	const char *gpu_freqs[GPU_FREQUENCIES][4] = {
		{"min", "MIN", "minimum", "MINIMUM"},
		{"cur", "CUR", "current", "CURRENT"},
		{"max", "MAX", "maximum", "MAXIMUM"}
	};
	
	const char *cpu_govs[CPU_GOVERNORS][4] = {
		{"save", "SAVE", "powersave"   , "POWERSAVE"},
		{"dmnd", "DMND", "ondemand"    , "ONDEMAND"},
		{"cons", "CONS", "conservative", "CONSERVATIVE"},
		{"perf", "PERF", "performance" , "PERFORMANCE"}
	};
	
	const char *ms_freq[SKIP_MS_FREQUENCIES][3] = {
		{"high", "HIGH", "High"},
		{"low", "LOW", "Low"}
	};
	
	const char *lib_var[VARIANTS][3] = {
		{"light", "LIGHT", "Light"},
		{"full", "FULL", "Full"}
	};
	
	int32_t opt;
	
	int32_t file_flag = 0;
	int32_t args_flag = 0;
	
	while ((opt = getopt(argc, argv, "h?c:g:f:m:s:S:e:a:G:C:L:H:V:o:v:ui")) != -1) {
		switch (opt) {
			case 'a':
				if (file_flag == 0) {
					LOG_ERROR("The argument flag must be set after the executable flag.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				copy_str_to_newarr(cur_settings, optarg);
				args_flag = 1;
				break;
			case 'e':
				copy_str_to_newstr(&(cur_settings->child_filename), optarg, strlen(optarg));
				file_flag = 1;
				break;
			case 'c':
				if (parse_sampling_rates(optarg, &(cur_settings->sample_rate_cpu), &(cur_settings->skip_ms_rate_cpu))) {
					LOG_ERROR("Wrong argument of option 'c'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'g':
				if (parse_sampling_rates(optarg, &(cur_settings->sample_rate_gpu), &(cur_settings->skip_ms_rate_gpu))) {
					LOG_ERROR("Wrong argument of option 'g'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'f':
				if (parse_sampling_rates(optarg, &(cur_settings->sample_rate_fpga), &(cur_settings->skip_ms_rate_fpga))) {
					LOG_ERROR("Wrong argument of option 'f'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'm':
				if (parse_sampling_rates(optarg, &(cur_settings->sample_rate_mic), &(cur_settings->skip_ms_rate_mic))) {
					LOG_ERROR("Wrong argument of option 'm'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;	
			case 's':
				if (parse_sampling_rates(optarg, &(cur_settings->sample_rate_sys), &(cur_settings->skip_ms_rate_sys))) {
					LOG_ERROR("Wrong argument of option 's'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'S':
				if (check_string_args(optarg, ms_freq[LOW], 3)) {
					cur_settings->skip_ms = LOW;
				} else if (check_string_args(optarg, ms_freq[HIGH], 3)) {
					cur_settings->skip_ms = HIGH;
				} else {
					LOG_ERROR("Encounter wrong argument with option 'G'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'G':
				if (check_string_args(optarg, gpu_freqs[GPU_FREQUENCY_MIN], 4)) {
					cur_settings->gpu_freq = GPU_FREQUENCY_MIN;
				} else if (check_string_args(optarg, gpu_freqs[GPU_FREQUENCY_MAX], 4)) {
					cur_settings->gpu_freq = GPU_FREQUENCY_MAX;
				} else if (check_string_args(optarg, gpu_freqs[GPU_FREQUENCY_CUR], 4)) {
					cur_settings->gpu_freq = GPU_FREQUENCY_CUR;
				} else {
					LOG_ERROR("Encounter wrong argument with option 'G'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'C':
				if (check_string_args(optarg, cpu_govs[CPU_GOVERNOR_POWERSAVE], 4)) {
					cur_settings->cpu_gov = CPU_GOVERNOR_POWERSAVE;
				} else if (check_string_args(optarg, cpu_govs[CPU_GOVERNOR_ONDEMAND], 4)) {
					cur_settings->cpu_gov = CPU_GOVERNOR_ONDEMAND;
				} else if (check_string_args(optarg, cpu_govs[CPU_GOVERNOR_CONSERVATIVE], 4)) {
					cur_settings->cpu_gov = CPU_GOVERNOR_CONSERVATIVE;
				} else if (check_string_args(optarg, cpu_govs[CPU_GOVERNOR_PERFORMANCE], 4)) {
					cur_settings->cpu_gov = CPU_GOVERNOR_PERFORMANCE;
				} else {
					LOG_ERROR("Encounter wrong argument with option 'C'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'L':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 'L' is not a number.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				cur_settings->cpu_freq_min = atoi(optarg)*1000;
				break;
			case 'H':
				if (!check_string_is_number(optarg)) {
					LOG_ERROR("The argument of option 'H' is not a number.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				cur_settings->cpu_freq_max = atoi(optarg)*1000;
				break;
			case 'V':
				if (check_string_args(optarg, lib_var[LIGHT2], 3)) {
					cur_settings->variant = LIGHT2;
				} else if (check_string_args(optarg, lib_var[FULL], 3)) {
					cur_settings->variant = FULL;
				} else {
					LOG_ERROR("Encounter wrong argument with option 'V'.");
					print_help(argv, std_settings);
					free_settings(&std_settings, &cur_settings);
					return EXIT_FAILURE;
				}
				break;
			case 'o':
				copy_str_to_newstr(&(cur_settings->ostream_filename), optarg, strlen(optarg));
				break;
			case 'v':
				copy_str_to_newstr(&(cur_settings->csv_filename), optarg, strlen(optarg));
				break;
			case 'i':
				cur_settings->force_idle_fpga = 1;
				break;
			case 'u':
				cur_settings->ush_client = 1;
				break;
			case '?':
			case 'h':
			default:
				print_help(argv, std_settings);
				free_settings(&std_settings, &cur_settings);
				return EXIT_FAILURE;
		}
	}
	
	if (file_flag) {
		if (args_flag == 0) {
			copy_str_to_newarr(cur_settings, NULL);
		}
		
		if (cur_settings->sample_rate_cpu  <  20) {
			LOG_WARN("CPU sampling rate should be >= 20ms.");
		}
		if (cur_settings->sample_rate_gpu  <  30) {
			LOG_WARN("GPU sampling rate should be >= 30ms.");
		}
		if (cur_settings->sample_rate_fpga <  50) {
			LOG_WARN("FPGA sampling rate should be >= 50ms.");
		}
		if (cur_settings->sample_rate_mic <  20) {
			LOG_WARN("MIC sampling rate should be >= 20ms.");
		}
		if (cur_settings->sample_rate_sys  < 100) {
			LOG_WARN("System sampling rate should be >= 100ms.");
		}
		
		status_child = run(cur_settings);
	} else {
		LOG_ERROR("Executable missing.");
		print_help(argv, std_settings);
		free_settings(&std_settings, &cur_settings);
		return EXIT_FAILURE;
	}
	
	free_settings(&std_settings, &cur_settings);
	
	return ((status_child == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void print_help(char **argv, ARGUMENTS *std_settings) {
	size_t gpu_freq_string_size = 3;
	char gpu_freq_string[gpu_freq_string_size+1];
	memset(gpu_freq_string, 0, gpu_freq_string_size+1);
	
	switch (std_settings->gpu_freq) {
		case GPU_FREQUENCY_MIN:
			strncpy(gpu_freq_string, "min", 3);
			break;
		case GPU_FREQUENCY_MAX:
			strncpy(gpu_freq_string, "max", 3);
			break;
		case GPU_FREQUENCY_CUR:
		default:
			strncpy(gpu_freq_string, "cur", 3);
			break;
	}
	
	size_t cpu_gov_string_size = 4;
	char cpu_gov_string[cpu_gov_string_size+1];
	memset(cpu_gov_string, 0, cpu_gov_string_size+1);
	
	switch (std_settings->cpu_gov) {
		case CPU_GOVERNOR_CONSERVATIVE:
			strncpy(cpu_gov_string, "cons", 4);
			break;
		case CPU_GOVERNOR_PERFORMANCE:
			strncpy(cpu_gov_string, "perf", 4);
			break;
		case CPU_GOVERNOR_POWERSAVE:
			strncpy(cpu_gov_string, "save", 4);
			break;
		case CPU_GOVERNOR_ONDEMAND:
		default:
			strncpy(cpu_gov_string, "dmnd", 4);
			break;
	}
	
	fprintf(stderr,
			"Usage:\n"
			"%s [-h|-?|-i|-c \"SAMPLE_CPU, SAMPLE_SKIP_CPU\"|-g \"SAMPLE_GPU, SAMPLE_SKIP_GPU\"|-f \"SAMPLE_FPGA, SAMPLE_SKIP_FPGA\"|\n"
			"         -m \"SAMPLE_MIC, SAMPLE_SKIP_MIC\"|-s \"SAMPLE_SYS, SAMPLE_SKIP_SYS\"|-G FREQUENCY|-C GOVERNOR|-L FREQUENCY|\n"
			"         -H FREQUENCY|-o RESULT_FILE|-v CSV_FILE|-u] -e EXECUTABLE [-a \"ARGS\"]\n"
			"-c \"SAMPLE_CPU,       | Sampling rate for CPU power/temp measurements in ms.\n"
			"    SAMPLE_SKIP_CPU\"  | Skip rate defines how many measurement points are skipped.\n"
			"                      | Default: %ums, %u. Recommended minimum: 20ms.\n"
			"-g \"SAMPLE_GPU,       | Sampling rate for GPU power/temp measurements in ms.\n"
			"    SAMPLE_SKIP_GPU\"  | Skip rate defines how many measurement points are skipped.\n"
			"                      | Default: %ums, %u. Recommended minimum: 30ms.\n"
			"-f \"SAMPLE_FPGA,      | Sampling rate for FPGA power/temp measurements in ms.\n"
			"    SAMPLE_SKIP_FPGA\" | Skip rate defines how many measurement points are skipped.\n"
			"                      | Default: %ums, %u. Recommended minimum: 50ms.\n"
			"-m \"SAMPLE_MIC,\"      | Sampling rate for MIC power/temp measurements in ms.\n"
			"    SAMPLE_SKIP_MIC\"  | Skip rate defines how many measurement points are skipped.\n"
			"                      | Default: %ums, %u. Recommended minimum: 20ms.\n"
			"-s \"SAMPLE_SYS        | Sampling rate for system-wide power/temp measurements in ms.\n"
			"    SAMPLE_SKIP_SYS\"  |  Skip rate defines how many measurement points are skipped.\n"
			"                      | Default: %ums, %u. Recommended minimum: 100ms.\n"
			"-e EXECUTABLE         | Name of the executable. This option is mandatory.\n"
			"-a \"ARGS\"             | Specify the arguments for executable EXECUTABLE with this option.\n"
			"                      | Note that the arguments have to be seperated by spaces.\n"
			"                      | The arguments must be surrounded by quotation marks!\n"
			"                      | Note that the ARGS option has to be the last in the argument list!\n"
			"-G FREQUENCY          | Set a GPU frequency before the child application get started.\n"
			"                      | Possible frequency settings are:\n"
			"                      | min, MIN, minimum, MINIMUM\n"
			"                      |    Set GPU frequency to its minimum value.\n"
			"                      | max, MAX, maximum, MAXIMUM\n"
			"                      |    Set GPU frequency to its maximum value.\n"
			"                      | cur, CUR, current, CURRENT\n"
			"                      |    Don't set GPU frequency. Leave the current setting untouched.\n"
			"                      | Default: %s.\n"
			"-C GOVERNOR           | Set a CPU frequency scaling governor for the 'acpi-cpufreq' driver.\n"
			"                      | Possible governors are:\n"
			"                      | save, SAVE, powersave, POWERSAVE\n"
			"                      |    Force CPU to use the lowest possible frequency.\n"
			"                      | dmnd, DMND, ondemand, ONDEMAND\n"
			"                      |    Dynamic frequency scaling. Aggresive strategy.\n"
			"                      | cons, CONS, conservative, CONSERVATIVE\n"
			"                      |    Dynamic frequency scaling. Conservative strategy.\n"
			"                      | perf, PERF, performance, PERFORMANCE\n"
			"                      |    Force CPU to use the highest possible frequency.\n"
			"                      | Default: %s.\n"
			"-L FREQUENCY          | Set the lowest permitted CPU frequency in MHz.\n"
			"-H FREQUENCY          | Set the highest permitted CPU frequency in MHz.\n"
			"-o RESULT_FILE        | Save results in a file instead of printing to stdout.\n"
			"-v CSV_FILE           | Save results in a CSV table file.\n"
			"-u                    | Use UNIX socket handler library to communicate with msmonitor.\n"
			"-i                    | Forcing FPGA to idle after measuring system initialization.\n"
			"-h                    | Print this help message.\n"
			"-?                    | Print this help message.\n"
			"\n"
			"Example:\n"
			"%s -c \"90, 10\" -i -G min -C conservative -e /usr/bin/find -a \"/usr -iname lib*\"\n"
			"\n",
			argv[0],
			std_settings->sample_rate_cpu,
			std_settings->skip_ms_rate_cpu,
			std_settings->sample_rate_gpu,
			std_settings->skip_ms_rate_gpu,
			std_settings->sample_rate_fpga,
			std_settings->skip_ms_rate_fpga,
			std_settings->sample_rate_mic,
			std_settings->skip_ms_rate_mic,
			std_settings->sample_rate_sys,
			std_settings->skip_ms_rate_sys,
			gpu_freq_string,
			cpu_gov_string,
			argv[0]
	);
}

static void init_settings(ARGUMENTS **std_settings, ARGUMENTS **cur_settings) {
 	*std_settings = (ARGUMENTS *)malloc(sizeof(ARGUMENTS));
	*cur_settings = (ARGUMENTS *)malloc(sizeof(ARGUMENTS));
	if (NULL == std_settings || NULL == cur_settings) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	// Please, set the default settings here
	(*std_settings)->child_filename			= NULL;
	(*std_settings)->child_args				= NULL;
	(*std_settings)->child_num_of_args		= 0;
	(*std_settings)->csv_filename			= NULL;
	(*std_settings)->ostream_filename		= NULL;
	(*std_settings)->force_idle_fpga		= 0;
	(*std_settings)->sample_rate_cpu		= 100;
	(*std_settings)->sample_rate_gpu		= 100;
	(*std_settings)->sample_rate_fpga		= 100;
	(*std_settings)->sample_rate_mic		= 100;
	(*std_settings)->sample_rate_sys		= 100;
	(*std_settings)->skip_ms_rate_cpu		= 1;
	(*std_settings)->skip_ms_rate_gpu		= 1;
	(*std_settings)->skip_ms_rate_fpga		= 1;
	(*std_settings)->skip_ms_rate_mic		= 1;
	(*std_settings)->skip_ms_rate_sys		= 1;
	(*std_settings)->gpu_freq				= GPU_FREQUENCY_CUR;
	(*std_settings)->cpu_gov				= CPU_GOVERNOR_ONDEMAND;
	(*std_settings)->cpu_freq_min			= 0;
	(*std_settings)->cpu_freq_max			= 0;
	(*std_settings)->ush_client				= 0;
	(*std_settings)->ipmi_timeout_setting	= IOC_SET_IPMI_TIMEOUT;
	(*std_settings)->skip_ms				= LOW;
	(*std_settings)->variant				= FULL;
	
	memcpy(*cur_settings, *std_settings, sizeof(ARGUMENTS));
}

static void free_settings(ARGUMENTS **std_settings, ARGUMENTS **cur_settings) {
	free(*std_settings);
	std_settings = NULL;
	
	if (NULL != (*cur_settings)->child_filename) {
		free((*cur_settings)->child_filename);
	}
	
	if (NULL != (*cur_settings)->child_args) {
		int i;
		for (i=0; i<((*cur_settings)->child_num_of_args-1); ++i) {
			free((*cur_settings)->child_args[i]);
		}
		free((*cur_settings)->child_args);
	}
	
	free(*cur_settings);
	cur_settings = NULL;
}

static void copy_str_to_newstr(char **target, char *source, int size) {
	*target = (char *)malloc(size+1);
	if (NULL == *target) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	memset(*target, 0, size+1);
	strncpy(*target, source, size);
}

static void copy_str_to_newarr(ARGUMENTS *settings, char *source) {
	int i = 0;
	int size = (NULL == source) ? 0 : strlen(source);
	
	// Count number of arguments by counting spaces
	settings->child_num_of_args = (NULL == source) ? 0 : 1;
	for (i=0; i<size; ++i) {
		if (source[i] == ' ') {
			if (i == 0 || i == (size-1)) {
				LOG_ERROR("There is a space at the beginning/end of arguments list.");
				exit(EXIT_FAILURE);
			} else if (i<(size-1) && source[i+1] == ' ') {
				LOG_ERROR("There are two or more successive spaces.");
				exit(EXIT_FAILURE);
			}
			
			settings->child_num_of_args++;
		}
	}
	
	// We need two more pointers. One for the name of the executable and one for a NULL pointer.
	settings->child_num_of_args += 2;
	
	settings->child_args = (char **)malloc(settings->child_num_of_args * sizeof(char *));
	if (NULL == settings->child_args) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	// Child filename itself is the first argument of child's argument list.
	copy_str_to_newstr(&(settings->child_args[0]), settings->child_filename, strlen(settings->child_filename));
	
	// Extract arguments from list
	for (i=1; i<(settings->child_num_of_args-1); ++i) {
		char find		= ' ';
		if (i >= (settings->child_num_of_args-2)) {
			find = 0;
		}
		
		char *temp		= strchr(source, find);
		int str_size	= (int)(temp - source);
		
		copy_str_to_newstr(&(settings->child_args[i]), source, str_size);
		
		source = temp+1;
	}
	
	settings->child_args[i] = NULL;
}

static int check_string_args(char *argument, const char **valid_arguments, const int size) {
	int i;
	
	if (NULL == argument || strlen(argument) == 0 || NULL == valid_arguments) {
		return 0;
	}
	
	for (i=0; i<size; ++i) {
		if (strlen(argument) == strlen(valid_arguments[i])) {
			if (strncmp(valid_arguments[i], argument, strlen(argument)) == 0) {
				return 1;
			}
		}
	}
	
	return 0;
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

static int parse_sampling_rates(char* argument, uint32_t *sample_rate, uint32_t *skip_ms_rate) {
	regex_t regex;
	regmatch_t rm;
	char buffer[100];
	int rv;
	char *regex_2_digits_string	= "^[[:digit:]]+[[:space:]]*,[[:space:]]*[[:digit:]]+$";
	char *regex_1_digit_string	= "^[[:space:]]*[[:digit:]]+[[:space:]]*$";
	memset(buffer, 0, 100);
	
	rv = regcomp(&regex, regex_2_digits_string, REG_EXTENDED);
	if (rv) {
		LOG_ERROR("Could not compile regex.\n");
		exit(EXIT_FAILURE);
	}
	
	if(!regexec(&regex, argument, 1, &rm, 0)){
		strncpy(buffer, &argument[rm.rm_so], (rm.rm_eo - rm.rm_so));
		sscanf(buffer, "%d , %d", sample_rate, skip_ms_rate);
		regfree(&regex);
	} else {
		rv = regcomp(&regex, regex_1_digit_string, REG_EXTENDED);
		if (rv) {
			LOG_ERROR("Could not compile regex.");
			exit(EXIT_FAILURE);
		}
		
		if(!regexec(&regex, argument, 1, &rm, 0)){
			strncpy(buffer, &argument[rm.rm_so], (rm.rm_eo - rm.rm_so));
			sscanf(buffer, "%d", sample_rate);
			*skip_ms_rate = 1;
			regfree(&regex);
		} else {
			regfree(&regex);
			return -1;
		}
	}
	
	if(*skip_ms_rate < 1){
		LOG_WARN("Set skip_ms_rate to default. skip_ms_rate must be greater than 0.");
		*skip_ms_rate = 1;
	}
	
	return 0;
}
