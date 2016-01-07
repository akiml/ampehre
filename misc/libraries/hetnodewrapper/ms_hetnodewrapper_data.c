/**
 * @file ms_hetnodewrapper_data.c
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
 * @author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 9/02/14
 * version: 0.1.16 - add wrapper for heterogenous Node and test tool
 *          0.7.0 - modularised measurement struct
 */

#include "ms_hetnodewrapper_internal.h"

void set_data_zero(MEASUREMENT_DATA *meas) {
	meas->cpu_time_total = 0;
	int g;
	for (g = 0; g < CPUS; g++) {
		meas->cpu_energy_total_pkg[g] = 0;
		meas->cpu_energy_total_pp0[g] = 0;
		meas->cpu_energy_total_dram[g] = 0;
	}
	meas->cpu_util_avg_all = 0;
	meas->gpu_time_total = 0;
	meas->gpu_energy_total = 0;
	meas->gpu_util_avg_gpu = 0;
	meas->gpu_util_avg_mem = 0;
	meas->fpga_time_total = 0;
	meas->fpga_energy_total = 0;
	meas->fpga_energy_total_vcc1v0_core = 0;
	meas->fpga_energy_total_vcc1v5_ddr = 0;
	meas->fpga_energy_total_vcc2v5_aux = 0;
	meas->fpga_energy_total_imgt_1v0 = 0;
	meas->fpga_energy_total_imgt_1v2 = 0;
	meas->fpga_energy_total_mgt_1v0 = 0;
	meas->fpga_energy_total_mgt_1v2 = 0;
	meas->fpga_util_avg_comp = 0;
	meas->sysboard_time_total = 0;
	meas->sysboard_energy_total = 0;
	meas->server_time_total = 0;
	meas->server_energy_total = 0;
}

