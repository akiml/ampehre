/*
 * ms_measurement.h
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

#ifndef __MS_MEASUREMENT_H__
#define __MS_MEASUREMENT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>

#include "ms_driver.h"
#include "ms_ipmiwrapper.h"
#include "ms_version.h"
#include "ms_cpu_intel_xeon_sandy.h"
#include "ms_gpu_nvidia_tesla_kepler.h"
#include "ms_fpga_maxeler_max3a.h"
#include "ms_mic_intel_knc.h"
#include "ms_sys_dell_idrac7.h"

// Value from kernel (2.6.38.8+mpss3.4.1) which may change with a new kernel version
#define S_PER_JIFFY 0.01

#define CPU		0x01
#define GPU		0x02
#define FPGA	0x04
#define SYSTEM	0x08
#define MIC 	0x10
#define ALL		(CPU | GPU | FPGA | SYSTEM | MIC)
		
#ifdef CPU_LIB
	#define CPU_LIB_NAME "libms_cpu_intel_xeon_sandy.so"
#else
	#define CPU_LIB_NAME "libms_stub.so"
#endif

#ifdef GPU_LIB
	#define GPU_LIB_NAME "libms_gpu_nvidia_tesla_kepler.so"
#else
	#define GPU_LIB_NAME "libms_stub.so"
#endif
		
#ifdef FPGA_LIB
	#define FPGA_LIB_NAME "libms_fpga_maxeler_max3a.so"
#else
	#define FPGA_LIB_NAME "libms_stub.so"
#endif
		
#ifdef SYS_LIB
	#define SYS_LIB_NAME "libms_sys_dell_idrac7.so"
#else
	#define SYS_LIB_NAME "libms_stub.so"
#endif
		
#ifdef MIC_LIB
	#define MIC_LIB_NAME "libms_mic_intel_knc.so"
#else
	#define MIC_LIB_NAME "libms_stub.so"
#endif

enum skip_ms_rate {
	SKIP_PERIODIC,
	SKIP_NEVER,
	SKIP_MS_RATES
};

enum lib_variant {
	VARIANT_LIGHT,
	VARIANT_FULL,
	VARIANTS
};

typedef struct __ms_config {
	int32_t cpu_enabled;
	int32_t gpu_enabled;
	int32_t fpga_enabled;
	int32_t mic_enabled;
	int32_t sys_enabled;
} MS_CONFIG;

typedef struct __ms_list {
	int32_t resource_id;
	void *ms_measurement;
	struct __ms_list *next;
} MS_LIST;

typedef struct __ms_system{
	void *mgmt;
	MS_CONFIG *config;
} MS_SYSTEM;

// Library management functions
MS_SYSTEM *ms_init(MS_VERSION* version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
				 enum gpu_frequency gpu_freq, uint64_t ipmi_timeout_setting, enum skip_ms_rate skip_ms, enum lib_variant variant);
void ms_init_fpga_force_idle(MS_SYSTEM *ms_system);
void ms_fini(MS_SYSTEM *ms_system);

MS_LIST *ms_alloc_measurement(MS_SYSTEM *ms_system);
void ms_free_measurement(MS_LIST *ms_list);

void ms_set_timer(MS_LIST *ms_list, int flag, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts);

void ms_init_measurement(MS_SYSTEM *ms_system, MS_LIST *ms_list, int flags);
void ms_fini_measurement(MS_SYSTEM *ms_system);
void ms_start_measurement(MS_SYSTEM *ms_system);
void ms_stop_measurement(MS_SYSTEM *ms_system);
void ms_join_measurement(MS_SYSTEM *ms_system);

void ms_reg_sighandler_start(MS_SYSTEM *ms_system, void(*signal_handler)(int));
void ms_reg_sighandler_stop(MS_SYSTEM *ms_system, void(*signal_handler)(int));

//Functions for MS_LIST
void appendList(MS_LIST **ms_list, int id);
void *getMeasurement(MS_LIST **ms_list, int id);
void deleteList(MS_LIST **ms_list);

//TODO: This functions should be moved to the resource specific modules
// CPU
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

// GPU
double gpu_time_total(MS_LIST *ms_list);
double gpu_energy_total(MS_LIST *ms_list);
double gpu_power_avg(MS_LIST *ms_list);
uint32_t gpu_temp_max(MS_LIST *ms_list);
double gpu_freq_avg_graph(MS_LIST *ms_list);
double gpu_freq_avg_sm(MS_LIST *ms_list);
double gpu_freq_avg_mem(MS_LIST *ms_list);
double gpu_util_avg_gpu(MS_LIST *ms_list);
double gpu_util_avg_mem(MS_LIST *ms_list);
uint32_t gpu_memory_total(MS_LIST *ms_list);
uint32_t gpu_memory_used_max(MS_LIST *ms_list);
uint32_t gpu_memory_free_max(MS_LIST *ms_list);

// FPGA
double fpga_time_total(MS_LIST *ms_list);
double fpga_energy_total_vcc1v0_core(MS_LIST *ms_list);
double fpga_energy_total_vcc1v5_ddr(MS_LIST *ms_list);
double fpga_energy_total_vcc2v5_aux(MS_LIST *ms_list);
double fpga_energy_total_imgt_1v0(MS_LIST *ms_list);
double fpga_energy_total_imgt_1v2(MS_LIST *ms_list);
double fpga_energy_total_mgt_1v0(MS_LIST *ms_list);
double fpga_energy_total_mgt_1v2(MS_LIST *ms_list);
double fpga_energy_total_power_usage(MS_LIST *ms_list);
double fpga_power_avg_vcc1v0_core(MS_LIST *ms_list);
double fpga_power_avg_vcc1v5_ddr(MS_LIST *ms_list);
double fpga_power_avg_vcc2v5_aux(MS_LIST *ms_list);
double fpga_power_avg_imgt_1v0(MS_LIST *ms_list);
double fpga_power_avg_imgt_1v2(MS_LIST *ms_list);
double fpga_power_avg_mgt_1v0(MS_LIST *ms_list);
double fpga_power_avg_mgt_1v2(MS_LIST *ms_list);
double fpga_power_avg_power_usage(MS_LIST *ms_list);
double fpga_temp_max_ifpga(MS_LIST *ms_list);
double fpga_temp_max_mfpga(MS_LIST *ms_list);
double fpga_util_avg_comp(MS_LIST *ms_list);

//MIC
double mic_time_total(MS_LIST *ms_list);
double mic_energy_total_pcie(MS_LIST *ms_list);
double mic_energy_total_c2x3(MS_LIST *ms_list);
double mic_energy_total_c2x4(MS_LIST *ms_list);
double mic_energy_total_vccp(MS_LIST *ms_list);
double mic_energy_total_vddg(MS_LIST *ms_list);
double mic_energy_total_vddq(MS_LIST *ms_list);
double mic_energy_total_power_usage(MS_LIST *ms_list);
double mic_power_avg_pcie(MS_LIST *ms_list);
double mic_power_avg_c2x3(MS_LIST *ms_list);
double mic_power_avg_c2x4(MS_LIST *ms_list);
double mic_power_avg_vccp(MS_LIST *ms_list);
double mic_power_avg_vddg(MS_LIST *ms_list);
double mic_power_avg_vddq(MS_LIST *ms_list);
double mic_power_avg_power_usage(MS_LIST *ms_list);
uint32_t mic_temp_max_die(MS_LIST *ms_list);
uint32_t mic_temp_max_gddr(MS_LIST *ms_list);
uint32_t mic_temp_max_fan_in(MS_LIST *ms_list);
uint32_t mic_temp_max_fan_out(MS_LIST *ms_list);
uint32_t mic_temp_max_vccp(MS_LIST *ms_list);
uint32_t mic_temp_max_vddg(MS_LIST *ms_list);
uint32_t mic_temp_max_vddq(MS_LIST *ms_list);
double mic_freq_avg_mem(MS_LIST *ms_list);
double mic_freq_avg_core(MS_LIST *ms_list);
double mic_active_avg_all(MS_LIST *ms_list);
double mic_idle_avg_all(MS_LIST *ms_list);
double mic_util_avg_all(MS_LIST *ms_list);
uint32_t mic_memory_total(MS_LIST *ms_list);
uint32_t mic_memory_used_max(MS_LIST *ms_list);
uint32_t mic_memory_free_max(MS_LIST *ms_list);

// System
double system_time_total(MS_LIST *ms_list);
double system_energy_board(MS_LIST *ms_list);
double system_power_board_avg(MS_LIST *ms_list);
double system_temp_max(MS_LIST *ms_list);
double system_energy_total(MS_LIST *ms_list);
double system_power_avg(MS_LIST *ms_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_MEASUREMENT_H__ */
