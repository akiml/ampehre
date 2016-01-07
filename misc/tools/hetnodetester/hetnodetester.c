/*
 * hetnodetester.c
 *
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 9/03/14
 * version: 0.1.16 - add wrapper for heterogenous Node and test tool
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 */

#include <stdio.h>
#include <unistd.h>

#include "../../../include/ms_hetnodewrapper.h"


// print all measured data to stdout
void print_measurement(MEASUREMENT_DATA measurement) {

	if (((int)measurement.resources & CPU) != 0) {
		printf("====CPU==== \n");
		printf("total time of cpu measuring \t \t: %lf s\n", measurement.cpu_time_total);
		printf("consumed energy of cpu 1 pkg \t \t: %lf mWs\n", measurement.cpu_energy_total_pkg[0]);
		printf("consumed energy of cpu 2 pkg \t \t: %lf mWs\n", measurement.cpu_energy_total_pkg[1]);
		printf("consumed energy of cpu 1 pp0 \t \t: %lf mWs\n", measurement.cpu_energy_total_pp0[0]);
		printf("consumed energy of cpu 2 pp0 \t \t: %lf mWs\n", measurement.cpu_energy_total_pp0[1]);
		printf("consumed energy of cpu 1 dram bank \t: %lf mWs\n", measurement.cpu_energy_total_dram[0]);
		printf("consumed energy of cpu 2 dram bank \t: %lf mWs\n", measurement.cpu_energy_total_dram[1]);
		printf("cpu utilization average	\t \t: %lf  %% \n", measurement.cpu_util_avg_all);
	}
	if (((int)measurement.resources & GPU) != 0) {
		printf("====GPU==== \n");
		printf("total time of gpu measuring  \t \t: %lf s\n", measurement.gpu_time_total);
		printf("consumed energy of gpu 	\t \t: %lf mWs \n", measurement.gpu_energy_total);
		printf("gpu utilization average	\t \t: %lf  %% \n", measurement.gpu_util_avg_gpu);
		printf("gpu mem utilization average	\t: %lf  %% \n", measurement.gpu_util_avg_mem);
	}
	if (((int)measurement.resources & FPGA) != 0) {
		printf("====FPGA==== \n");
		printf("total time of fpga measuring \t \t: %lf s\n", measurement.fpga_time_total);
		printf("consumed energy of fpga \t \t: %lf mWs \n", measurement.fpga_energy_total);
		printf("consumed energy of fpga vcc1v0_core \t: %lf mWs \n", measurement.fpga_energy_total_vcc1v0_core);
		printf("consumed energy of fpga vcc1v5_ddr \t: %lf mWs \n", measurement.fpga_energy_total_vcc1v5_ddr);
		printf("consumed energy of fpga vcc2v5_aux \t: %lf mWs \n", measurement.fpga_energy_total_vcc2v5_aux);
		printf("consumed energy of fpga imgt1v0 \t: %lf mWs \n", measurement.fpga_energy_total_imgt_1v0);
		printf("consumed energy of fpga imgt1v2 \t: %lf mWs \n", measurement.fpga_energy_total_imgt_1v2);
		printf("consumed energy of fpga mgt1v0 \t \t: %lf mWs \n", measurement.fpga_energy_total_mgt_1v0);
		printf("consumed energy of fpga mgt1v2 \t \t: %lf mWs \n", measurement.fpga_energy_total_mgt_1v2);
		printf("fpga utilization average  \t \t: %lf  %% \n", measurement.fpga_util_avg_comp);
	}
	if (((int)measurement.resources & SYSTEM) != 0) {
		printf("====SYSTEM==== \n");
		printf("total time of system board measuring \t: %lf s\n", measurement.sysboard_time_total);
		printf("consumed energy of system board \t: %lf Ws \n", measurement.sysboard_energy_total);
		printf("====SERVER==== \n");
		printf("total time of servcer measuring \t: %lf s\n", measurement.server_time_total);
		printf("consumed energy of server \t \t: %lf Ws \n", measurement.server_energy_total);
	}
}

int main(int argc, char **argv) {

	HN_MEASUREMENT measure;
	measure.resources = malloc(3 * sizeof(int32_t));
	if (NULL == measure.resources) {
		fprintf(stderr, "Couldn't allocate memory. (file: %s, line: %i)\n", __FILE__, __LINE__);
		return -1;
	}

	//Create 3 test measurements
	measure.resources[0] = CPU | GPU ;
	measure.resources[1] = GPU;	
	measure.resources[2] = CPU | GPU | FPGA | SYSTEM ;
	measure.sample_rate_cpu = 10;
	measure.sample_rate_gpu = 20;
	measure.sample_rate_fpga = 20;
	measure.sample_rate_sys = 50;
	measure.cpu_freq_max = 0;
	measure.cpu_freq_min = 0;
	measure.cpu_gov = CPU_GOVERNOR_ONDEMAND;
	measure.gpu_freq = GPU_FREQUENCY_CUR;

	printf("Initialize Measurement...");
	//inititalize measurement system with three test mesaurements
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	mshnw_init(&version, &measure, 3);
	printf("done");

	//start the Measurement System
	printf("Start the Measurement System...");
	mshnw_start();
	printf("done");
	// start the first measurement
	printf("Start Measurement 0...");
	mshnw_start_measurement(0);
	printf("done");
	printf("Start Measurement 2...");
	mshnw_start_measurement(2);
	printf("done");
	sleep(10);
	printf("Start Measurement 1...");
	mshnw_start_measurement(1);
	printf("done");
	sleep(5);
	printf("Stop Measurement 0...");
	mshnw_stop_measurement(0);
	printf("done");
	printf("Stop Measurement 1...");
	mshnw_stop_measurement(1);
	printf("done");
	printf("Stop Measurement 2...");
	mshnw_stop_measurement(2);
	printf("done");
	//shutdown the measurement System
	printf("Shutdown the measurement system...");
	mshnw_shutdown();
	printf("done");
	MEASUREMENT_DATA *results = mshnw_get_ms_data();
	printf("===RESULTS 0===\n");
	print_measurement(results[0]);
	printf("===RESULTS 1===\n");
	print_measurement(results[1]);
	printf("===RESULTS 2===\n");
	print_measurement(results[2]);
	mshnw_free();
	free(measure.resources);

	return EXIT_SUCCESS;
}
