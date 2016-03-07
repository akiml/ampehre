/*
 * ms_odroid_hardkernel_xu4.c
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

#include "ms_odroid_hardkernel_xu4.h"

#define MEASUREMENT_ODROID \
	(NULL == getMeasurement(&ms_list, ODROID)) ? 0 : ((MS_MEASUREMENT_ODROID *) getMeasurement(&ms_list, ODROID))

double odroid_time_total(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_time_runtime;
}

double odroid_energy_total_a15(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_energy_acc[ODROID_POWER_A15];
}

double odroid_energy_total_a7(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_energy_acc[ODROID_POWER_A7];
}

double odroid_energy_total_mali(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_energy_acc[ODROID_POWER_MALI];
}

double odroid_energy_total_mem(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_energy_acc[ODROID_POWER_MEM];
}

double odroid_power_avg_a15(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_power_avg[ODROID_POWER_A15];
}

double odroid_power_avg_a7(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_power_avg[ODROID_POWER_A7];
}

double odroid_power_avg_mali(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_power_avg[ODROID_POWER_MALI];
}

double odroid_power_avg_mem(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_power_avg[ODROID_POWER_MEM];
}

uint32_t odroid_temp_max_a15(MS_LIST* ms_list, int core) {
	return MEASUREMENT_ODROID->odroid_temperature_max[ODROID_TEMP_A15_CORE_0+core];
}

uint32_t odroid_temp_max_mali(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_temperature_max[ODROID_TEMP_MALI];
}

double odroid_freq_avg_a15(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_clock_avg[ODROID_FREQ_A15];
}

double odroid_freq_avg_a7(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_clock_avg[ODROID_FREQ_A7];
}

double odroid_freq_avg_mali(MS_LIST* ms_list) {
	return MEASUREMENT_ODROID->odroid_clock_avg[ODROID_FREQ_MALI];
}

double odroid_util_avg_a15(MS_LIST *ms_list) {
	return MEASUREMENT_ODROID->odroid_util_avg[ODROID_UTIL_A15];
}

double odroid_util_avg_a7(MS_LIST *ms_list) {
	return MEASUREMENT_ODROID->odroid_util_avg[ODROID_UTIL_A7];
}

double odroid_util_avg_mali(MS_LIST *ms_list) {
	return MEASUREMENT_ODROID->odroid_util_avg[ODROID_UTIL_MALI];
}
