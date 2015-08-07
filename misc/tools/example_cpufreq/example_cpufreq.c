/*
 * example_cpufreq.c
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
 * created: 9/14/14
 * version: 0.1.14 - write a cpufreq test app
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cpufreq.h>

#define CORES 8

int main(int argc, char **argv) {
	int take_min = 0;
	int i;
	
	// Small hack to reset minimum frequency to hardware minimum.
	if (argc == 2) {
		take_min = 1;
	}
	
	printf("Current frequencies:\n");
	for (i=0; i<CORES; ++i) {
		printf("core %i: %lu kHz\n", i, cpufreq_get_freq_kernel(i));
	}
	printf("\n");
	
	printf("Hardware limits:\n");
	for (i=0; i<CORES; ++i) {
		uint64_t min	= 0;
		uint64_t max	= 0;
		
		cpufreq_get_hardware_limits(i, &min, &max);
		
		printf("core %i: min %lu kHz, max %lu kHz\n", i, min, max);
	}
	printf("\n");
	
	struct cpufreq_policy* policy	= NULL;
	uint64_t min					= 0;
	uint64_t max					= 0;
	uint64_t middle					= 0;
	
	cpufreq_get_hardware_limits(0, &min, &max);
	policy = cpufreq_get_policy(0);
	if (NULL == policy) {
		fprintf(stderr, "Cannot obtain cpufreq policy.\n");
		return EXIT_FAILURE;
	}
	
	printf("Current governor:\n");
	printf("core 0: governor %s, min %lu kHz, max %lu\n", policy->governor, policy->min, policy->max);
	printf("\n");
	
	struct cpufreq_available_frequencies *avail_freq = NULL;
	avail_freq = cpufreq_get_available_frequencies(0);
	if (NULL != avail_freq) {
		printf("Available frequencies:\n");
		
		struct cpufreq_available_frequencies *it_freq	= avail_freq;
		i	= 0;
		
		while (NULL != it_freq) {
			printf("core 0: freq %2i %lu kHz\n", i++, it_freq->frequency);
			it_freq = it_freq->next;
		}
		
		it_freq	= avail_freq;
		int n	= i/2-1;
		for (i=0; i<n; ++i) {
			it_freq = it_freq->next;
		}
		
		middle = it_freq->frequency;
		
		cpufreq_put_available_frequencies(avail_freq);
		
		printf("\n");
	} else {
		middle = min;
	}
	
	printf("Set policy frequencies to new min and max frequencies:\n");
	if (cpufreq_modify_policy_min(0, take_min ? min : middle)) {
		fprintf(stderr, "Cannot set minimum frequency. Are you root?\n");
		return EXIT_FAILURE;
	}
	if (cpufreq_modify_policy_max(0, max)) {
		fprintf(stderr, "Cannot set maximum frequency. Are you root?\n");
		return EXIT_FAILURE;
	}
	policy = cpufreq_get_policy(0);
	printf("core 0: governor %s, min %lu kHz, max %lu\n", policy->governor, policy->min, policy->max);
	printf("\n");
	
	return EXIT_SUCCESS;
}
