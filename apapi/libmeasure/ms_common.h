#ifndef __MS_COMMON_H__
#define __MS_COMMON_H__

#include "ms_measurement.h"
#include "apapi.h"

char *known_components[] = { 
    "rapl",
    "nvml",
    "maxeler",
    "ipmi",
    "micknc",
    NULL
};

int component_mapping[] = {
	CPU,
	GPU,
	FPGA,
	SYSTEM,
	MIC
};

struct __mgmt_update {
	int cmp_index;
	struct __mgmt_internal *mgmt;
	int measurement_id;
	void* measurement;
	struct apapi_eventset *set;
	struct __mapper *map;
};

struct __mgmt_internal {
	int num_cmp;
	// one item per known component
	int *available_components; // 0 -> not available, 1 -> available
	struct apapi_eventset **sets;
	struct apapi_timer **timers;
	struct __mgmt_update *update_args;
	// default event descriptions
	char *defaults_file;
	struct apapi_event_ops *defaults_events;
	int defaults_events_num;
};

enum __mapper_type {
	longlong2longlong,
	longlong2double,
	longlong2uint32,
	longlong2uint64,
	double2double,
	double2uint32,
	double2uint64
};

struct __mapper {
	void *source;
	void *destination;
	enum __mapper_type type;
	double factor;
};

#endif /* __MS_COMMON_H__ */
