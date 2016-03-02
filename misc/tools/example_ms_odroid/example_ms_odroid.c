/*
 * example_ms_odroid.c
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 3/02/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#include <stdio.h>
#include <unistd.h>

#include "../../../include/ms_measurement.h"

int main(int argc, char **argv) {
	// Initialize library and measurement system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	MS_SYSTEM *ms = ms_init(&version, CPU_GOVERNOR_ONDEMAND, 2000000, 2500000, GPU_FREQUENCY_CUR, IPMI_SET_TIMEOUT, SKIP_PERIODIC, VARIANT_FULL);
	
	// Allocate measurement list
	MS_LIST *m = ms_alloc_measurement(ms);
	
	// Set timer for m. Measurements performed every 50ms.
	ms_set_timer(m, ODROID, 0, 50000000, 1);
	ms_init_measurement(ms, m, ODROID);
	
	// Start measurements
	ms_start_measurement(ms);
	
	// Replace with user's code
	sleep(10);
	
	// Stop all measurement procedures.
	ms_stop_measurement(ms);
	
	// Join measurement threads and remove thread objects.
	ms_join_measurement(ms);
	ms_fini_measurement(ms);
	
	// Print some measured data to stdout.
	printf("energy total a15  [mWs]: %lf\n", odroid_energy_total_a15(m));
	printf("energy total a7   [mWs]: %lf\n", odroid_energy_total_a7(m));
	printf("energy total mali [mWs]: %lf\n", odroid_energy_total_mali(m));
	printf("energy total mem  [mWs]: %lf\n", odroid_energy_total_mem(m));
	printf("power  avg   a15  [mW ]: %lf\n", odroid_power_avg_a15(m));
	printf("power  avg   a7   [mW ]: %lf\n", odroid_power_avg_a7(m));
	printf("power  avg   mali [mW ]: %lf\n", odroid_power_avg_mali(m));
	printf("power  avg   mem  [mW ]: %lf\n", odroid_power_avg_mem(m));
	printf("temp   max   a15.0[\u00b0C ]: %u\n", odroid_temp_max_a15(m, 0));
	printf("temp   max   a15.1[\u00b0C ]: %u\n", odroid_temp_max_a15(m, 1));
	printf("temp   max   a15.2[\u00b0C ]: %u\n", odroid_temp_max_a15(m, 2));
	printf("temp   max   a15.3[\u00b0C ]: %u\n", odroid_temp_max_a15(m, 3));
	printf("temp   max   mali [\u00b0C ]: %u\n", odroid_temp_max_mali(m));
	printf("freq   avg   a15  [MHz]: %lf\n", odroid_freq_avg_a15(m));
	printf("freq   avg   a7   [MHz]: %lf\n", odroid_freq_avg_a7(m));
	printf("freq   avg   mali [MHz]: %lf\n", odroid_freq_avg_mali(m));
	
	// Cleanup the environment before exiting the program
	ms_free_measurement(m);
	ms_fini(ms);
	
	return EXIT_SUCCESS;
}
