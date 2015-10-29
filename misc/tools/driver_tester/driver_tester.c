/*
 * driver_tester.c
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 8/27/14
 * version: 0.1.6 - add an measuring system driver for cpu stat readings
 *          0.2.0 - add support for msr registers to the measure driver
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.3.3 - add cpu memory info to measure driver
 *          0.5.12 - add ioctl call to driver to configure the ipmi timeout
 */

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/ioctl.h>

#include "../../../include/ms_driver.h"
#include "../../../include/ms_ipmiwrapper.h"
//#include "../../../include/measurement.h"

#define CORES 4
#define CPUS 2

int main(int argc, char **argv);
void print_cpustats(int fildes, int64_t cpu_num);
void cpustat_test(int fildes);
void msr_read_test(int fildes, uint64_t cpu, int comp);
void msr_write_test(int fildes, uint64_t cpu);

void print_cpustats(int fildes, int64_t cpu_num) {
	int i;
	uint64_t cpustat[CPUSTATS];
	
	lseek(fildes, cpu_num*0x1000000000000, cpu_stats);
	read(fildes, cpustat, CPUSTATS*sizeof(uint64_t));
	
	if (cpu_num == CORES*CPUS) {
		printf("cpu ");
	} else {
		printf("cpu%i", (int)cpu_num);
	}
	
	for (i=0; i<CPUSTATS; ++i) {
		printf(" %li", cpustat[i]);
	}
	
	printf("\n");
}

void cpustat_test(int fildes){
	int i = 0;
	printf("==CPU STAT TEST==\n");
	print_cpustats(fildes, CORES*CPUS);
	
	for (i=0; i<CPUS*CORES; ++i) {
		print_cpustats(fildes, i);
	}
}

void msr_read_test(int fildes, uint64_t cpu, int comp){
	unsigned long long int msr = 0;
	unsigned long long int msr2 = 0;
	ssize_t sz = 0;
	int msr_reg;
	int fildes_orig = -1;
	printf("==MSR READ TEST CORE %d==\n", (int)(cpu/CORE1));
	if(comp){
		char* device = malloc(sizeof(char)*15);
		snprintf(device, 15, "/dev/cpu/%d/msr", (int)(cpu/CORE1));
		fildes_orig = open(device, O_RDWR);
		if (fildes_orig < 0) {
			fprintf(stderr, "Cannot open driver/dev/cpu/%d/msr.\n",(int)(cpu/CORE1));
			exit(EXIT_FAILURE);
		}
		
	}
	msr_reg = MSR_RAPL_POWER_UNIT;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (MSR_RAPL_POWER_UNIT) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
		
	msr_reg = MSR_PKG_ENERGY_STATUS;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (MSR_PKG_ENERGY_STATUS) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = MSR_PP0_ENERGY_STATUS;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (MSR_PP0_ENERGY_STATUS) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = MSR_PP1_ENERGY_STATUS;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (MSR_PP1_ENERGY_STATUS) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = MSR_DRAM_ENERGY_STATUS;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (MSR_DRAM_ENERGY_STATUS) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = IA32_THERM_STATUS;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (IA32_THERM_STATUS) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = IA32_PACKAGE_THERM_STATUS;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (IA32_PACKAGE_THERM_STATUS) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = MSR_TEMPERATURE_TARGET;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (MSR_TEMPERATURE_TARGET) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = IA32_TIME_STAMP_COUNTER;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (IA32_TIME_STAMP_COUNTER) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = IA32_MPERF;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (IA32_MPERF) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	msr_reg = IA32_APERF;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &msr, sizeof(msr));
	printf("Read from MSR 0x%x (IA32_APERF) = 0x%llx, ",msr_reg, msr);	
	printf("Error Code: %d \n", (int) sz);
	
	if(comp){
		msr_reg = MSR_RAPL_POWER_UNIT;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (MSR_RAPL_POWER_UNIT) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = MSR_PKG_ENERGY_STATUS;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (MSR_PKG_ENERGY_STATUS) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);	
		msr2 = 0;
		msr_reg = MSR_PP0_ENERGY_STATUS;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (MSR_PP0_ENERGY_STATUS) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = MSR_PP1_ENERGY_STATUS;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (MSR_PP1_ENERGY_STATUS) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = MSR_DRAM_ENERGY_STATUS;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (MSR_DRAM_ENERGY_STATUS) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = IA32_THERM_STATUS;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (IA32_THERM_STATUS) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = IA32_PACKAGE_THERM_STATUS;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (IA32_PACKAGE_THERM_STATUS) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = MSR_TEMPERATURE_TARGET;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (MSR_TEMPERATURE_TARGET) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = IA32_TIME_STAMP_COUNTER;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (IA32_TIME_STAMP_COUNTER) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = IA32_MPERF;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (IA32_MPERF) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
		msr2 = 0;
		msr_reg = IA32_APERF;
		lseek(fildes_orig, msr_reg, SEEK_SET);
		sz = read(fildes_orig, &msr2, sizeof(msr2));
		printf("Read from original msr driver MSR 0x%x (IA32_APERF) = 0x%llx, ", msr_reg, msr2);
		printf("Error Code: %d \n",(int) sz);
	}
	
	close(fildes_orig);
}