// get and set all the start values for the measurements
void set_measurement(int no, MINTERN *mintern, MSDATAINTERN *temp_data) {
	// Only values for selected resources are read
	if (((int)mintern->measurements[no].resources & CPU) != 0) {
		//CPU
		MS_MEASUREMENT_CPU *pMsMeasurementCpu = getMeasurement(&(mintern->global_m), CPU);
		mintern->measurements[no].cpu_time_total = cpu_time_total(mintern->global_m);
		int j = 0;
		for (j = 0; j < CPUS; j++) {
			mintern->measurements[no].cpu_energy_total_pkg[j] = cpu_energy_total_pkg(mintern->global_m, j);
			mintern->measurements[no].cpu_energy_total_pp0[j] = cpu_energy_total_pp0(mintern->global_m, j);
			mintern->measurements[no].cpu_energy_total_dram[j] = cpu_energy_total_dram(mintern->global_m, j);
		}
		for (j = 0; j < CPU_STATS; j++){
			temp_data[no].cpu_util_acc[j] = pMsMeasurementCpu->measure_util_acc[j];
		}
	}
	if (((int)mintern->measurements[no].resources & GPU) != 0) {
		//GPU
		MS_MEASUREMENT_GPU *pMsMeasurementGpu = getMeasurement(&(mintern->global_m), GPU);
		mintern->measurements[no].gpu_time_total = gpu_time_total(mintern->global_m);
		mintern->measurements[no].gpu_energy_total = gpu_energy_total(mintern->global_m);
		mintern->measurements[no].gpu_util_avg_gpu = pMsMeasurementGpu->nvml_util_gpu_acc;
		mintern->measurements[no].gpu_util_avg_mem = pMsMeasurementGpu->nvml_util_mem_acc;
	}
	if (((int)mintern->measurements[no].resources & FPGA) != 0) {
		// FPGA
		MS_MEASUREMENT_FPGA *pMsMeasurementFpga = getMeasurement(&(mintern->global_m), FPGA);
		mintern->measurements[no].fpga_time_total = fpga_time_total(mintern->global_m);
		mintern->measurements[no].fpga_energy_total = fpga_energy_total_power_usage(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_vcc1v0_core = fpga_energy_total_vcc1v0_core(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_vcc1v5_ddr = fpga_energy_total_vcc1v5_ddr(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_vcc2v5_aux = fpga_energy_total_vcc2v5_aux(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_imgt_1v0 = fpga_energy_total_imgt_1v0(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_imgt_1v2 = fpga_energy_total_imgt_1v2(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_mgt_1v0 = fpga_energy_total_mgt_1v0(mintern->global_m);
		mintern->measurements[no].fpga_energy_total_mgt_1v2 = fpga_energy_total_mgt_1v2(mintern->global_m);
		mintern->measurements[no].fpga_util_avg_comp = pMsMeasurementFpga->maxeler_util_comp_acc;
	}
	if (((int)mintern->measurements[no].resources & SYSTEM) != 0) {
		// System board
		mintern->measurements[no].sysboard_time_total = system_time_total(mintern->global_m);
		mintern->measurements[no].sysboard_energy_total = system_energy_board(mintern->global_m);

		// Server
		mintern->measurements[no].server_time_total = system_time_total(mintern->global_m);
		mintern->measurements[no].server_energy_total = system_energy_total(mintern->global_m);
	}
}

void get_diff_measurement(int no, MINTERN *mintern, MSDATAINTERN *temp_data) {
	// Only values for selected resources are read
	if (((int)mintern->measurements[no].resources & CPU) != 0) {
		//CPU
		MS_MEASUREMENT_CPU *pMsMeasurementCpu = getMeasurement(&(mintern->global_m), CPU);
		mintern->measurements[no].cpu_time_total = cpu_time_total(mintern->global_m) - mintern->measurements[no].cpu_time_total;
		int j = 0;
		for (j = 0; j < CPUS; j++) {
			mintern->measurements[no].cpu_energy_total_pkg[j] = cpu_energy_total_pkg(mintern->global_m, j) - mintern->measurements[no].cpu_energy_total_pkg[j];
			mintern->measurements[no].cpu_energy_total_pp0[j] = cpu_energy_total_pp0(mintern->global_m, j) - mintern->measurements[no].cpu_energy_total_pp0[j];
			mintern->measurements[no].cpu_energy_total_dram[j] = cpu_energy_total_dram(mintern->global_m, j) - mintern->measurements[no].cpu_energy_total_dram[j];
		}
		for (j = 0; j < CPU_STATS; j++){		
			temp_data[no].cpu_util_acc[j] = pMsMeasurementCpu->measure_util_acc[j]-temp_data[no].cpu_util_acc[j];
		}
		temp_data[no].cpu_util_active_total = temp_data[no].cpu_util_acc[0] + temp_data[no].cpu_util_acc[1] + temp_data[no].cpu_util_acc[2] + temp_data[no].cpu_util_acc[5] + temp_data[no].cpu_util_acc[6] + temp_data[no].cpu_util_acc[7] + temp_data[no].cpu_util_acc[8];
		temp_data[no].cpu_util_idle_total = temp_data[no].cpu_util_acc[3] + temp_data[no].cpu_util_acc[4];
		mintern->measurements[no].cpu_util_avg_all = (double) temp_data[no].cpu_util_active_total / (double) (temp_data[no].cpu_util_active_total + temp_data[no].cpu_util_idle_total) * 100.0;
	} 
	if (((int)mintern->measurements[no].resources & GPU) != 0) {
		//GPU
		MS_MEASUREMENT_GPU *pMsMeasurementGpu = getMeasurement(&(mintern->global_m), GPU);
		mintern->measurements[no].gpu_time_total = gpu_time_total(mintern->global_m) - mintern->measurements[no].gpu_time_total;
		mintern->measurements[no].gpu_energy_total  = gpu_energy_total(mintern->global_m) - mintern->measurements[no].gpu_energy_total;
		mintern->measurements[no].gpu_util_avg_gpu = (pMsMeasurementGpu->nvml_util_gpu_acc - mintern->measurements[no].gpu_util_avg_gpu) / mintern->measurements[no].gpu_time_total;
		mintern->measurements[no].gpu_util_avg_mem = (pMsMeasurementGpu->nvml_util_mem_acc - mintern->measurements[no].gpu_util_avg_mem) / mintern->measurements[no].gpu_time_total;
	} 
	if (((int)mintern->measurements[no].resources & FPGA) != 0) {
		// FPGA
		MS_MEASUREMENT_FPGA *pMsMeasurementFpga = getMeasurement(&(mintern->global_m), FPGA);
		mintern->measurements[no].fpga_time_total = fpga_time_total(mintern->global_m) - mintern->measurements[no].fpga_time_total;
		mintern->measurements[no].fpga_energy_total = fpga_energy_total_power_usage(mintern->global_m) - mintern->measurements[no].fpga_energy_total;
		mintern->measurements[no].fpga_energy_total_vcc1v0_core = fpga_energy_total_vcc1v0_core(mintern->global_m) - mintern->measurements[no].fpga_energy_total_vcc1v0_core;
		mintern->measurements[no].fpga_energy_total_vcc1v5_ddr = fpga_energy_total_vcc1v5_ddr(mintern->global_m) - mintern->measurements[no].fpga_energy_total_vcc1v5_ddr;
		mintern->measurements[no].fpga_energy_total_vcc2v5_aux = fpga_energy_total_vcc2v5_aux(mintern->global_m) - mintern->measurements[no].fpga_energy_total_vcc2v5_aux;
		mintern->measurements[no].fpga_energy_total_imgt_1v0 = fpga_energy_total_imgt_1v0(mintern->global_m) - mintern->measurements[no].fpga_energy_total_imgt_1v0;
		mintern->measurements[no].fpga_energy_total_imgt_1v2 = fpga_energy_total_imgt_1v2(mintern->global_m) - mintern->measurements[no].fpga_energy_total_imgt_1v2;
		mintern->measurements[no].fpga_energy_total_mgt_1v0 = fpga_energy_total_mgt_1v0(mintern->global_m) - mintern->measurements[no].fpga_energy_total_mgt_1v0;
		mintern->measurements[no].fpga_energy_total_mgt_1v2 = fpga_energy_total_mgt_1v2(mintern->global_m) - mintern->measurements[no].fpga_energy_total_mgt_1v2;
		mintern->measurements[no].fpga_util_avg_comp = (pMsMeasurementFpga->maxeler_util_comp_acc - mintern->measurements[no].fpga_util_avg_comp) / mintern->measurements[no].fpga_time_total;
	}
	if (((int)mintern->measurements[no].resources & SYSTEM) != 0) {
		// System board
		mintern->measurements[no].sysboard_time_total = system_time_total(mintern->global_m) - mintern->measurements[no].sysboard_time_total;
		mintern->measurements[no].sysboard_energy_total = system_energy_board(mintern->global_m) - mintern->measurements[no].sysboard_energy_total;

		// Server
		mintern->measurements[no].server_time_total = system_time_total(mintern->global_m) - mintern->measurements[no].server_time_total;
		mintern->measurements[no].server_energy_total = system_energy_total(mintern->global_m);
	}
}
