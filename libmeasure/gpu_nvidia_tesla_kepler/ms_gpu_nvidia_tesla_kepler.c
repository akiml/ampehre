/*
 * ms_gpu_nvidia_tesla_kepler.c
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
 * author: Achim Lösch
 * created: 01/25/16
 * version: 0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 */

#include "ms_gpu_nvidia_tesla_kepler.h"

double gpu_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_time_runtime;
}

double gpu_energy_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_energy_acc;
}

double gpu_power_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_power_avg;
}

uint32_t gpu_temp_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_temperature_max;
}

double gpu_freq_avg_graph(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_clock_graphics_avg;
}

double gpu_freq_avg_sm(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_clock_sm_avg;
}

double gpu_freq_avg_mem(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_clock_mem_avg;
}

double gpu_util_avg_gpu(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_util_gpu_avg;
}

double gpu_util_avg_mem(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_util_mem_avg;
}

uint32_t gpu_memory_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_memory_total;
}

uint32_t gpu_memory_used_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_memory_used_max;
}

uint32_t gpu_memory_free_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_memory_free_max;
}

uint32_t gpu_active_processes_max ( MS_LIST* ms_list ) {
	MS_MEASUREMENT_GPU *ms_measurement_gpu = (MS_MEASUREMENT_GPU *) getMeasurement(&ms_list, GPU);
	NULL_CHECK(ms_measurement_gpu);
	
	return ms_measurement_gpu->nvml_active_processes_count_max;
}

