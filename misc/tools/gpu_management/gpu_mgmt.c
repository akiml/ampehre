/*
 * gpu_mgmt.c
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 11/19/14
 * version: 0.2.3 - add gpu_management tool and use the tool in the libmeasure
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <nvml.h>

nvmlDevice_t device;
int verbose = 0;

void printhelp(){
	printf(
		"===GPU Management Tool==="
	    "Usage:\n"
	    "======\n"
	    "./gpu_management -m 'memory clock in MHz' -c 'gpu core clock in MHz' -p 'persistence mode' -r\n" 
		"The GPU management tool allows you to set the memory and core clock, to reset the clock frequencies\n and to enable or disable the persistence mode.\n \n"
	    "Arguments:\n"
	    "======\n"
	    "-m memory_clock \t| Memory clock frequency in MHz.\n"
		"-c core_clock \t \t| GPU core clock in MHz.\n"
		"\t \t \t| It is only possible to change the memory and core frequency at the same time.\n"
		"-p persistence_mode \t| 1: Enable the persistence mode\n"
		"\t \t \t| 0: Disable the persistence mode\n"
	    "-r \t \t \t| Reset memory and core clock frequencies.\n"
	    "-h \t \t \t| Prints this help.\n"
		"-v \t \t \t| Activate verbose mode.\n"
	);
}

void reset_clock(){
	nvmlReturn_t result;
	
	result = nvmlDeviceResetApplicationsClocks(device);
	
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: cannot reset clock frequencies!\n");
		exit(EXIT_FAILURE);
	}
	if(verbose)
		printf("GPU management: Reset memory and core clock!\n");
	
}

void set_clock(int mem_clock, int core_clock){
	nvmlReturn_t result;
	
	result = nvmlDeviceSetApplicationsClocks(device, mem_clock, core_clock);
	
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: cannot set clock frequencies!\n");
		exit(EXIT_FAILURE);
	}
	if(verbose)
		printf("GPU management: Set memory clock to %d MHz and core clock to %d MHz!\n", mem_clock, core_clock);
	
}

void set_persistence_mode(int mode){
	nvmlReturn_t result;
	
	if(mode){
		result = nvmlDeviceSetPersistenceMode(device,  NVML_FEATURE_ENABLED);
		if(verbose)
			printf("GPU management: Enabled persistence mode!\n");
	} else {
		result = nvmlDeviceSetPersistenceMode(device,  NVML_FEATURE_DISABLED);
		if(verbose)
			printf("GPU management: Disabled persistence mode!\n");
	}
	
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: cannot set persistence mode!\n");
		exit(EXIT_FAILURE);
	}
}

void nvml_init(){
	nvmlReturn_t result;
	uint32_t device_count;
	
	result = nvmlInit();
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: cannot initialize nvml library! \n");
		exit(EXIT_FAILURE);
	}
		
	result = nvmlDeviceGetCount(&device_count);
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: cannot query device count!\n");
		exit(EXIT_FAILURE);
	}
		
	if (device_count > 1) {
		fprintf(stderr, "GPU management error: this software has to be rewritten if you want to support more than 1 device.\n");
		exit(EXIT_FAILURE);
	}
	
	result = nvmlDeviceGetHandleByIndex(0, &device);
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: cannot get device handler.\n");
		exit(EXIT_FAILURE);
	}
	
}

void nvml_close(){
	nvmlReturn_t result;
	
	result = nvmlShutdown();
	if (NVML_SUCCESS != result) {
		fprintf(stderr, "GPU management error: Failed to destroy library environment.\n");
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char **argv) {
	int opt;
	int memory_clock = 0;
	int core_clock = 0;
	int per_mode = -1;
	int reset = 0;
		
	nvml_init();

	while ((opt = getopt(argc, argv, "m:c:p:rvh")) != -1) {
		switch (opt) {
			case 'm':
				memory_clock = atoi(optarg);
				break;
			case 'c':
				core_clock = atoi(optarg);
				break;
			case 'p':
				per_mode = atoi(optarg);
				break;
			case 'r':
				reset = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
			default:
				printhelp();
				return 0;
				break;
				break;
		}
	}
	
	if(per_mode == 0){
		set_persistence_mode(0);
	}
	
	if(per_mode == 1){
		set_persistence_mode(1);
	}
	
	if(reset){
		reset_clock();
	}
	
	if(memory_clock != 0 && core_clock !=0){
		set_clock(memory_clock, core_clock);
	} else {
		if((memory_clock!=0 && core_clock==0) || (memory_clock==0 && core_clock!=0)){
			fprintf(stderr,"GPU management error: Memory clock and core clock need to be set!");
			exit(EXIT_FAILURE);
		}
	}
		
	nvml_close();
	
	return 0;
}
