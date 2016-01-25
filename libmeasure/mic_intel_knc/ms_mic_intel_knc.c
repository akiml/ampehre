/*
 * ms_mic_intel_knc.c
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
 */

#include "ms_mic_intel_knc.h"

inline static MS_MEASUREMENT_MIC *get_measurement_mic(MS_LIST *ms_list);

inline static MS_MEASUREMENT_MIC *get_measurement_mic(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic;
}

double mic_time_total(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_time_runtime);
}

double mic_energy_total_pcie(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_PCIE]);
}

double mic_energy_total_c2x3(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_C2X3]);
}

double mic_energy_total_c2x4(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_C2X4]);
}

double mic_energy_total_vccp(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_VCCP]);
}

double mic_energy_total_vddg(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_VDDG]);
}

double mic_energy_total_vddq(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_VDDQ]);
}

double mic_energy_total_power_usage(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_energy_acc[MIC_POWER]);
}

double mic_power_avg_pcie(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_PCIE]);
}

double mic_power_avg_c2x3(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_C2X3]);
}

double mic_power_avg_c2x4(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_C2X4]);
}

double mic_power_avg_vccp(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_VCCP]);
}

double mic_power_avg_vddg(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_VDDG]);
}

double mic_power_avg_vddq(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_VDDQ]);
}

double mic_power_avg_power_usage(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_power_avg[MIC_POWER]);
}

uint32_t mic_temp_max_die(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_DIE_TEMP]);
}

uint32_t mic_temp_max_gddr(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_GDDR_TEMP]);
}

uint32_t mic_temp_max_fan_in(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_FAN_IN_TEMP]);
}

uint32_t mic_temp_max_fan_out(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_FAN_OUT_TEMP]);
}

uint32_t mic_temp_max_vccp(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_VCCP_TEMP]);
}

uint32_t mic_temp_max_vddg(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_VDDG_TEMP]);
}

uint32_t mic_temp_max_vddq(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_temperature_max[MIC_VDDQ_TEMP]);
}

double mic_freq_avg_mem(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_freq_mem_avg);
}

double mic_freq_avg_core(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_freq_core_avg);
}

double mic_active_avg_all(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_util_active_avg);
}

double mic_idle_avg_all(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_util_idle_avg);
}

double mic_util_avg_all(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_util_avg);
}

uint32_t mic_memory_total(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_memory_total);
}

uint32_t mic_memory_used_max(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_memory_used_max);
}

uint32_t mic_memory_free_max(MS_LIST *ms_list) {
	return (get_measurement_mic(ms_list)->mic_memory_free_max);
}
