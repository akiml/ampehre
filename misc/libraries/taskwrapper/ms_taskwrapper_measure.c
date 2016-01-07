/*
 * ms_taskwrapper_measure.c
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
 * created: 8/05/14
 * version: 0.1.0 - initial implementation
 */

#include "ms_taskwrapper_internal.h"

void mstw_init_rel_measurement(MREL_MEASUREMENT *m) {
	// Clean acc vals of m
	
	// CPU
	m->acc_cpu0_energy_total_pkg	= 0.0;
	m->acc_cpu0_energy_total_dram	= 0.0;
	m->acc_cpu1_energy_total_pkg	= 0.0;
	m->acc_cpu1_energy_total_dram	= 0.0;
	
	// GPU
	m->acc_gpu_energy_total			= 0.0;
	
	// FPGA
	m->acc_fpga_energy_total		= 0.0;
	
	// MIC
	m->acc_mic_energy_total			= 0.0;
	
	// System
	m->acc_sysboard_energy_total	= 0.0;
	m->acc_server_energy_total		= 0.0;
}

void mstw_reset_rel_measurement(MREL_MEASUREMENT *m) {
	mstw_init_rel_measurement(m);
}

MREL_MEASUREMENT *mstw_get_rel_measurement(MTASK *mtask, int resource, int phase) {
	MREL_MEASUREMENT *m = NULL;
	
	int i;
	for (i=0; i<NUM_OF_RESOURCES; ++i) {
		if ((resource >> i) & 0x1) {
			m = &(mtask->rm[i][phase]);
			break;
		}
	}
	
	return m;
}

void mstw_capture_start_measure(MINTERNAL *minternal, MREL_MEASUREMENT *m) {
	// Store relevant current acc values of minternal->global_m to cur vals in m
	
	// CPU
	if (minternal->resources & CPU) {
		m->cur_cpu0_energy_total_pkg	= cpu_energy_total_pkg(minternal->global_m, 0);
		m->cur_cpu0_energy_total_dram	= cpu_energy_total_dram(minternal->global_m, 0);
		m->cur_cpu1_energy_total_pkg	= cpu_energy_total_pkg(minternal->global_m, 1);
		m->cur_cpu1_energy_total_dram	= cpu_energy_total_dram(minternal->global_m, 1);
	}
	
	// GPU
	if (minternal->resources & GPU) {
		m->cur_gpu_energy_total			= gpu_energy_total(minternal->global_m);
	}
	
	// FPGA
	if (minternal->resources & FPGA) {
		m->cur_fpga_energy_total		= fpga_energy_total_power_usage(minternal->global_m);
	}
	
	// MIC
	if (minternal->resources & MIC) {
		m->cur_mic_energy_total			= mic_energy_total_power_usage(minternal->global_m);
	}
	
	// System
	if (minternal->resources & SYSTEM) {
		m->cur_sysboard_energy_total	= system_energy_board(minternal->global_m);
		m->cur_server_energy_total		= system_energy_total(minternal->global_m);
	}
}

void mstw_capture_stop_measure(MINTERNAL *minternal, MREL_MEASUREMENT *m) {
	// Calc difference between cur vals in m to current acc values of minternal->global_m and store to acc vals of m
	
	// CPU
	if (minternal->resources & CPU) {
		m->acc_cpu0_energy_total_pkg	+= cpu_energy_total_pkg(minternal->global_m, 0)			- m->cur_cpu0_energy_total_pkg;
		m->acc_cpu0_energy_total_dram	+= cpu_energy_total_dram(minternal->global_m, 0)		- m->cur_cpu0_energy_total_dram;
		m->acc_cpu1_energy_total_pkg	+= cpu_energy_total_pkg(minternal->global_m, 1)			- m->cur_cpu1_energy_total_pkg;
		m->acc_cpu1_energy_total_dram	+= cpu_energy_total_dram(minternal->global_m, 1)		- m->cur_cpu1_energy_total_dram;
	}
	
	// GPU
	if (minternal->resources & GPU) {
		m->acc_gpu_energy_total			+= gpu_energy_total(minternal->global_m)				- m->cur_gpu_energy_total;
	}
	
	// FPGA
	if (minternal->resources & FPGA) {
		m->acc_fpga_energy_total		+= fpga_energy_total_power_usage(minternal->global_m)	- m->cur_fpga_energy_total;
	}
	
	// MIC
	if (minternal->resources & MIC) {
		m->acc_mic_energy_total			+= mic_energy_total_power_usage(minternal->global_m)	- m->cur_mic_energy_total;
	}
	
	// System
	if (minternal->resources & SYSTEM) {
		m->acc_sysboard_energy_total	+= system_energy_board(minternal->global_m)				- m->cur_sysboard_energy_total;
		m->acc_server_energy_total		+= system_energy_total(minternal->global_m)				- m->cur_server_energy_total;
	}
}
