/**
 * @file ms_hetnodewrapper_internal.h
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
 *          0.7.0 - modularised measurement struct
 */

#ifndef __MS_HETNODEWRAPPER_INTERNAL_H__
#define __MS_HETNODEWRAPPER_INTERNAL_H__

#include <stdio.h>
#include "../../../include/ms_hetnodewrapper.h"

#define CPU_STATS 9

#define ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", __FILE__, __LINE__, msg)
#define WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", __FILE__, __LINE__, msg)

/**
 * Struct to store all relevant data for the measurement system.
 */
typedef struct __mintern {
	MS_SYSTEM *global_ms;
	MS_LIST *global_m;
	MEASUREMENT_DATA *measurements;
	int32_t	resources;
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_sys;
	int32_t measurement_no;
} MINTERN;

/**
 * struct to store temp data.
 */
typedef struct __msdataintern {
	int32_t started;
	uint64_t cpu_util_acc[9];
	uint64_t cpu_util_active_total;
	uint64_t cpu_util_idle_total;
} MSDATAINTERN;

/**
 * Set all relevant variables to zero.
 * @param meas Pointer to the measurement data
 */
void set_data_zero(MEASUREMENT_DATA *meas);

/**
 * Set the start values for the measurement.
 * @param no ID of the measurement
 * @param mintern struct with the measurement data
 * @param temp_date Pointer to a struct for temp data.
 */
void set_measurement(int no, MINTERN *mintern, MSDATAINTERN *temp_data);

/**
 * Get the difference from the start values and the actual values and save them in the MEASUREMENT_DATA struct.
 * @param no ID of the measurement
 * @param mintern struct with the measurement data
 * @param temp_data Pointer to a struct for temp data.
 */
void get_diff_measurement(int no, MINTERN *mintern, MSDATAINTERN *temp_data);
#endif /* __MS_HETNODEWRAPPER_INTERNAL_H__*/
