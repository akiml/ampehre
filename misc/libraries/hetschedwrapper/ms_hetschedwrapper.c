/*
 * ms_hetschedwrapper.c
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
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 */

#include "../../../include/ms_hetschedwrapper.h"
#include <stdio.h>


void mshetsched_init(MS_VERSION *version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
					 enum gpu_frequency gpu_freq, uint64_t ipmi_timeout_setting, enum skip_ms_rate skip_ms, enum lib_variant variant,
					 void(*signal_handler_start)(int), void(*signal_handler_stop)(int)) {
	if((version->major != MS_MAJOR_VERSION) || (version->minor != MS_MINOR_VERSION) || (version->revision != MS_REVISION_VERSION)){
		printf("Error in hetschedwrapper: Wrong version number! hetschedwrapper version %d.%d.%d is called from tool with version %d.%d.%d.\n", MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, version->major, version->minor, version->revision);
		exit(EXIT_FAILURE);
	}
	mstw_init(version, CPU | GPU | FPGA, cpu_gov, cpu_freq_min, cpu_freq_max, gpu_freq, ipmi_timeout_setting, skip_ms, variant);
	
	if (signal_handler_start) {
		mstw_reg_sighandler_start(signal_handler_start);
	}
	if (signal_handler_stop) {
		mstw_reg_sighandler_stop(signal_handler_stop);
	}
	
	mstw_start();
}

void mshetsched_destroy(void) {
	mstw_stop();
	mstw_free();
}

void mshetsched_register(MTASK **mtask) {
	mstw_reg_task(mtask, 3);
}

void mshetsched_unregister(MTASK **mtask) {
	mstw_unreg_task(mtask);
}

/*
 * CPU
 */
void mshetsched_init_cpu_init(MTASK *mtask) {
	mstw_start_measurement(mtask, CPU, 0);
}

void mshetsched_init_cpu_compute(MTASK *mtask) {
	mstw_start_measurement(mtask, CPU, 1);
}

void mshetsched_init_cpu_free(MTASK *mtask) {
	mstw_start_measurement(mtask, CPU, 2);
}

void mshetsched_fini_cpu_init(MTASK *mtask) {
	mstw_stop_measurement(mtask, CPU, 0);
}

void mshetsched_fini_cpu_compute(MTASK *mtask) {
	mstw_stop_measurement(mtask, CPU, 1);
}

void mshetsched_fini_cpu_free(MTASK *mtask) {
	mstw_stop_measurement(mtask, CPU, 2);
}

/*
 * GPU
 */
void mshetsched_init_gpu_init(MTASK *mtask) {
	mstw_start_measurement(mtask, GPU, 0);
}

void mshetsched_init_gpu_compute(MTASK *mtask) {
	mstw_start_measurement(mtask, GPU, 1);
}

void mshetsched_init_gpu_free(MTASK *mtask) {
	mstw_start_measurement(mtask, GPU, 2);
}

void mshetsched_fini_gpu_init(MTASK *mtask) {
	mstw_stop_measurement(mtask, GPU, 0);
}

void mshetsched_fini_gpu_compute(MTASK *mtask) {
	mstw_stop_measurement(mtask, GPU, 1);
}

void mshetsched_fini_gpu_free(MTASK *mtask) {
	mstw_stop_measurement(mtask, GPU, 2);
}

/*
 * FPGA
 */
void mshetsched_init_fpga_init(MTASK *mtask) {
	mstw_start_measurement(mtask, FPGA, 0);
}

void mshetsched_init_fpga_compute(MTASK *mtask) {
	mstw_start_measurement(mtask, FPGA, 1);
}

void mshetsched_init_fpga_free(MTASK *mtask) {
	mstw_start_measurement(mtask, FPGA, 2);
}

void mshetsched_fini_fpga_init(MTASK *mtask) {
	mstw_stop_measurement(mtask, FPGA, 0);
}

void mshetsched_fini_fpga_compute(MTASK *mtask) {
	mstw_stop_measurement(mtask, FPGA, 1);
}

void mshetsched_fini_fpga_free(MTASK *mtask) {
	mstw_stop_measurement(mtask, FPGA, 2);
}

/*
 * OBTAIN RESULTS
 */
