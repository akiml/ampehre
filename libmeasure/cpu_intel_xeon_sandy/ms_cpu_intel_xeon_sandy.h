/*
 * ms_cpu_intel_xeon_sandy.h
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
 *          0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 */

#ifndef __MS_CPU_INTEL_XEON_SANDY_H__
#define __MS_CPU_INTEL_XEON_SANDY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <time.h>

#include "ms_list.h"
#include "ms_driver.h"

#define CPUS 2
#define CORES 4

enum cpu_governor {
	CPU_GOVERNOR_POWERSAVE,
	CPU_GOVERNOR_ONDEMAND,
	CPU_GOVERNOR_CONSERVATIVE,
	CPU_GOVERNOR_PERFORMANCE,
	CPU_GOVERNORS
};

enum msr_power {
	PKG,
	PP0,
	DRAM
};

// Only for library internal usage!
typedef struct __measurement_internal_cpu {
	struct timespec msr_time_cur;
	struct timespec msr_time_temp;
	struct timespec msr_time_diff;
	double msr_time_diff_double;
	double msr_energy_temp[CPUS][3];
	double msr_energy_max;
	uint64_t msr_timestamp_core_temp[CPUS][CORES];
	uint64_t msr_timestamp_core_cur[CPUS][CORES];
	uint64_t msr_aperf_core_cur[CPUS][CORES];
	uint64_t msr_aperf_core_temp[CPUS][CORES];
	uint64_t msr_mperf_core_cur[CPUS][CORES];
	uint64_t msr_mperf_core_temp[CPUS][CORES];
	uint64_t measure_util_cur[CPUSTATS];
	uint64_t measure_util_temp[CPUSTATS];
} MEASUREMENT_INTERNAL_CPU;

typedef struct __ms_measurement_cpu {
	// onyl for internal usage
	MEASUREMENT_INTERNAL_CPU internal;
	
	struct timespec msr_time_wait;
	uint32_t msr_check_for_exit_interrupts;
	
	double msr_time_runtime;
	
	double msr_energy_cur[CPUS][3];
	double msr_power_cur[CPUS][3];
	double msr_power_avg[CPUS][3];
	double msr_energy_acc[CPUS][3];
	
	uint32_t msr_temperature_pkg_cur[CPUS];
	uint32_t msr_temperature_pkg_max[CPUS];
	uint32_t msr_temperature_core_cur[CPUS][CORES];
	uint32_t msr_temperature_core_max[CPUS][CORES];
	
	double msr_freq_core_cur[CPUS][CORES];
	double msr_freq_core_acc[CPUS][CORES];
	double msr_freq_core_avg[CPUS][CORES];
	
	double msr_freq_core_eff_cur[CPUS][CORES];
	double msr_freq_core_eff_acc[CPUS][CORES];
	double msr_freq_core_eff_avg[CPUS][CORES];
	
	uint64_t measure_util_cur[CPUSTATS];
	uint64_t measure_util_acc[CPUSTATS];
	uint64_t measure_util_active_total;
	uint64_t measure_util_idle_total;
	double measure_util_active_cur;
	double measure_util_idle_cur;
	double measure_util_avg_cur;
	double measure_util_active_avg;
	double measure_util_idle_avg;
	double measure_util_avg;
	uint64_t measure_memory_cur[MEMINFO_SIZE];
	uint32_t measure_memory_free_max;
	uint32_t measure_memory_used_max;
	uint32_t measure_swap_free_max;
	uint32_t measure_swap_used_max;
} MS_MEASUREMENT_CPU;

double cpu_time_total(MS_LIST *ms_list);
double cpu_energy_total_pkg(MS_LIST *ms_list, int cpu);
double cpu_energy_total_pp0(MS_LIST *ms_list, int cpu);
double cpu_energy_total_dram(MS_LIST *ms_list, int cpu);
double cpu_power_avg_pkg(MS_LIST *ms_list, int cpu);
double cpu_power_avg_pp0(MS_LIST *ms_list, int cpu);
double cpu_power_avg_dram(MS_LIST *ms_list, int cpu);
uint32_t cpu_temp_max_pkg(MS_LIST *ms_list, int cpu);
uint32_t cpu_temp_max_core(MS_LIST *ms_list, int cpu, int core);
double cpu_freq_avg_core(MS_LIST *ms_list, int cpu, int core);
double cpu_freq_eff_core(MS_LIST *ms_list, int cpu, int core);
double cpu_active_avg_all(MS_LIST *ms_list);
double cpu_idle_avg_all(MS_LIST *ms_list);
double cpu_util_avg_all(MS_LIST *ms_list);
uint32_t cpu_memory_total(MS_LIST *ms_list);
uint32_t cpu_memory_used_max(MS_LIST *ms_list);
uint32_t cpu_memory_free_max(MS_LIST *ms_list);
uint32_t cpu_swap_total(MS_LIST *ms_list);
uint32_t cpu_swap_used_max(MS_LIST *ms_list);
uint32_t cpu_swap_free_max(MS_LIST *ms_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_CPU_INTEL_XEON_SANDY_H__ */
