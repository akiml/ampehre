/*
 * example_ms_hetsched.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../../include/ms_hetschedwrapper.h"

typedef struct __task_context {
	MTASK *mtask;
	uint32_t duration_init;
	uint32_t duration_compute;
	uint32_t duration_free;
} TASK_CONTEXT;

int main(int argc, char **argv);
void pseudo_task(TASK_CONTEXT *ctxt);

int main(int argc, char **argv) {
	// Init measuring system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	mshetsched_init(&version, CPU_GOVERNOR_ONDEMAND, 2000000, 2500000, GPU_FREQUENCY_CUR, IOC_SET_IPMI_TIMEOUT, SKIP_NEVER, VARIANT_FULL, NULL, NULL);
	
	// Task data container
	TASK_CONTEXT ctxt;
	ctxt.mtask				= NULL;
	ctxt.duration_init		= 3;
	ctxt.duration_compute	= 20;
	ctxt.duration_free		= 4;
	
	// Run task 5 seconds after hetsched start
	sleep(5);
	pseudo_task(&ctxt);
	sleep(5);
	
	// Destroy measuring system
	mshetsched_destroy();
	
	return EXIT_SUCCESS;
}

void pseudo_task(TASK_CONTEXT *ctxt) {
	// Register pseudo task
	mshetsched_register(&(ctxt->mtask));
	
	// Execute pseudo init phase for calculation on device FPGA
	mshetsched_init_fpga_init(ctxt->mtask);
	printf("Task init...\n");
	sleep(ctxt->duration_init);
	mshetsched_fini_fpga_init(ctxt->mtask);
	
	// Execute pseudo compute phase for calculation on device FPGA
	mshetsched_init_fpga_compute(ctxt->mtask);
	printf("Task compute part 1 of 2...\n");
	sleep(ctxt->duration_compute/2);
	mshetsched_fini_fpga_compute(ctxt->mtask);
	sleep(1);
	mshetsched_init_fpga_compute(ctxt->mtask);
	printf("Task compute part 2 of 2...\n");
	sleep(ctxt->duration_compute/2);
	mshetsched_fini_fpga_compute(ctxt->mtask);
	
	// Execute pseudo free phase for calculation on device FPGA
	mshetsched_init_fpga_free(ctxt->mtask);
	printf("Task free...\n");
	sleep(ctxt->duration_free);
	mshetsched_fini_fpga_free(ctxt->mtask);
	
	// Get measurements for our task dispatched to FPGA
	MREL_MEASUREMENT *fpga_init		= mshetsched_get_fpga_init(ctxt->mtask);
	MREL_MEASUREMENT *fpga_compute	= mshetsched_get_fpga_compute(ctxt->mtask);
	MREL_MEASUREMENT *fpga_free		= mshetsched_get_fpga_free(ctxt->mtask);
	
	printf("RESULTS:\n========\n");
	printf("duration init    [s]: %u\n", ctxt->duration_init);
	printf("duration compute [s]: %u\n", ctxt->duration_compute);
	printf("duration free    [s]: %u\n", ctxt->duration_free);
	
	// Print the CPU energy consumption of our pseudo FPGA task.
	printf("\nCPU:\n====\n");
	printf("CPU0 init    [mWs]: %lf\n", fpga_init->acc_cpu0_energy_total_pkg	+ fpga_init->acc_cpu0_energy_total_dram);
 	printf("CPU0 compute [mWs]: %lf\n", fpga_compute->acc_cpu0_energy_total_pkg	+ fpga_compute->acc_cpu0_energy_total_dram);
	printf("CPU0 free    [mWs]: %lf\n", fpga_free->acc_cpu0_energy_total_pkg	+ fpga_free->acc_cpu0_energy_total_dram);
	printf("CPU1 init    [mWs]: %lf\n", fpga_init->acc_cpu1_energy_total_pkg	+ fpga_init->acc_cpu1_energy_total_dram);
	printf("CPU1 compute [mWs]: %lf\n", fpga_compute->acc_cpu1_energy_total_pkg	+ fpga_compute->acc_cpu1_energy_total_dram);
	printf("CPU1 free    [mWs]: %lf\n", fpga_free->acc_cpu1_energy_total_pkg	+ fpga_free->acc_cpu1_energy_total_dram);
	
	// Print the FPGA energy consumption of our pseudo FPGA task.
	printf("\nFPGA:\n====\n");
	printf("FPGA init    [mWs]: %lf\n", fpga_init->acc_fpga_energy_total);
	printf("FPGA compute [mWs]: %lf\n", fpga_compute->acc_fpga_energy_total);
	printf("FPGA free    [mWs]: %lf\n", fpga_free->acc_fpga_energy_total);
	
	// Unregister pseudo task
	mshetsched_unregister(&(ctxt->mtask));
}
