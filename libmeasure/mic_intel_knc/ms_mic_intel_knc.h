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
 * version: 0.7.0 - modularised measurement struct
 */

#ifndef __MS_MIC_INTEL_KNC_H__
#define __MS_MIC_INTEL_KNC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_MIC_INTEL_KNC_H__ */