/*
 * interface.c
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
 * created: 3/20/14
 * version: 0.1.0 - initial implementation
 *          0.1.2 - add GPU frequency measurements
 *          0.1.3 - add CPU frequency measurements
 *          0.1.5 - add CPU performance counters
 *          0.1.7 - add CPU utilization measurements
 *          0.1.8 - add GPU utilization measurements
 *          0.1.9 - add FPGA utilization measurements
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 */

#include "../../include/measurement.h"

// CPU
double cpu_time_total(MEASUREMENT *measurement) {
	return measurement->msr_time_runtime;
}

double cpu_energy_total_pkg(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_energy_acc[cpu][PKG];
}

double cpu_energy_total_pp0(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_energy_acc[cpu][PP0];
}

double cpu_energy_total_dram(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_energy_acc[cpu][DRAM];
}

double cpu_power_avg_pkg(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_power_avg[cpu][PKG];
}

double cpu_power_avg_pp0(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_power_avg[cpu][PP0];
}

double cpu_power_avg_dram(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_power_avg[cpu][DRAM];
}

uint32_t cpu_temp_max_pkg(MEASUREMENT *measurement, int cpu) {
	return measurement->msr_temperature_pkg_max[cpu];
}

uint32_t cpu_temp_max_core(MEASUREMENT *measurement, int cpu, int core) {
	return measurement->msr_temperature_core_max[cpu][core];
}

double cpu_freq_avg_core(MEASUREMENT *measurement, int cpu, int core) {
	return measurement->msr_freq_core_avg[cpu][core];
}

double cpu_freq_eff_core(MEASUREMENT *measurement, int cpu, int core) {
	return measurement->msr_freq_core_eff_avg[cpu][core];
}

double cpu_active_avg_all(MEASUREMENT *measurement) {
	return measurement->measure_util_active_avg;
}

double cpu_idle_avg_all(MEASUREMENT *measurement) {
	return measurement->measure_util_idle_avg;
}

double cpu_util_avg_all(MEASUREMENT *measurement) {
	return measurement->measure_util_avg;
}

uint32_t cpu_memory_total(MEASUREMENT *measurement) {
	return (uint32_t) measurement->measure_memory_cur[CPU_MEM_RAM_TOTAL];
}

uint32_t cpu_memory_used_max(MEASUREMENT *measurement) {
	return measurement->measure_memory_used_max;
}

uint32_t cpu_memory_free_max(MEASUREMENT *measurement) {
	return measurement->measure_memory_free_max;
}

uint32_t cpu_swap_total(MEASUREMENT *measurement) {
	return (uint32_t) measurement->measure_memory_cur[CPU_MEM_SWAP_TOTAL];
}

uint32_t cpu_swap_used_max(MEASUREMENT *measurement) {
	return measurement->measure_swap_used_max;
}

uint32_t cpu_swap_free_max(MEASUREMENT *measurement) {
	return measurement->measure_swap_free_max;
}

// GPU
double gpu_time_total(MEASUREMENT *measurement) {
	return measurement->nvml_time_runtime;
}

double gpu_energy_total(MEASUREMENT *measurement) {
	return measurement->nvml_energy_acc;
}

double gpu_power_avg(MEASUREMENT *measurement) {
	return measurement->nvml_power_avg;
}

uint32_t gpu_temp_max(MEASUREMENT *measurement) {
	return measurement->nvml_temperature_max;
}

double gpu_freq_avg_graph(MEASUREMENT *measurement) {
	return measurement->nvml_clock_graphics_avg;
}

double gpu_freq_avg_sm(MEASUREMENT *measurement) {
	return measurement->nvml_clock_sm_avg;
}

double gpu_freq_avg_mem(MEASUREMENT *measurement) {
	return measurement->nvml_clock_mem_avg;
}

double gpu_util_avg_gpu(MEASUREMENT *measurement) {
	return measurement->nvml_util_gpu_avg;
}

double gpu_util_avg_mem(MEASUREMENT *measurement) {
	return measurement->nvml_util_mem_avg;
}

uint32_t gpu_memory_total(MEASUREMENT *measurement) {
	return measurement->nvml_memory_total;
}

uint32_t gpu_memory_used_max(MEASUREMENT *measurement) {
	return measurement->nvml_memory_used_max;
}

uint32_t gpu_memory_free_max(MEASUREMENT* measurement) {
	return measurement->nvml_memory_free_max;
}

// FPGA
double fpga_time_total(MEASUREMENT *measurement) {
	return measurement->maxeler_time_runtime;
}

double fpga_energy_total_vcc1v0_core(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[VCC10];
}

double fpga_energy_total_vcc1v5_ddr(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[VCC15];
}

double fpga_energy_total_vcc2v5_aux(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[VCC25];
}

double fpga_energy_total_imgt_1v0(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[IMGT10];
}

double fpga_energy_total_imgt_1v2(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[IMGT12];
}

