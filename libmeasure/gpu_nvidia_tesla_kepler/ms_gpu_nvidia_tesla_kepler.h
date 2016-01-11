/*
 * ms_gpu_nvidia_tesla_kepler.h
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
 */

#ifndef __MS_GPU_NVIDIA_TESLA_KEPLER_H__
#define __MS_GPU_NVIDIA_TESLA_KEPLER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum gpu_frequency {
	GPU_FREQUENCY_MIN,
	GPU_FREQUENCY_CUR,
	GPU_FREQUENCY_MAX,
	GPU_FREQUENCIES
};


// Only for library internal usage!
typedef struct __measurement_internal_gpu {
	struct timespec nvml_time_cur;
	struct timespec nvml_time_temp;
	struct timespec nvml_time_diff;
	double nvml_time_diff_double;
	int nvml_power_state;
} MEASUREMENT_INTERNAL_GPU;

typedef struct __ms_measurement_gpu {
	// onyl for internal usage
	MEASUREMENT_INTERNAL_GPU internal;
	
	struct timespec nvml_time_wait;
	uint32_t nvml_check_for_exit_interrupts;
	
	double nvml_time_runtime;
	
	uint32_t nvml_power_cur;
	double nvml_power_avg;
	double nvml_energy_acc;
	
	uint32_t nvml_temperature_cur;
	uint32_t nvml_temperature_max;
	
	uint32_t nvml_clock_graphics_cur;
	uint32_t nvml_clock_sm_cur;
	uint32_t nvml_clock_mem_cur;
	double nvml_clock_graphics_avg;
	double nvml_clock_sm_avg;
	double nvml_clock_mem_avg;
	double nvml_clock_graphics_acc;
	double nvml_clock_sm_acc;
	double nvml_clock_mem_acc;
	uint32_t nvml_util_gpu_cur;
	uint32_t nvml_util_mem_cur;
	double nvml_util_gpu_avg;
	double nvml_util_mem_avg;
	double nvml_util_gpu_acc;
	double nvml_util_mem_acc;
	
	uint32_t nvml_memory_free_cur;
	uint32_t nvml_memory_used_cur;
	uint32_t nvml_memory_free_max;
	uint32_t nvml_memory_used_max;
	uint32_t nvml_memory_total;
} MS_MEASUREMENT_GPU;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_GPU_NVIDIA_TESLA_KEPLER_H__ */