/*
 * List.cpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 12/10/15
 * version: 0.7.0 - modularised measurement struct
 */

#include "../../include/ms_measurement.h"

#include <iostream>
#include <cstring>

#include <stdlib.h>
#define UINT64_MAX (0xffffffffffffffff)

static void* allocMeasurement(int id);

void appendList(MS_LIST **ms_list, int id) {
	MS_LIST *cur = *ms_list;
	
	if(NULL == cur) {
		*ms_list = new MS_LIST;
		cur = *ms_list;
	} else {
		while(cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = new MS_LIST;
		cur = cur->next;
	}
	
	cur->resource_id = id;
	cur->ms_measurement = allocMeasurement(id);
	cur->next = NULL;
}

void *getMeasurement(MS_LIST **ms_list, int id) {
	MS_LIST *cur = *ms_list;
	
	while ((cur != NULL) && (cur->resource_id != id)) {
		cur = cur->next;
	}
	
	if(NULL == cur){
		return NULL;
	} else {
		return cur->ms_measurement;
	}
}

void deleteList(MS_LIST **ms_list){
	MS_LIST *cur;
	MS_LIST *next;
	
	cur = *ms_list;
	while(cur != NULL) {
		next = cur->next;
		
		free(cur->ms_measurement);
		delete cur;
		
		cur = next;
	}
	
	*ms_list = NULL;
}

void* allocMeasurement(int id) {
	void* ms_measurement = NULL;
	
	switch(id) {
		case CPU:
			ms_measurement = malloc(sizeof(MS_MEASUREMENT_CPU));
			memset(ms_measurement, 0, sizeof(MS_MEASUREMENT_CPU));
			((MS_MEASUREMENT_CPU *)ms_measurement)->msr_time_wait.tv_sec	= UINT64_MAX;
			((MS_MEASUREMENT_CPU *)ms_measurement)->msr_time_wait.tv_nsec	= UINT64_MAX;
			((MS_MEASUREMENT_CPU *)ms_measurement)->msr_skip_ms_rate		= 1;
			break;
		case GPU:
			ms_measurement =  malloc(sizeof(MS_MEASUREMENT_GPU));
			memset(ms_measurement, 0, sizeof(MS_MEASUREMENT_GPU));
			((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_time_wait.tv_sec	= UINT64_MAX;
			((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_time_wait.tv_nsec	= UINT64_MAX;
			((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_skip_ms_rate		= 1;
			break;
		case FPGA:
			ms_measurement =  malloc(sizeof(MS_MEASUREMENT_FPGA));
			memset(ms_measurement, 0, sizeof(MS_MEASUREMENT_FPGA));
			((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_time_wait.tv_sec	= UINT64_MAX;
			((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_time_wait.tv_nsec	= UINT64_MAX;
			((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_skip_ms_rate		= 1;
			break;
		case MIC:
			ms_measurement =  malloc(sizeof(MS_MEASUREMENT_MIC));
			memset(ms_measurement, 0, sizeof(MS_MEASUREMENT_MIC));
			((MS_MEASUREMENT_MIC *)ms_measurement)->mic_time_wait.tv_sec	= UINT64_MAX;
			((MS_MEASUREMENT_MIC *)ms_measurement)->mic_time_wait.tv_nsec	= UINT64_MAX;
			((MS_MEASUREMENT_MIC *)ms_measurement)->mic_skip_ms_rate		= 1;
			break;
		case SYSTEM:
			ms_measurement =  malloc(sizeof(MS_MEASUREMENT_SYS));
			memset(ms_measurement, 0, sizeof(MS_MEASUREMENT_SYS));
			((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_time_wait.tv_sec	= UINT64_MAX;
			((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_time_wait.tv_nsec	= UINT64_MAX;
			((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_skip_ms_rate		= 1;
			break;
		default:
			std::cout << "!!! 'mgmt' (thread main): Error: cannot allocate resource specific measurement struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
	}
	return ms_measurement;
}