double fpga_energy_total_mgt_1v0(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[MGT10];
}

double fpga_energy_total_mgt_1v2(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[MGT12];
}

double fpga_energy_total_power_usage(MEASUREMENT *measurement) {
	return measurement->maxeler_energy_acc[POWER];
}

double fpga_power_avg_vcc1v0_core(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[VCC10];
}

double fpga_power_avg_vcc1v5_ddr(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[VCC15];
}

double fpga_power_avg_vcc2v5_aux(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[VCC25];
}

double fpga_power_avg_imgt_1v0(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[IMGT10];
}

double fpga_power_avg_imgt_1v2(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[IMGT12];
}

double fpga_power_avg_mgt_1v0(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[MGT10];
}

double fpga_power_avg_mgt_1v2(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[MGT12];
}

double fpga_power_avg_power_usage(MEASUREMENT *measurement) {
	return measurement->maxeler_power_avg[POWER];
}

double fpga_temp_max_ifpga(MEASUREMENT *measurement) {
	return measurement->maxeler_temperature_max[ITEMP];
}

double fpga_temp_max_mfpga(MEASUREMENT *measurement) {
	return measurement->maxeler_temperature_max[MTEMP];
}

double fpga_util_avg_comp(MEASUREMENT *measurement) {
	return measurement->maxeler_util_comp_avg;
}

//MIC
double mic_time_total(MEASUREMENT *measurement) {
	return measurement->mic_time_runtime;
}

double mic_energy_total_pcie(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_PCIE];
}

double mic_energy_total_c2x3(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_C2X3];
}

double mic_energy_total_c2x4(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_C2X4];
}

double mic_energy_total_vccp(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_VCCP];
}

double mic_energy_total_vddg(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_VDDG];
}

double mic_energy_total_vddq(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_VDDQ];
}

double mic_energy_total_power_usage(MEASUREMENT *measurement) {
	return measurement->mic_energy_acc[MIC_POWER];
}

double mic_power_avg_pcie(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_PCIE];
}

double mic_power_avg_c2x3(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_C2X3];
}

double mic_power_avg_c2x4(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_C2X4];
}

double mic_power_avg_vccp(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_VCCP];
}

double mic_power_avg_vddg(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_VDDG];
}

double mic_power_avg_vddq(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_VDDQ];
}

double mic_power_avg_power_usage(MEASUREMENT *measurement) {
	return measurement->mic_power_avg[MIC_POWER];
}

uint32_t mic_temp_max_die(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_DIE_TEMP];
}

uint32_t mic_temp_max_gddr(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_GDDR_TEMP];
}

uint32_t mic_temp_max_fan_in(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_FAN_IN_TEMP];
}

uint32_t mic_temp_max_fan_out(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_FAN_OUT_TEMP];
}

uint32_t mic_temp_max_vccp(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_VCCP_TEMP];
}

uint32_t mic_temp_max_vddg(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_VDDG_TEMP];
}

uint32_t mic_temp_max_vddq(MEASUREMENT *measurement) {
	return measurement->mic_temperature_max[MIC_VDDQ_TEMP];
}

double mic_freq_avg_mem(MEASUREMENT *measurement) {
	return measurement->mic_freq_mem_avg;
}

double mic_freq_avg_core(MEASUREMENT *measurement) {
	return measurement->mic_freq_core_avg;
}

double mic_active_avg_all(MEASUREMENT *measurement) {
	return measurement->mic_util_active_avg;
}

double mic_idle_avg_all(MEASUREMENT *measurement) {
	return measurement->mic_util_idle_avg;
}

double mic_util_avg_all(MEASUREMENT *measurement) {
	return measurement->mic_util_avg;
}

uint32_t mic_memory_total(MEASUREMENT *measurement) {
	return measurement->mic_memory_total;
}

uint32_t mic_memory_used_max(MEASUREMENT *measurement) {
	return measurement->mic_memory_used_max;
}

uint32_t mic_memory_free_max(MEASUREMENT* measurement) {
	return measurement->mic_memory_free_max;
}

// System board
double sysboard_time_total(MEASUREMENT *measurement) {
	return measurement->ipmi_time_runtime;
}

double sysboard_energy_total(MEASUREMENT *measurement) {
	return measurement->ipmi_energy_sysboard_acc;
}

double sysboard_power_avg(MEASUREMENT *measurement) {
	return measurement->ipmi_power_sysboard_avg;
}

double sysboard_temp_max(MEASUREMENT *measurement) {
	return measurement->ipmi_temperature_sysboard_max;
}

// Server
double server_time_total(MEASUREMENT *measurement) {
	return measurement->ipmi_time_runtime;
}

double server_energy_total(MEASUREMENT *measurement) {
	return measurement->ipmi_energy_server_acc;
}

double server_power_avg(MEASUREMENT *measurement) {
	return measurement->ipmi_power_server_avg;
}
