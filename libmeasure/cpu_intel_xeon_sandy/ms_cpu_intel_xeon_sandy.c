/*
 * ms_cpu_intel_xeon_sandy.c
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

#include "ms_cpu_intel_xeon_sandy.h"

#define MEASUREMENT_CPU \
	(NULL == getMeasurement(&ms_list, CPU)) ? 0 : ((MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU))

double cpu_time_total(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->msr_time_runtime;
}

double cpu_energy_total_pkg(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_energy_acc[cpu][PKG];
}

double cpu_energy_total_pp0(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_energy_acc[cpu][PP0];
}

double cpu_energy_total_dram(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_energy_acc[cpu][DRAM];
}

double cpu_power_avg_pkg(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_power_avg[cpu][PKG];
}

double cpu_power_avg_pp0(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_power_avg[cpu][PP0];
}

double cpu_power_avg_dram(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_power_avg[cpu][DRAM];
}

double cpu_v2freq_total(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_v2freq_acc[cpu];
}

uint64_t cpu_pstate_total(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_pstate_acc[cpu];
}

uint32_t cpu_temp_max_pkg(MS_LIST *ms_list, int cpu) {
	return MEASUREMENT_CPU->msr_temperature_pkg_max[cpu];
}

uint32_t cpu_temp_max_core(MS_LIST *ms_list, int cpu, int core) {
	return MEASUREMENT_CPU->msr_temperature_core_max[cpu][core];
}

double cpu_freq_avg_core(MS_LIST *ms_list, int cpu, int core) {
	return MEASUREMENT_CPU->msr_freq_core_avg[cpu][core];
}

double cpu_freq_eff_core(MS_LIST *ms_list, int cpu, int core) {
	return MEASUREMENT_CPU->msr_freq_core_eff_avg[cpu][core];
}

double cpu_active_avg_all(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_util_active_avg;
}

double cpu_idle_avg_all(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_util_idle_avg;
}

double cpu_util_avg_all(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_util_avg;
}

uint32_t cpu_memory_total(MS_LIST *ms_list) {
	return (uint32_t) MEASUREMENT_CPU->measure_memory_cur[CPU_MEM_RAM_TOTAL];
}

uint32_t cpu_memory_used_max(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_memory_used_max;
}

uint32_t cpu_memory_free_max(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_memory_free_max;
}

uint32_t cpu_swap_total(MS_LIST *ms_list) {
	return (uint32_t) MEASUREMENT_CPU->measure_memory_cur[CPU_MEM_SWAP_TOTAL];
}

uint32_t cpu_swap_used_max(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_swap_used_max;
}

uint32_t cpu_swap_free_max(MS_LIST *ms_list) {
	return MEASUREMENT_CPU->measure_swap_free_max;
}