/**
 * @file ms_hetnodewrapper.h
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
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.7.0 - modularized measurement struct
 */

#ifndef __MS_HETNODEWRAPPER_H__
#define __MS_HETNODEWRAPPER_H__

#include "ms_measurement.h"

/**
 * Struct for the measurement results.
 */
typedef struct __measurement_data {
	
	//resources which are measured
	//possible values are: CPU	0x1, GPU 0x2, FPGA	0x4, SYSTEM	0x8, ALL 0xF
	double resources;
	
	//CPU
	double cpu_time_total;
	double cpu_energy_total_pkg[CPUS];
	double cpu_energy_total_pp0[CPUS];
	double cpu_energy_total_dram[CPUS];
	double cpu_util_avg_all;
	
	//GPU
	double gpu_time_total;
	double gpu_energy_total;
	double gpu_util_avg_gpu;
	double gpu_util_avg_mem;
	
	// FPGA
	double fpga_time_total;
	double fpga_energy_total;
	double fpga_energy_total_vcc1v0_core;
	double fpga_energy_total_vcc1v5_ddr;
	double fpga_energy_total_vcc2v5_aux;
	double fpga_energy_total_imgt_1v0;
	double fpga_energy_total_imgt_1v2;
	double fpga_energy_total_mgt_1v0;
	double fpga_energy_total_mgt_1v2;
	double fpga_util_avg_comp;
	
	// System board
	double sysboard_time_total;
	double sysboard_energy_total;

	// Server
	double server_time_total;
	double server_energy_total;
	
} MEASUREMENT_DATA;

/**
 * Struct for the measurements from the heterogenous node
 * For each measurement there has to be a definition of the desired resources in the resources array
 */
typedef struct __hn_measurement{
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_sys;
	enum cpu_governor cpu_gov;
	uint64_t cpu_freq_min;
	uint64_t cpu_freq_max;
	enum gpu_frequency gpu_freq;
	int32_t* resources;
} HN_MEASUREMENT;

/**
 * Initialize the measurement system.
 * @param measurement_no needs to be the number of measurements which are specified in measurements
 * @param measurements struct with measurement settings from heterogenous node
 */
void mshnw_init(MS_VERSION *version, HN_MEASUREMENT *measurements, int measurement_no);

/**
 * Delete all data and exit the Measurement System.
 */
void mshnw_free();

/**
 * Start the Measurement System.
 */
void mshnw_start();

/** 
 * Stop the Measurement System.
 */
void mshnw_shutdown();

/**
 * Start the Measurement and set the start values.
 * @param no ID of the measurement which will be started
 */
void mshnw_start_measurement(int no);

/**
 * Stop the Measurement with the ID no.
 * @param no ID of the measurement which will be stopped
 */
void mshnw_stop_measurement(int no);

/**
 * Get the Measurement results.
 * @return Pointer to all measurement results
 */
MEASUREMENT_DATA* mshnw_get_ms_data();

/**
 * Force the FPGA to idle.
 */
void mshnw_fpga_force_idle();


#endif /* __MS_HETNODEWRAPPER_H__*/
