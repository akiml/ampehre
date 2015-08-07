/*
 * measurement.h
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
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 3/04/14
 * version: 0.1.0 - initial implementation
 *          0.1.1 - add functionality to force FPGA to idle
 *          0.1.2 - add GPU frequency measurements
 *          0.1.3 - add CPU frequency measurements
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.5 - add CPU performance counters
 *          0.1.7 - add CPU utilization measurements
 *          0.1.8 - add GPU utilization measurements
 *          0.1.9 - add FPGA utilization measurements
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 */

#ifndef __MEASUREMENT_H__
#define __MEASUREMENT_H__

#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef __cplusplus
	//#include <boost/cstdint.hpp>
	//#include <ctime>
	extern "C" {
#endif /* __cplusplus */

#include "ms_driver.h"
#include "ms_ipmiwrapper.h"
#include "ms_version.h"

#define MAX_NUM_POWER 8
#define MAX_NUM_TEMPERATURE 2

#define MIC_NUM_POWER 7
#define MIC_NUM_TEMPERATURE 7
#define MIC_NUM_UTIL 4
#define MIC_CORES 57
#define MIC_THREADS 4

// Value from kernel (2.6.38.8+mpss3.4.1) which may change with a new kernel version
#define S_PER_JIFFY 0.01

#define CPUS 2
#define CORES 4

#define CPU		0x01
#define GPU		0x02
#define FPGA	0x04
#define SYSTEM	0x08
#define MIC 	0x10
#define ALL		(CPU | GPU | GPU | FPGA | SYSTEM | MIC)
		
		
#ifndef LIGHT
		
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
		
#else
		
#ifdef CPU_LIB
	#define CPU_LIB_NAME "libms_cpu_intel_xeon_sandy_light.so"
#else
	#define CPU_LIB_NAME "libms_stub.so"
#endif

#ifdef GPU_LIB
	#define GPU_LIB_NAME "libms_gpu_nvidia_tesla_kepler_light.so"
#else
	#define GPU_LIB_NAME "libms_stub.so"
#endif
		
#ifdef FPGA_LIB
	#define FPGA_LIB_NAME "libms_fpga_maxeler_max3a_light.so"
#else
	#define FPGA_LIB_NAME "libms_stub.so"
#endif
		
#ifdef SYS_LIB
	#define SYS_LIB_NAME "libms_sys_dell_idrac7_light.so"
#else
	#define SYS_LIB_NAME "libms_stub.so"
#endif
		
#ifdef MIC_LIB
	#define MIC_LIB_NAME "libms_mic_intel_knc_light.so"
#else
	#define MIC_LIB_NAME "libms_stub.so"
#endif
		
#endif
		
	

typedef void MSYSTEM;

enum maxeler_power {
	VCC10,
	VCC15,
	VCC25,
	IMGT10,
	IMGT12,
	MGT10,
	MGT12,
	POWER
};

enum maxeler_temperature {
	MTEMP,
	ITEMP
};

enum msr_power {
	PKG,
	PP0,
	DRAM
};

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

enum gpu_frequency {
	GPU_FREQUENCY_MIN,
	GPU_FREQUENCY_CUR,
	GPU_FREQUENCY_MAX,
	GPU_FREQUENCIES
};


enum cpu_governor {
	CPU_GOVERNOR_POWERSAVE,
	CPU_GOVERNOR_ONDEMAND,
	CPU_GOVERNOR_CONSERVATIVE,
	CPU_GOVERNOR_PERFORMANCE,
	CPU_GOVERNORS
};

// Only for library internal usage!
typedef struct __measurement_internal {
	// NVML
	struct timespec nvml_time_cur;
	struct timespec nvml_time_temp;
	struct timespec nvml_time_diff;
	double nvml_time_diff_double;
	int nvml_power_state;
	
	// Maxeler
	struct timespec maxeler_time_cur;
	struct timespec maxeler_time_temp;
	struct timespec maxeler_time_diff;
	double maxeler_time_diff_double;
	
	// IPMI
	struct timespec ipmi_time_cur;
	struct timespec ipmi_time_temp;
	struct timespec ipmi_time_diff;
	double ipmi_time_diff_double;
	
	// MSR
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
	
	// MIC
	struct timespec mic_time_cur;
	struct timespec mic_time_temp;
	struct timespec mic_time_diff;
	double mic_time_diff_double;
	uint64_t mic_util_cur[MIC_NUM_UTIL];
	uint64_t mic_util_temp[MIC_NUM_UTIL];
} MEASUREMENT_INTERNAL;

typedef struct __measurement {
	// onyl for internal usage
	MEASUREMENT_INTERNAL internal;
	
	// NVML
	struct timespec nvml_time_wait;
	
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
	
	// Maxeler
	struct timespec maxeler_time_wait;
	
	double maxeler_time_runtime;
	
	double maxeler_power_cur[MAX_NUM_POWER];
	double maxeler_power_avg[MAX_NUM_POWER];
	double maxeler_energy_acc[MAX_NUM_POWER];
	
	double maxeler_temperature_cur[MAX_NUM_TEMPERATURE];
	double maxeler_temperature_max[MAX_NUM_TEMPERATURE];
	
	double maxeler_util_comp_cur;
	double maxeler_util_comp_acc;
	double maxeler_util_comp_avg;
	
	// IPMI
	struct timespec ipmi_time_wait;
	
	double ipmi_time_runtime;
	
	double ipmi_temperature_sysboard_cur;
	double ipmi_temperature_sysboard_max;
	double ipmi_power_sysboard_cur;
	double ipmi_power_sysboard_avg;
	double ipmi_energy_sysboard_acc;
	double ipmi_temperature_cur[CPUS];
	double ipmi_temperature_max[CPUS];
	
	uint32_t ipmi_power_server_cur;
	double ipmi_power_server_avg;
	uint32_t ipmi_energy_server_acc;
	
	uint32_t ipmi_energy_server_acc_since_reset;
	double ipmi_power_server_avg_since_reset;
	
	// MSR
	struct timespec msr_time_wait;
	
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
	
	// MIC
	struct timespec mic_time_wait;
	
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
	
} MEASUREMENT;

// CPU
double cpu_time_total(MEASUREMENT *measurement);
double cpu_energy_total_pkg(MEASUREMENT *measurement, int cpu);
double cpu_energy_total_pp0(MEASUREMENT *measurement, int cpu);
double cpu_energy_total_dram(MEASUREMENT *measurement, int cpu);
double cpu_power_avg_pkg(MEASUREMENT *measurement, int cpu);
double cpu_power_avg_pp0(MEASUREMENT *measurement, int cpu);
double cpu_power_avg_dram(MEASUREMENT *measurement, int cpu);
uint32_t cpu_temp_max_pkg(MEASUREMENT *measurement, int cpu);
uint32_t cpu_temp_max_core(MEASUREMENT *measurement, int cpu, int core);
double cpu_freq_avg_core(MEASUREMENT *measurement, int cpu, int core);
double cpu_freq_eff_core(MEASUREMENT *measurement, int cpu, int core);
double cpu_active_avg_all(MEASUREMENT *measurement);
double cpu_idle_avg_all(MEASUREMENT *measurement);
double cpu_util_avg_all(MEASUREMENT *measurement);
uint32_t cpu_memory_total(MEASUREMENT *measurement);
uint32_t cpu_memory_used_max(MEASUREMENT *measurement);
uint32_t cpu_memory_free_max(MEASUREMENT *measurement);
uint32_t cpu_swap_total(MEASUREMENT *measurement);
uint32_t cpu_swap_used_max(MEASUREMENT *measurement);
uint32_t cpu_swap_free_max(MEASUREMENT *measurement);

// GPU
double gpu_time_total(MEASUREMENT *measurement);
double gpu_energy_total(MEASUREMENT *measurement);
double gpu_power_avg(MEASUREMENT *measurement);
uint32_t gpu_temp_max(MEASUREMENT *measurement);
double gpu_freq_avg_graph(MEASUREMENT *measurement);
double gpu_freq_avg_sm(MEASUREMENT *measurement);
double gpu_freq_avg_mem(MEASUREMENT *measurement);
double gpu_util_avg_gpu(MEASUREMENT *measurement);
double gpu_util_avg_mem(MEASUREMENT *measurement);
uint32_t gpu_memory_total(MEASUREMENT *measurement);
uint32_t gpu_memory_used_max(MEASUREMENT *measurement);
uint32_t gpu_memory_free_max(MEASUREMENT* measurement);

// FPGA
double fpga_time_total(MEASUREMENT *measurement);
double fpga_energy_total_vcc1v0_core(MEASUREMENT *measurement);
double fpga_energy_total_vcc1v5_ddr(MEASUREMENT *measurement);
double fpga_energy_total_vcc2v5_aux(MEASUREMENT *measurement);
double fpga_energy_total_imgt_1v0(MEASUREMENT *measurement);
double fpga_energy_total_imgt_1v2(MEASUREMENT *measurement);
double fpga_energy_total_mgt_1v0(MEASUREMENT *measurement);
double fpga_energy_total_mgt_1v2(MEASUREMENT *measurement);
double fpga_energy_total_power_usage(MEASUREMENT *measurement);
double fpga_power_avg_vcc1v0_core(MEASUREMENT *measurement);
double fpga_power_avg_vcc1v5_ddr(MEASUREMENT *measurement);
double fpga_power_avg_vcc2v5_aux(MEASUREMENT *measurement);
double fpga_power_avg_imgt_1v0(MEASUREMENT *measurement);
double fpga_power_avg_imgt_1v2(MEASUREMENT *measurement);
double fpga_power_avg_mgt_1v0(MEASUREMENT *measurement);
double fpga_power_avg_mgt_1v2(MEASUREMENT *measurement);
double fpga_power_avg_power_usage(MEASUREMENT *measurement);
double fpga_temp_max_ifpga(MEASUREMENT *measurement);
double fpga_temp_max_mfpga(MEASUREMENT *measurement);
double fpga_util_avg_comp(MEASUREMENT *measurement);

//MIC
double mic_time_total(MEASUREMENT *measurement);
double mic_energy_total_pcie(MEASUREMENT *measurement);
double mic_energy_total_c2x3(MEASUREMENT *measurement);
double mic_energy_total_c2x4(MEASUREMENT *measurement);
double mic_energy_total_vccp(MEASUREMENT *measurement);
double mic_energy_total_vddg(MEASUREMENT *measurement);
double mic_energy_total_vddq(MEASUREMENT *measurement);
double mic_energy_total_power_usage(MEASUREMENT *measurement);
double mic_power_avg_pcie(MEASUREMENT *measurement);
double mic_power_avg_c2x3(MEASUREMENT *measurement);
double mic_power_avg_c2x4(MEASUREMENT *measurement);
double mic_power_avg_vccp(MEASUREMENT *measurement);
double mic_power_avg_vddg(MEASUREMENT *measurement);
double mic_power_avg_vddq(MEASUREMENT *measurement);
double mic_power_avg_power_usage(MEASUREMENT *measurement);
uint32_t mic_temp_max_die(MEASUREMENT *measurement);
uint32_t mic_temp_max_gddr(MEASUREMENT *measurement);
uint32_t mic_temp_max_fan_in(MEASUREMENT *measurement);
uint32_t mic_temp_max_fan_out(MEASUREMENT *measurement);
uint32_t mic_temp_max_vccp(MEASUREMENT *measurement);
uint32_t mic_temp_max_vddg(MEASUREMENT *measurement);
uint32_t mic_temp_max_vddq(MEASUREMENT *measurement);
double mic_freq_avg_mem(MEASUREMENT *measurement);
double mic_freq_avg_core(MEASUREMENT *measurement);
double mic_active_avg_all(MEASUREMENT *measurement);
double mic_idle_avg_all(MEASUREMENT *measurement);
double mic_util_avg_all(MEASUREMENT *measurement);
uint32_t mic_memory_total(MEASUREMENT *measurement);
uint32_t mic_memory_used_max(MEASUREMENT *measurement);
uint32_t mic_memory_free_max(MEASUREMENT* measurement);

// System board
double sysboard_time_total(MEASUREMENT *measurement);
double sysboard_energy_total(MEASUREMENT *measurement);
double sysboard_power_avg(MEASUREMENT *measurement);
double sysboard_temp_max(MEASUREMENT *measurement);

// Server
double server_time_total(MEASUREMENT *measurement);
double server_energy_total(MEASUREMENT *measurement);
double server_power_avg(MEASUREMENT *measurement);

// Library management functions
MSYSTEM *ms_init(MS_VERSION* version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max, enum gpu_frequency gpu_freq);
void ms_init_fpga_force_idle(MSYSTEM *mgmt);
void ms_fini(MSYSTEM *mgmt);

MEASUREMENT *ms_alloc_measurement(void);
void ms_free_measurement(MEASUREMENT *measurement);

void ms_set_timer(MEASUREMENT *measurement, int flag, uint64_t sec, uint64_t nsec);

void ms_init_measurement(MSYSTEM *msystem, MEASUREMENT *measurement, int flags);
void ms_fini_measurement(MSYSTEM *msystem, MEASUREMENT* measurement);
void ms_start_measurement(MSYSTEM *msystem, MEASUREMENT *measurement);
void ms_stop_measurement(MSYSTEM *msystem, MEASUREMENT *measurement);
void ms_join_measurement(MSYSTEM *msystem, MEASUREMENT* measurement);

void ms_reg_sighandler_start(MSYSTEM *mgmt, void(*signal_handler)(int));
void ms_reg_sighandler_stop(MSYSTEM *mgmt, void(*signal_handler)(int));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEASUREMENT_H__ */
