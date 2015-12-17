/**
 * @file ms_hetnodewrapper.c
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
 * @author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 9/02/14
 * version: 0.1.16 - add wrapper for heterogenous Node and test tool
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 */

#include "ms_hetnodewrapper_internal.h"

static MINTERN *mintern = NULL;
static MSDATAINTERN *temp_data = NULL;

void mshnw_init(MS_VERSION *version, HN_MEASUREMENT *measurements, int measurement_no) {
	
	if((version->major != MS_MAJOR_VERSION) || (version->minor != MS_MINOR_VERSION) || (version->revision != MS_REVISION_VERSION)){
		printf("Error in hetnodewrapper: Wrong version number! hetnodewrapper version %d.%d.%d is called from tool with version %d.%d.%d.\n", MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, version->major, version->minor, version->revision);
		exit(EXIT_FAILURE);
	}

	if (NULL != mintern) {
		WARN("Measuring system is already initialized!");
		return;
	}
	
	mintern = malloc(sizeof(MINTERN));
	if (NULL == mintern) {
		ERROR("Couldn't allocate memory!");
		exit(EXIT_FAILURE);
	}
	
	temp_data = malloc(sizeof(MSDATAINTERN)*measurement_no);
	if (NULL == temp_data) {
		ERROR("Couldn't allocate memory!");
		exit(EXIT_FAILURE);
	}

	// allocate memory for measurement results
	mintern->measurements = malloc(measurement_no * sizeof(MEASUREMENT_DATA));
	if (NULL == mintern) {
		ERROR("Couldn't allocate memory!");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	//initialy all measurements are stopped
	for (i = 0; i < measurement_no; i++) {
		temp_data[i].started = 0;
		mintern->measurements[i].resources = measurements->resources[i];
		set_data_zero(&(mintern->measurements[i]));
	}

	//Initialize Measurementsystem
	mintern->global_ms = ms_init(version, measurements->cpu_gov, measurements->cpu_freq_min, measurements->cpu_freq_max, measurements->gpu_freq, IOC_SET_IPMI_TIMEOUT, SKIP_NEVER, VARIANT_FULL);

	//Initialize global Measurement
	mintern->global_m = ms_alloc_measurement();

	mintern->sample_rate_cpu	= measurements->sample_rate_cpu;
	mintern->sample_rate_gpu	= measurements->sample_rate_gpu;
	mintern->sample_rate_fpga	= measurements->sample_rate_fpga;
	mintern->sample_rate_sys	= measurements->sample_rate_sys;
	
	mintern->measurement_no = measurement_no;

	mintern->resources = 0;

	// Measurement gets initialized only on necessary devices
	for (i = 0; i < measurement_no; i++) {
		mintern->resources = mintern->resources | measurements->resources[i];
	}

	// Set timer for  global measurement
	ms_set_timer(mintern->global_m, CPU   , mintern->sample_rate_cpu / 1000, (mintern->sample_rate_cpu % 1000) * 1000000, 1);
	ms_set_timer(mintern->global_m, GPU   , mintern->sample_rate_gpu / 1000, (mintern->sample_rate_gpu % 1000) * 1000000, 1);
	ms_set_timer(mintern->global_m, FPGA  , mintern->sample_rate_fpga / 1000, (mintern->sample_rate_fpga % 1000) * 1000000, 1);
	ms_set_timer(mintern->global_m, SYSTEM, mintern->sample_rate_sys / 1000, (mintern->sample_rate_sys % 1000) * 1000000, 1);
	
	ms_init_measurement(mintern->global_ms, mintern->global_m, mintern->resources);
}

void mshnw_start() {
	// Start measuring system
	ms_start_measurement(mintern->global_ms, mintern->global_m);
}

void mshnw_shutdown() {
	// Stop measuring system
	ms_stop_measurement(mintern->global_ms, mintern->global_m);

	// Join measurement threads and remove thread objects
	ms_join_measurement(mintern->global_ms, mintern->global_m);
	ms_fini_measurement(mintern->global_ms, mintern->global_m);

}

void mshnw_free() {
	//Cleanup and free all memory before exiting
	ms_free_measurement(mintern->global_m);
	ms_fini(mintern->global_ms);
	free(mintern->measurements);
	free(mintern);
	free(temp_data);
	temp_data = NULL;
	mintern = NULL;
}

void mshnw_start_measurement(int no) {
	if((no > mintern->measurement_no-1) || (no < 0)){
		WARN("The measurement with this index is not initialized!");
	}else if (temp_data[no].started == 1) {
		WARN("Measurement is allready started");
	} else {
		set_measurement(no ,mintern, temp_data);
		temp_data[no].started = 1;
	}
}

void mshnw_stop_measurement(int no) {
	if((no > mintern->measurement_no-1) || (no < 0)){
		WARN("The measurement with this index is not initialized!");
	}else if (temp_data[no].started == 0) {
		WARN("Measurement is allready stopped");
	} else {
		get_diff_measurement(no, mintern, temp_data);
		temp_data[no].started = 0;
	}
}

MEASUREMENT_DATA *mshnw_get_ms_data() {
	return mintern->measurements;
}

void mshnw_fpga_force_idle(){
	ms_init_fpga_force_idle(mintern->global_ms);
}