MREL_MEASUREMENT *mshetsched_get_cpu_init(MTASK *mtask) {
	return &(mtask->rm[0][0]);
}

MREL_MEASUREMENT *mshetsched_get_cpu_compute(MTASK *mtask) {
	return &(mtask->rm[0][1]);
}

MREL_MEASUREMENT *mshetsched_get_cpu_free(MTASK *mtask) {
	return &(mtask->rm[0][2]);
}

MREL_MEASUREMENT *mshetsched_get_gpu_init(MTASK *mtask) {
	return &(mtask->rm[1][0]);
}

MREL_MEASUREMENT *mshetsched_get_gpu_compute(MTASK *mtask) {
	return &(mtask->rm[1][1]);
}

MREL_MEASUREMENT *mshetsched_get_gpu_free(MTASK *mtask) {
	return &(mtask->rm[1][2]);
}

MREL_MEASUREMENT *mshetsched_get_fpga_init(MTASK *mtask) {
	return &(mtask->rm[2][0]);
}

MREL_MEASUREMENT *mshetsched_get_fpga_compute(MTASK *mtask) {
	return &(mtask->rm[2][1]);
}

MREL_MEASUREMENT *mshetsched_get_fpga_free(MTASK *mtask) {
	return &(mtask->rm[2][2]);
}

/*
 * OBTAIN TRANSPORT STRUCT FOR SCHEDULER TO CLIENT COMMUNICATION
 */
