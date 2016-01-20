/*
 * ms_mic_intel_knc.h
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
 * version: 0.7.0 - modularized measurement struct
 *          0.7.1 - move functions to query measurement results into the modules
 */

#ifndef __MS_MIC_INTEL_KNC_H__
#define __MS_MIC_INTEL_KNC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
#include "ms_list.h"

#define MIC_NUM_POWER 7
#define MIC_NUM_TEMPERATURE 7
#define MIC_NUM_UTIL 4
#define MIC_CORES 57
#define MIC_THREADS 4

enum mic_power {
	MIC_PCIE,
	MIC_C2X3,
	MIC_C2X4,
	MIC_VCCP,
	MIC_VDDG,
	MIC_VDDQ,
	MIC_POWER	
};

enum mic_temperature {
	MIC_DIE_TEMP,
	MIC_GDDR_TEMP,
	MIC_FAN_IN_TEMP,
	MIC_FAN_OUT_TEMP,
	MIC_VCCP_TEMP,
	MIC_VDDG_TEMP,
	MIC_VDDQ_TEMP
};

enum mic_util {
	IDLE_SUM,
	SYS_SUM,
	NICE_SUM,
	USER_SUM,
};

// Only for library internal usage!
typedef struct __measurement_internal_mic {
	struct timespec mic_time_cur;
	struct timespec mic_time_temp;
	struct timespec mic_time_diff;
	double mic_time_diff_double;
	uint64_t mic_util_cur[MIC_NUM_UTIL];
	uint64_t mic_util_temp[MIC_NUM_UTIL];
} MEASUREMENT_INTERNAL_MIC;

typedef struct __ms_measurement_mic {
	// onyl for internal usage
	MEASUREMENT_INTERNAL_MIC internal;
	
	struct timespec mic_time_wait;
	uint32_t mic_check_for_exit_interrupts;
	
	double mic_time_runtime;
	
	uint32_t mic_freq_core_cur;
	double mic_freq_core_acc;
	double mic_freq_core_avg;
	
	uint32_t mic_freq_mem_cur;
	double mic_freq_mem_acc;
	double mic_freq_mem_avg;
	
	uint32_t mic_power_cur[MIC_NUM_POWER];
	double mic_power_avg[MIC_NUM_POWER];
	double mic_energy_acc[MIC_NUM_POWER];
	
	uint32_t mic_temperature_cur[MIC_NUM_TEMPERATURE];
	uint32_t mic_temperature_max[MIC_NUM_TEMPERATURE];
	
	uint64_t mic_util_cur[MIC_NUM_UTIL];
	uint64_t mic_util_acc[MIC_NUM_UTIL];
	uint64_t mic_util_active_total;
	uint64_t mic_util_idle_total;
	double mic_util_active_cur;
	double mic_util_idle_cur;
	double mic_util_avg_cur;
	double mic_util_active_avg;
	double mic_util_idle_avg;
	double mic_util_avg;
	
	uint32_t mic_memory_free_cur;
	uint32_t mic_memory_used_cur;
	uint32_t mic_memory_free_max;
	uint32_t mic_memory_used_max;
	uint32_t mic_memory_total;
} MS_MEASUREMENT_MIC;

inline double mic_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_time_runtime;
}

inline double mic_energy_total_pcie(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_PCIE];
}

inline double mic_energy_total_c2x3(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_C2X3];
}

inline double mic_energy_total_c2x4(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_C2X4];
}

inline double mic_energy_total_vccp(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_VCCP];
}

inline double mic_energy_total_vddg(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_VDDG];
}

inline double mic_energy_total_vddq(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_VDDQ];
}

inline double mic_energy_total_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_POWER];
}

inline double mic_power_avg_pcie(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_PCIE];
}

inline double mic_power_avg_c2x3(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_C2X3];
}

inline double mic_power_avg_c2x4(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_C2X4];
}

inline double mic_power_avg_vccp(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_VCCP];
}

inline double mic_power_avg_vddg(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_VDDG];
}

inline double mic_power_avg_vddq(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_VDDQ];
}

inline double mic_power_avg_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_POWER];
}

inline uint32_t mic_temp_max_die(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_DIE_TEMP];
}

inline uint32_t mic_temp_max_gddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_GDDR_TEMP];
}

inline uint32_t mic_temp_max_fan_in(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_FAN_IN_TEMP];
}

inline uint32_t mic_temp_max_fan_out(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_FAN_OUT_TEMP];
}

inline uint32_t mic_temp_max_vccp(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_VCCP_TEMP];
}

inline uint32_t mic_temp_max_vddg(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_VDDG_TEMP];
}

inline uint32_t mic_temp_max_vddq(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_VDDQ_TEMP];
}

inline double mic_freq_avg_mem(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_freq_mem_avg;
}

inline double mic_freq_avg_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_freq_core_avg;
}

inline double mic_active_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_util_active_avg;
}

inline double mic_idle_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_util_idle_avg;
}

inline double mic_util_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_util_avg;
}

inline uint32_t mic_memory_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_memory_total;
}

inline uint32_t mic_memory_used_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_memory_used_max;
}

inline uint32_t mic_memory_free_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_memory_free_max;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_MIC_INTEL_KNC_H__ */