void msr_write_test(int fildes, uint64_t cpu){
	uint64_t data;
	ssize_t sz;
	int msr_reg;
	
	printf("==MSR WRITE TEST CORE %d==\n", (int)(cpu/CORE1));
	
	data = 0x1111222233334444;
	msr_reg = IA32_MPERF;
	printf("Write %lx to MSR 0x%x (IA32_MPERF), ", data, msr_reg);
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = write(fildes, &data, sizeof(data));
	printf("Error Code: %d \n",(int) sz);
	data = 0;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &data, sizeof(data));
	printf("Read from MSR 0x%x (IA32_MPERF) = 0x%lx, ",msr_reg, data);	
	printf("Error Code: %d \n", (int) sz);
	
	data = 0x5555666677778888;
	msr_reg = IA32_APERF;
	printf("Write %lx to MSR 0x%x (IA32_APERF), ", data, msr_reg);
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = write(fildes, &data, sizeof(data));
	printf("Error Code: %d \n",(int) sz);
	data = 0;
	lseek(fildes, cpu | msr_reg, cpu_msr);
	sz = read(fildes, &data, sizeof(data));
	printf("Read from MSR 0x%x (IA32_APERF) = 0x%lx, ",msr_reg, data);	
	printf("Error Code: %d \n", (int) sz);
	
}

void ipmi_test(int fildes){
	printf("==IPMI RAW TEST==\n");
	unsigned char data[MAX_MSG_LENGTH];
	int size;
	int i = 0;
	int j = 0;
	
	for(i = 1; i < 20; i++){
		lseek(fildes, IPMI_GET_SENSOR_READING | i, ipmi);
		size = read(fildes, data, sizeof(data));
		printf("IPMI Sensor Reading ID %d: ", i);
		for(j = 0; j < size; j++){
			printf("%x ", data[j]);
		}
		printf("Error Code: %d\n", size);
	}
	
	for(i = 1; i < 20; i++){
		lseek(fildes, IPMI_GET_SDR | i, ipmi);
		size = read(fildes, data, sizeof(data));
		printf("IPMI Get SDR Record ID %d: ", i);
		for(j = 0; j < size; j++){
			printf("%x ", data[j]);
		}
		printf("Error Code: %d\n", size);
	}
	
	lseek(fildes, IPMI_DELL_RESET_ENERGY, ipmi);
	size = read(fildes, data, sizeof(data));
	printf("IPMI Dell Reset Energy: ");
	for(j = 0; j < size; j++){
		printf("%x ", data[j]);
	}
	printf("Error Code: %d\n", size);
	
	lseek(fildes, IPMI_DELL_CUMULATIVE_ENERGY, ipmi);
	size = read(fildes, data, sizeof(data));
	printf("IPMI Dell Cumulative Energy: ");
	for(j = 0; j < size; j++){
		printf("%x ", data[j]);
	}
	printf("Error Code: %d\n", size);
	
	lseek(fildes, IPMI_DELL_CURRENT_POWER, ipmi);
	size = read(fildes, data, sizeof(data));
	printf("IPMI Dell Current Power: ");
	for(j = 0; j < size; j++){
		printf("%x ", data[j]);
	}
	printf("Error Code: %d\n", size);
}