void mshetsched_get_all(MTASK *mtask, MTRANS *mtrans, uint32_t *size) {
	MREL_MEASUREMENT *temp = NULL;
	*size = sizeof(MTRANS);
	
	// CPU
	temp = mshetsched_get_cpu_init(mtask);
	mtrans->cpu.init.measurement[m_cpu0_energy_total_pkg]		= temp->acc_cpu0_energy_total_pkg;
	mtrans->cpu.init.measurement[m_cpu0_energy_total_dram]		= temp->acc_cpu0_energy_total_dram;
	mtrans->cpu.init.measurement[m_cpu1_energy_total_pkg]		= temp->acc_cpu1_energy_total_pkg;
	mtrans->cpu.init.measurement[m_cpu1_energy_total_dram]		= temp->acc_cpu1_energy_total_dram;
	mtrans->cpu.init.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->cpu.init.measurement[m_fpga_energy_total]			= temp->acc_fpga_energy_total;
	
	temp = mshetsched_get_cpu_compute(mtask);
	mtrans->cpu.compute.measurement[m_cpu0_energy_total_pkg]	= temp->acc_cpu0_energy_total_pkg;
	mtrans->cpu.compute.measurement[m_cpu0_energy_total_dram]	= temp->acc_cpu0_energy_total_dram;
	mtrans->cpu.compute.measurement[m_cpu1_energy_total_pkg]	= temp->acc_cpu1_energy_total_pkg;
	mtrans->cpu.compute.measurement[m_cpu1_energy_total_dram]	= temp->acc_cpu1_energy_total_dram;
	mtrans->cpu.compute.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->cpu.compute.measurement[m_fpga_energy_total]		= temp->acc_fpga_energy_total;
	
	temp = mshetsched_get_cpu_free(mtask);
	mtrans->cpu.free.measurement[m_cpu0_energy_total_pkg]		= temp->acc_cpu0_energy_total_pkg;
	mtrans->cpu.free.measurement[m_cpu0_energy_total_dram]		= temp->acc_cpu0_energy_total_dram;
	mtrans->cpu.free.measurement[m_cpu1_energy_total_pkg]		= temp->acc_cpu1_energy_total_pkg;
	mtrans->cpu.free.measurement[m_cpu1_energy_total_dram]		= temp->acc_cpu1_energy_total_dram;
	mtrans->cpu.free.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->cpu.free.measurement[m_fpga_energy_total]			= temp->acc_fpga_energy_total;
	
	// GPU
	temp = mshetsched_get_gpu_init(mtask);
	mtrans->gpu.init.measurement[m_cpu0_energy_total_pkg]		= temp->acc_cpu0_energy_total_pkg;
	mtrans->gpu.init.measurement[m_cpu0_energy_total_dram]		= temp->acc_cpu0_energy_total_dram;
	mtrans->gpu.init.measurement[m_cpu1_energy_total_pkg]		= temp->acc_cpu1_energy_total_pkg;
	mtrans->gpu.init.measurement[m_cpu1_energy_total_dram]		= temp->acc_cpu1_energy_total_dram;
	mtrans->gpu.init.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->gpu.init.measurement[m_fpga_energy_total]			= temp->acc_fpga_energy_total;
	
	temp = mshetsched_get_gpu_compute(mtask);
	mtrans->gpu.compute.measurement[m_cpu0_energy_total_pkg]	= temp->acc_cpu0_energy_total_pkg;
	mtrans->gpu.compute.measurement[m_cpu0_energy_total_dram]	= temp->acc_cpu0_energy_total_dram;
	mtrans->gpu.compute.measurement[m_cpu1_energy_total_pkg]	= temp->acc_cpu1_energy_total_pkg;
	mtrans->gpu.compute.measurement[m_cpu1_energy_total_dram]	= temp->acc_cpu1_energy_total_dram;
	mtrans->gpu.compute.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->gpu.compute.measurement[m_fpga_energy_total]		= temp->acc_fpga_energy_total;
	
	temp = mshetsched_get_gpu_free(mtask);
	mtrans->gpu.free.measurement[m_cpu0_energy_total_pkg]		= temp->acc_cpu0_energy_total_pkg;
	mtrans->gpu.free.measurement[m_cpu0_energy_total_dram]		= temp->acc_cpu0_energy_total_dram;
	mtrans->gpu.free.measurement[m_cpu1_energy_total_pkg]		= temp->acc_cpu1_energy_total_pkg;
	mtrans->gpu.free.measurement[m_cpu1_energy_total_dram]		= temp->acc_cpu1_energy_total_dram;
	mtrans->gpu.free.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->gpu.free.measurement[m_fpga_energy_total]			= temp->acc_fpga_energy_total;
	
	// FPGA
	temp = mshetsched_get_fpga_init(mtask);
	mtrans->fpga.init.measurement[m_cpu0_energy_total_pkg]		= temp->acc_cpu0_energy_total_pkg;
	mtrans->fpga.init.measurement[m_cpu0_energy_total_dram]		= temp->acc_cpu0_energy_total_dram;
	mtrans->fpga.init.measurement[m_cpu1_energy_total_pkg]		= temp->acc_cpu1_energy_total_pkg;
	mtrans->fpga.init.measurement[m_cpu1_energy_total_dram]		= temp->acc_cpu1_energy_total_dram;
	mtrans->fpga.init.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->fpga.init.measurement[m_fpga_energy_total]			= temp->acc_fpga_energy_total;
	
	temp = mshetsched_get_fpga_compute(mtask);
	mtrans->fpga.compute.measurement[m_cpu0_energy_total_pkg]	= temp->acc_cpu0_energy_total_pkg;
	mtrans->fpga.compute.measurement[m_cpu0_energy_total_dram]	= temp->acc_cpu0_energy_total_dram;
	mtrans->fpga.compute.measurement[m_cpu1_energy_total_pkg]	= temp->acc_cpu1_energy_total_pkg;
	mtrans->fpga.compute.measurement[m_cpu1_energy_total_dram]	= temp->acc_cpu1_energy_total_dram;
	mtrans->fpga.compute.measurement[m_gpu_energy_total]		= temp->acc_gpu_energy_total;
	mtrans->fpga.compute.measurement[m_fpga_energy_total]		= temp->acc_fpga_energy_total;
	
	temp = mshetsched_get_fpga_free(mtask);
	mtrans->fpga.free.measurement[m_cpu0_energy_total_pkg]		= temp->acc_cpu0_energy_total_pkg;
	mtrans->fpga.free.measurement[m_cpu0_energy_total_dram]		= temp->acc_cpu0_energy_total_dram;
	mtrans->fpga.free.measurement[m_cpu1_energy_total_pkg]		= temp->acc_cpu1_energy_total_pkg;
	mtrans->fpga.free.measurement[m_cpu1_energy_total_dram]		= temp->acc_cpu1_energy_total_dram;
	mtrans->fpga.free.measurement[m_gpu_energy_total]			= temp->acc_gpu_energy_total;
	mtrans->fpga.free.measurement[m_fpga_energy_total]			= temp->acc_fpga_energy_total;
}
