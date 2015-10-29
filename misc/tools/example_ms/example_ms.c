/*
 * example_ms.c
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
 * created: 4/01/14
 * version: 0.1.0 - initial implementation
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.4.0 - MIC integration into libmeasure
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../../include/measurement.h"

int main(int argc, char **argv) {
	// Initialize library and measurement system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	MSYSTEM *ms = ms_init(&version, CPU_GOVERNOR_ONDEMAND, 2000000, 2500000, GPU_FREQUENCY_CUR, IOC_SET_IPMI_TIMEOUT);
	
	// Allocate measurement structs
	MEASUREMENT *m1 = ms_alloc_measurement();
	
	// Set timer for m1. Measurements perform every 10ms/30ms.
	ms_set_timer(m1, CPU , 0, 10000000);
	ms_set_timer(m1, GPU , 0, 30000000);
	ms_set_timer(m1, FPGA , 0, 30000000);
	ms_set_timer(m1, SYSTEM , 0, 100000000);
	ms_set_timer(m1, MIC , 0, 30000000);
	ms_init_measurement(ms, m1, ALL);
	
	// Start measurements
	ms_start_measurement(ms, m1);
	sleep(10);
	
	// Stop all measurement procedures.
	ms_stop_measurement(ms, m1);
	
	// Join measurement threads and remove thread objects.
	ms_join_measurement(ms, m1);
	ms_fini_measurement(ms, m1);
	
	// Print some measured data to stdout.
	printf("consumed energy of cpu 1 dram bank : %.2lf mWs\n", cpu_energy_total_dram(m1, 1));
	printf("maximum temperature of gpu         : %u \u00b0C\n", gpu_temp_max(m1));
	printf("total time of mic measuring        : %.2lf s\n", mic_time_total(m1));
	printf("mic temperature max die            : %u \u00b0C\n", mic_temp_max_die(m1));
	
	// Cleanup the environment before exiting the program
	ms_free_measurement(m1);
	ms_fini(ms);
	
	return EXIT_SUCCESS;
}
