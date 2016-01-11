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
 *          0.7.0 - modularized measurement struct
 */

#include "../../include/ms_measurement.h"

#define NULL_CHECK(val)  if(NULL == val) return 0;

// CPU
double cpu_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_time_runtime;
}

double cpu_energy_total_pkg(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_energy_acc[cpu][PKG];
}

double cpu_energy_total_pp0(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_energy_acc[cpu][PP0];
}

double cpu_energy_total_dram(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_energy_acc[cpu][DRAM];
}

double cpu_power_avg_pkg(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_power_avg[cpu][PKG];
}

double cpu_power_avg_pp0(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_power_avg[cpu][PP0];
}

double cpu_power_avg_dram(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_power_avg[cpu][DRAM];
}

uint32_t cpu_temp_max_pkg(MS_LIST *ms_list, int cpu) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_temperature_pkg_max[cpu];
}

uint32_t cpu_temp_max_core(MS_LIST *ms_list, int cpu, int core) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_temperature_core_max[cpu][core];
}

double cpu_freq_avg_core(MS_LIST *ms_list, int cpu, int core) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_freq_core_avg[cpu][core];
}

double cpu_freq_eff_core(MS_LIST *ms_list, int cpu, int core) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->msr_freq_core_eff_avg[cpu][core];
}

double cpu_active_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_util_active_avg;
}

double cpu_idle_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_util_idle_avg;
}

double cpu_util_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_util_avg;
}

uint32_t cpu_memory_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return (uint32_t) ms_measurement_cpu->measure_memory_cur[CPU_MEM_RAM_TOTAL];
}

uint32_t cpu_memory_used_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_memory_used_max;
}

uint32_t cpu_memory_free_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_memory_free_max;
}

uint32_t cpu_swap_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return (uint32_t) ms_measurement_cpu->measure_memory_cur[CPU_MEM_SWAP_TOTAL];
}

uint32_t cpu_swap_used_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_swap_used_max;
}

uint32_t cpu_swap_free_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_CPU *ms_measurement_cpu = (MS_MEASUREMENT_CPU *) getMeasurement(&ms_list, CPU);
	NULL_CHECK(ms_measurement_cpu);
	
	return ms_measurement_cpu->measure_swap_free_max;
}

// GPU
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

// FPGA
double fpga_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_time_runtime;
}

double fpga_energy_total_vcc1v0_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC10];
}

double fpga_energy_total_vcc1v5_ddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC15];
}

double fpga_energy_total_vcc2v5_aux(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC25];
}

double fpga_energy_total_imgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[IMGT10];
}

double fpga_energy_total_imgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[IMGT12];
}

double fpga_energy_total_mgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[MGT10];
}

double fpga_energy_total_mgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[MGT12];
}

double fpga_energy_total_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[POWER];
}

double fpga_power_avg_vcc1v0_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC10];
}

double fpga_power_avg_vcc1v5_ddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC15];
}

double fpga_power_avg_vcc2v5_aux(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC25];
}

double fpga_power_avg_imgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[IMGT10];
}

double fpga_power_avg_imgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[IMGT12];
}

double fpga_power_avg_mgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[MGT10];
}

double fpga_power_avg_mgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[MGT12];
}

double fpga_power_avg_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[POWER];
}

double fpga_temp_max_ifpga(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_temperature_max[ITEMP];
}

double fpga_temp_max_mfpga(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_temperature_max[MTEMP];
}

double fpga_util_avg_comp(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_util_comp_avg;
}

//MIC
double mic_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_time_runtime;
}

double mic_energy_total_pcie(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_PCIE];
}

double mic_energy_total_c2x3(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_C2X3];
}

double mic_energy_total_c2x4(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_C2X4];
}

double mic_energy_total_vccp(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_VCCP];
}

double mic_energy_total_vddg(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_VDDG];
}

double mic_energy_total_vddq(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_VDDQ];
}

double mic_energy_total_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_energy_acc[MIC_POWER];
}

double mic_power_avg_pcie(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_PCIE];
}

double mic_power_avg_c2x3(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_C2X3];
}

double mic_power_avg_c2x4(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_C2X4];
}

double mic_power_avg_vccp(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_VCCP];
}

double mic_power_avg_vddg(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_VDDG];
}

double mic_power_avg_vddq(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_VDDQ];
}

double mic_power_avg_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_power_avg[MIC_POWER];
}

uint32_t mic_temp_max_die(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_DIE_TEMP];
}

uint32_t mic_temp_max_gddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_GDDR_TEMP];
}

uint32_t mic_temp_max_fan_in(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_FAN_IN_TEMP];
}

uint32_t mic_temp_max_fan_out(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_FAN_OUT_TEMP];
}

uint32_t mic_temp_max_vccp(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_VCCP_TEMP];
}

uint32_t mic_temp_max_vddg(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_VDDG_TEMP];
}

uint32_t mic_temp_max_vddq(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_temperature_max[MIC_VDDQ_TEMP];
}

double mic_freq_avg_mem(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_freq_mem_avg;
}

double mic_freq_avg_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_freq_core_avg;
}

double mic_active_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_util_active_avg;
}

double mic_idle_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_util_idle_avg;
}

double mic_util_avg_all(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_util_avg;
}

uint32_t mic_memory_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_memory_total;
}

uint32_t mic_memory_used_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_memory_used_max;
}

uint32_t mic_memory_free_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_MIC *ms_measurement_mic = (MS_MEASUREMENT_MIC *) getMeasurement(&ms_list, MIC);
	NULL_CHECK(ms_measurement_mic);
	
	return ms_measurement_mic->mic_memory_free_max;
}

// System
double system_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_time_runtime;
}

double system_energy_board(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_energy_sysboard_acc;
}

double system_power_board_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_power_sysboard_avg;
}

double system_temp_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_temperature_sysboard_max;
}

double system_energy_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_energy_server_acc;
}

double system_power_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_power_server_avg;
}