void ipmi_wrapper_test(){
	struct timespec ts_start, ts_end;
	printf("==IPMI WRAPPER TEST==\n");
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	init_ipmi_wrapper(&version);
	
#if 0
	int i=0;
	for(i = 0; i < 50; i++){
		printf("Record ID: %d Temperature: %d\n",i, getTemperature(i));
	}
	for(i = 50; i < 100; i++){
		printf("Record ID: %d Power: %f\n",i, getPower(i));
	}
#endif
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Temperature: %d \u00b0C\n",153, getTemperature(153));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Temperature: %d \u00b0C\n",154, getTemperature(154));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Temperature: %d \u00b0C\n",138, getTemperature(138));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Temperature: %d \u00b0C\n",205, getTemperature(205));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Temperature: %d \u00b0C\n",272, getTemperature(272));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",808, getPower(808));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1009, getPower(1009));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1143, getPower(1143));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1210, getPower(1210));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1277, getPower(1277));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1344, getPower(1344));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1411, getPower(1411));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1478, getPower(1478));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	clock_gettime(CLOCK_REALTIME, &ts_start);
	printf("Record ID: %d Power: %f\n",1545, getPower(1545));
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	
	clock_gettime(CLOCK_REALTIME, &ts_start);
	int rv = dellResetEnergyCounter();
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Dell ResetEnergyCounter: %d\n", rv);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	
	uint16_t curr_power=0;
	clock_gettime(CLOCK_REALTIME, &ts_start);
	rv = dellCurrentPower(&curr_power);
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Dell CurrentPower: %u Error Code: %d\n", curr_power,rv);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	
	uint32_t time = 0;
	uint32_t energy = 0;
	clock_gettime(CLOCK_REALTIME, &ts_start);
	rv = dellCumulativeEnergy(&time, &energy);
	clock_gettime(CLOCK_REALTIME, &ts_end);
	printf("Dell Cumulative Energy: %u Time: %u Error Code: %d\n",time, energy, rv);
	printf("Needed %.2f ms for ipmi request. \n", (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1000000);
	
	rv = getIPMITimeout();
	if(rv < 0){
		printf("Error in getIPMITImeout!\n");
	}
	printf("Current IPMI timeout: %d.\n", rv);
	
	uint32_t new_timeout=100;
	rv = setIPMITimeoutIOCTL(IOC_SET_IPMI_TIMEOUT, new_timeout);
	if(rv == ERROR_IPMI_TIMEOUT_LOCKED){
		printf("Error IPMI timeout is locked!\n");
	} else if(rv == ERROR_IPMI_TIMEOUT_MAX){
		printf("Error IPMI timeout is to large. Set timeout to default!\n");
	} else if(rv){
		printf("Error couldn't set IPMI timeout!\n");
	} else {
		printf("Set IPMI timeout: %d.\n", new_timeout);
	}
	
	new_timeout = 150;
	rv = setIPMITimeoutIOCTL(IOC_SET_AND_LOCK_IPMI_TIMEOUT, new_timeout);
	if(rv == ERROR_IPMI_TIMEOUT_MAX){
		printf("Error IPMI timeout is to large. Set timeout to default!\n");
	} else if(rv){
		printf("Error couldn't set IPMI timeout!\n");
	} else {
		printf("Set and locked IPMI timeout to: %d.\n", new_timeout);
	}
	
	new_timeout=100;
	rv = setIPMITimeoutIOCTL(IOC_SET_IPMI_TIMEOUT, new_timeout);
	if(rv == ERROR_IPMI_TIMEOUT_LOCKED){
		printf("Error IPMI timeout is locked!\n");
	} else if(rv == ERROR_IPMI_TIMEOUT_MAX){
		printf("Error IPMI timeout is to large. Set timeout to default!\n");
	} else if(rv){
		printf("Error couldn't set IPMI timeout!\n");
	} else {
		printf("Set IPMI timeout: %d.\n", new_timeout);
	}
	
	rv = getIPMITimeout();
	if(rv < 0){
		printf("Error in getIPMITImeout!\n");
	}
	printf("Current IPMI timeout: %d.\n", rv);
	
	new_timeout = 200;
	rv = setIPMITimeoutIOCTL(IOC_SET_AND_LOCK_IPMI_TIMEOUT, new_timeout);
	if(rv == ERROR_IPMI_TIMEOUT_MAX){
		printf("Error IPMI timeout is to large. Set timeout to default!\n");
	} else if(rv){
		printf("Error couldn't set IPMI timeout!\n");
	} else {
		printf("Set and locked IPMI timeout to: %d.\n", new_timeout);
	}
	
	rv = getIPMITimeout();
	if(rv < 0){
		printf("Error in getIPMITImeout!\n");
	}
	printf("Current IPMI timeout: %d.\n", rv);
	
	close_ipmi_wrapper();
}

