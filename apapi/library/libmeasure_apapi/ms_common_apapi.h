/*                                                                                                                                              
 * ms_common_apapi.h
 * 
 * Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Alex Wiens <awiens@mail.uni-paderborn.de>  * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Alex Wiens (awiens@mail.uni-paderborn.de)
 * created: 6/01/17
 * version: 0.8.0 - initial implementation
 */


#ifndef __MS_COMMON_APAPI_H__
#define __MS_COMMON_APAPI_H__

#include "ms_measurement.h"
#include "apapi.h"

// note:
// ms_cpu_intel_xeon_sandy.h: use CPUS and CORES definitions

char *known_components[] = { 
    "rapl",
    "nvml",
    "maxeler",
    "ipmi",
    "micknc",
    NULL
};

/**
 *	Maps known PAPI components to libmeasure resources
 *	Related to known_components
 */
int component_mapping[] = {
	CPU,
	GPU,
	FPGA,
	SYSTEM,
	MIC
};

/**
 *	Defines information passed to update method after
 *  each timer interval/ sampling
 */
struct __mgmt_update {
	// index of the PAPI component
	int cmp_index;
	// pointer to mgmt
	struct __mgmt_internal *mgmt;
	// hettime resource type id
	int measurement_id;
	// pointer to resource-dependent measurement struct
	void* measurement;
	// pointer to apapi_eventset for PAPI component
	struct apapi_eventset *set;
	// pointer to map with value mappings for intermediate values
	struct __mapper *map;
	// pointer to map with value mappings for final values
	struct __mapper *map_last;
};

/**
 *	struct for internal data structure of libmeasure interface
 */
struct __mgmt_internal {
	int num_cmp;
	// one item per known component
	int *available_components; // 0 -> not available, 1 -> available
	struct apapi_eventset **sets;
	struct apapi_timer **timers;
	struct __mgmt_update *update_args;
	// user event descriptions
	char *user_eventops_file;
	struct apapi_event_ops *user_eventops;
	int user_eventops_num;
	// user eventlist
	char *user_eventlist_file;
	char ***user_eventlist_sorted;
	char **user_eventlist_cmp;
	// user component list
	char *user_cmplist_buffer;
	char **user_cmplist;
};

/**
 *	defines source and destination types for
 *	value copying
 */
enum __mapper_type {
	longlong2longlong,
	longlong2double,
	longlong2uint32,
	longlong2uint64,
	double2double,
	double2uint32,
	double2uint64
};

/**
 *	defines parameters for one value mapping
 *	After each timer interval (for each component)
 *		the intermediate results are copied to the
 *		hettime structs.
 *	After the last interval additionally the final
 *		results are copied to the hettime structs.
 */
struct __mapper {
	// pointer to source value
	void *source;
	// pointer to destination value
	void *destination;
	// defines the types of source and destination for
	// correct conversion
	enum __mapper_type type;
	// defines factor to use to convert between APAPI
	// counter values and hettime values
	double factor;
};

#endif /* __MS_COMMON_APAPI_H__ */
