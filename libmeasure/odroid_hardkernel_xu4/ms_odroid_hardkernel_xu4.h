/*
 * ms_odroid_hardkernel_xu4.h
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 2/25/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#ifndef __MS_ODROID_HARDKERNEL_XU4_H__
#define __MS_ODROID_HARDKERNEL_XU4_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <time.h>

#include "ms_list.h"

#define ODROID_NUM_POWER		4
#define ODROID_NUM_TEMP			5
#define ODROID_NUM_FREQ			3
#define ODROID_NUM_UTIL			3
#define ODROID_NUM_UTIL_FIELDS	2

enum odroid_power {
	ODROID_POWER_A15,
	ODROID_POWER_A7,
	ODROID_POWER_MALI,
	ODROID_POWER_MEM
};

enum odroid_temp {
	ODROID_TEMP_A15_CORE_0,
	ODROID_TEMP_A15_CORE_1,
	ODROID_TEMP_A15_CORE_2,
	ODROID_TEMP_A15_CORE_3,
	ODROID_TEMP_MALI
};

enum odroid_freq {
	ODROID_FREQ_A7,
	ODROID_FREQ_A15,
	ODROID_FREQ_MALI
};

enum odroid_util {
	ODROID_UTIL_A7,
	ODROID_UTIL_A15,
	ODROID_UTIL_MALI
};

enum odroid_util_fields {
	ODROID_UTIL_ACTIVE,
	ODROID_UTIL_IDLE
};

// Only for library internal usage!
typedef struct __measurement_internal_odroid {
	struct timespec odroid_time_cur;
	struct timespec odroid_time_temp;
	struct timespec odroid_time_diff;
	double odroid_time_diff_double;
	uint64_t odroid_measure_util_cur[ODROID_NUM_UTIL][ODROID_NUM_UTIL_FIELDS];
	uint64_t odroid_measure_util_temp[ODROID_NUM_UTIL][ODROID_NUM_UTIL_FIELDS];
} MEASUREMENT_INTERNAL_ODROID;

typedef struct __ms_measurement_odroid {
	// onyl for internal usage
	MEASUREMENT_INTERNAL_ODROID internal;
	
	struct timespec odroid_time_wait;
	uint32_t odroid_check_for_exit_interrupts;
	
	double odroid_time_runtime;
	
	double odroid_power_cur[ODROID_NUM_POWER];
	double odroid_power_avg[ODROID_NUM_POWER];
	double odroid_energy_acc[ODROID_NUM_POWER];
	
	uint32_t odroid_temperature_cur[ODROID_NUM_TEMP];
	uint32_t odroid_temperature_max[ODROID_NUM_TEMP];
	
	uint32_t odroid_clock_cur[ODROID_NUM_FREQ];
	double odroid_clock_avg[ODROID_NUM_FREQ];
	double odroid_clock_acc[ODROID_NUM_FREQ];
} MS_MEASUREMENT_ODROID;

double odroid_time_total(MS_LIST *ms_list);
double odroid_energy_total_a15(MS_LIST *ms_list);
double odroid_energy_total_a7(MS_LIST *ms_list);
double odroid_energy_total_mali(MS_LIST *ms_list);
double odroid_energy_total_mem(MS_LIST *ms_list);
double odroid_power_avg_a15(MS_LIST *ms_list);
double odroid_power_avg_a7(MS_LIST *ms_list);
double odroid_power_avg_mali(MS_LIST *ms_list);
double odroid_power_avg_mem(MS_LIST *ms_list);
uint32_t odroid_temp_max_a15(MS_LIST *ms_list, int core);
uint32_t odroid_temp_max_mali(MS_LIST *ms_list);
double odroid_freq_avg_a15(MS_LIST *ms_list);
double odroid_freq_avg_a7(MS_LIST *ms_list);
double odroid_freq_avg_mali(MS_LIST *ms_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_ODROID_HARDKERNEL_XU4_H__ */