void cpu_meminfo_test(int fildes, size_t size){
	size_t sz;
	uint64_t meminfo[MEMINFO_SIZE];
	char* buf;
	size_t i = 0;

	printf("==MEMINFO TEST==\n");
#ifndef LSEEK_MAX_2
	lseek(fildes, 0, cpu_meminfo);
#else
	lseek(fildes, MEMINFO, cpu_meminfo);
#endif
	sz = read(fildes, &meminfo, MEMINFO_SIZE * sizeof(uint64_t));
	if(sz <0 ){
		printf("Error Code: %d \n",(int) sz);
	}
	
	printf("Total RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_TOTAL] >> 10);
	printf("Free RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_FREE] >> 10);
	printf("Used RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_USED] >> 10);
	printf("Total Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_TOTAL] >> 10);
	printf("Free Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_FREE] >> 10);
	printf("Used Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_USED] >> 10);
	
	
	buf = malloc(size << 20);
	printf("\nAllocated %zd MiB. \n\n", size);
	if(NULL == buf){
		printf("Couldn't allocate memory!");
		return;
	}
	
	for(i = 0; i < (size << 20); i+=(1<<12)){
		buf[i] = 42;
	}
	
#ifndef LSEEK_MAX_2
	lseek(fildes, 0, cpu_meminfo);
#else
	lseek(fildes, MEMINFO, cpu_meminfo);
#endif
	sz = read(fildes, &meminfo, MEMINFO_SIZE * sizeof(uint64_t));
	if(sz <0 ){
		printf("Error Code: %d \n",(int) sz);
	}
	
	printf("Total RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_TOTAL] >> 10);
	printf("Free RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_FREE] >> 10);
	printf("Used RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_USED] >> 10);
	printf("Total Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_TOTAL] >> 10);
	printf("Free Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_FREE] >> 10);
	printf("Used Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_USED] >> 10);
	
	free(buf);
	printf("\nFreed %zd MiB. \n\n", size);
	
#ifndef LSEEK_MAX_2
	lseek(fildes, 0, cpu_meminfo);
#else
	lseek(fildes, MEMINFO, cpu_meminfo);
#endif
	sz = read(fildes, &meminfo, MEMINFO_SIZE * sizeof(uint64_t));
	if(sz <0 ){
		printf("Error Code: %d \n",(int) sz);
	}
	
	printf("Total RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_TOTAL] >> 10);
	printf("Free RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_FREE] >> 10);
	printf("Used RAM: \t %8lu MiB\n", meminfo[CPU_MEM_RAM_USED] >> 10);
	printf("Total Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_TOTAL] >> 10);
	printf("Free Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_FREE] >> 10);
	printf("Used Swap: \t %8lu MiB\n", meminfo[CPU_MEM_SWAP_USED] >> 10);	
}

void ipmi_timeout_test(int fildes){
	unsigned long ipmi_timeout;
	int rv;
	printf("==IPMI SET/GET TIMEOUT TEST==\n");
	
	rv = ioctl(fildes, IOC_GET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Error couldn't get IPMI timeout.");
	}
	printf("Current IPMI timeout: %lu\n", ipmi_timeout);
	
	ipmi_timeout = 100;
	rv = ioctl(fildes, IOC_SET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Set IPMI timeout error code: %d \n", rv);
	}
	rv = ioctl(fildes, IOC_GET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Error couldn't get IPMI timeout.");
	}
	printf("Current IPMI timeout: %lu\n", ipmi_timeout);
	
	ipmi_timeout = 150;
	rv = ioctl(fildes, IOC_SET_AND_LOCK_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Set and lock IPMI timeout error code: %d \n", rv);
	}
	
	rv = ioctl(fildes, IOC_GET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Error couldn't get IPMI timeout.");
	}
	printf("Current IPMI timeout: %lu\n", ipmi_timeout);
	
	ipmi_timeout = 100;
	rv = ioctl(fildes, IOC_SET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Set IPMI timeout error code: %d \n", rv);
	}
	
	rv = ioctl(fildes, IOC_GET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		printf("Error couldn't get IPMI timeout.");
	}
	printf("Current IPMI timeout: %lu\n", ipmi_timeout);
}

int main(int argc, char **argv) {
	char *driver = "/dev/measure";
	
	printf("=====Driver Tester=====\n");
	int fildes = open(driver, O_RDWR);
	if (fildes < 0) {
		fprintf(stderr, "Cannot open driver \"%s\".\n", driver);
		exit(EXIT_FAILURE);
	}
	
	cpustat_test(fildes);
	
	msr_read_test(fildes, CORE0, 0);
	msr_read_test(fildes, CORE1, 0);
	
	msr_write_test(fildes, CORE0);
	msr_write_test(fildes, CORE1);
	
	ipmi_test(fildes);
		
	ipmi_wrapper_test();
	
	cpu_meminfo_test(fildes, 8192);
	
	ipmi_timeout_test(fildes);
	
	close(fildes);		
	return EXIT_SUCCESS;
}
