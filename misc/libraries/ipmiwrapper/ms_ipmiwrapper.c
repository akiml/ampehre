/*
 * ms_ipmiwrapper.c
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
 * created: 10/28/14
 * version: 0.2.1 - add support for IPMI to the measure driver
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 */

#include "ms_ipmiwrapper_internal.h"

static int fd = 0;

void init_ipmi_wrapper(MS_VERSION* version){
	
	if((version->major != MS_MAJOR_VERSION) || (version->minor != MS_MINOR_VERSION) || (version->revision != MS_REVISION_VERSION)){
		printf("Error in IPMI wrapper: Wrong version number! IPMI wrapper version %d.%d.%d is called from tool with version %d.%d.%d.\n", MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, version->major, version->minor, version->revision);
		exit(EXIT_FAILURE);
	}
	
	char *driver = "/dev/measure";
	if (fd != 0){
		printf("IPMI wrapper is already initialized!\n");
		return;
	}
	fd = open(driver, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Cannot open driver \"%s\".\n", driver);
		exit(EXIT_FAILURE);
	}	
}

void close_ipmi_wrapper(){
	int rv;
	char *driver = "/dev/measure";
	rv = close(fd);
	fd = 0;
	if(rv){
		fprintf(stderr, "Error while closing driver \"%s\".\n", driver);
		exit(EXIT_FAILURE);
	}
}

int dellResetEnergyCounter(){
	unsigned char datarc[MAX_MSG_LENGTH];
	int size;
	int count = 0;
	do{
		lseek(fd, IPMI_DELL_RESET_ENERGY, ipmi);
		size = read(fd, datarc, MAX_MSG_LENGTH);
		count++;
	}while(count < 50 && (datarc[0] != 0 || size < 1));
	if(datarc[0] != 0 || size < 1){
		fprintf(stderr, "Error in IPMI wrapper at dellResetEnergyCounter!\n");
		return -1;
	}
	return 0;
}

int dellCumulativeEnergy(uint32_t* time, uint32_t* result_energy){
	unsigned char datarc[MAX_MSG_LENGTH];
	int size;
	*time = 0;
	*result_energy  =  0;
	errno = 0;
	lseek(fd, IPMI_DELL_CUMULATIVE_ENERGY, ipmi);
	size = read(fd, datarc, MAX_MSG_LENGTH);
	if(datarc[0] != 0 || size != 25){
		if(errno == ETIMEDOUT){
			return -ETIMEDOUT;
		}
		fprintf(stderr, "Error in IPMI wrapper at dellCumulativeEnergy!\n");
		return -1;
	}
	
	*time |= datarc[1];
	*time |= (datarc[2] << 8);
	*time |= (datarc[3] << 16);
	*time |= (datarc[4] << 24);
	
	*result_energy |= datarc[5];
	*result_energy |= (datarc[6] << 8);
	*result_energy |= (datarc[7] << 16);
	*result_energy |= (datarc[8] << 24);

	return 0;
}

int dellCurrentPower(uint16_t* current_power){
	unsigned char datarc[MAX_MSG_LENGTH];
	int size;
	*current_power = 0;
	errno = 0;
	lseek(fd, IPMI_DELL_CURRENT_POWER, ipmi);
	size = read(fd, datarc, MAX_MSG_LENGTH);
	if(datarc[0] != 0 || size != 8){
		if(errno == ETIMEDOUT){
			return -ETIMEDOUT;
		}
		fprintf(stderr, "Error in IPMI wrapper at dellCurrentPower!\n");
		return -1;
	}
	
	*current_power |=  datarc[1];
	*current_power |=  (datarc[2] << 8);
	
	return 0;
}

int getTemperature(int record_id){
	int temperature = -1;
	int length = MAX_MSG_LENGTH;
	
	if((record_id < 1) || (record_id > 65535)){
		fprintf(stderr, "Error in IPMI wrapper at getPower: wrong record id %d!.\n", record_id);
		return -1;
	}
	
	unsigned char* sdr = malloc(sizeof(unsigned char)*length);
	int size = getSDR(record_id, sdr, length);
	if(size < 2){
		if(size == -ETIMEDOUT){
			return -ETIMEDOUT;
		}
		fprintf(stderr, "Error in IPMI wrapper at getTemperature, getSDR.\n");
		free(sdr);
		return -1;
	}
	int sensor_id = sdr[SDR_SENSOR_ID];
		
	unsigned char sensorReading[4];
	size = getSensorReading(sensor_id, sensorReading, 4);
	if(size < 2){
		if(size == -ETIMEDOUT){
			return -ETIMEDOUT;
		}
		fprintf(stderr, "Error in IPMI wrapper at getTemperature, getSensorReading.\n");
		free(sdr);
		return -1;
	}
	temperature = sensorReading[1];
	
	int M = (((sdr[SDR_M_MS_BITS] & 0xC0) << 2) | sdr[SDR_M_LS_BITS]);
	//twos complement negative
	if((M & 0x200)!=0){
		M = -((~M+1) & 0x3FF);
	}
	
	int B = (((sdr[SDR_B_MS_BITS] & 0xC0) << 2) | sdr[SDR_B_LS_BITS]);
	//twos complement negative
	if((B & 0x200)!=0){
		B = -((~B+1) & 0x3FF);
	}
	
	int Rexp = (sdr[SDR_EXPONENTS] & 0xF0)>>4;
	//twos complement negative
	if((Rexp & 0x8)!=0){
		Rexp = -((~Rexp+1) & 0xF);
	}
	
	int Bexp = (sdr[SDR_EXPONENTS] & 0x0F);
	//twos complement negative
	if((Bexp & 0x8)!=0){
		Bexp = -((~Bexp+1) & 0xF);
	}
	
	temperature = (M * temperature + B*pow(10, Bexp))*pow(10,Rexp);
	
	free(sdr);
	return temperature;
}

double getPower(int record_id){
	double power = -1;
	int length = MAX_MSG_LENGTH;
	
	if((record_id < 1) || (record_id > 65535)){
		fprintf(stderr, "Error in IPMI wrapper at getPower: wrong record id %d!.\n", record_id);
		return -1;
	}
	
	unsigned char* sdr = malloc(sizeof(unsigned char)*length);
	int size = getSDR(record_id, sdr, length);
	if(size < 2){
		if(size == -ETIMEDOUT){
			return -ETIMEDOUT;
		}
		fprintf(stderr, "Error in IPMI wrapper at getPower, getSDR.\n");
		free(sdr);
		return -1;
	}
	int sensor_id = sdr[SDR_SENSOR_ID];
	unsigned char sensorReading[4];
	size = getSensorReading(sensor_id, sensorReading, 4);
	if(size < 2){
		if(size == -ETIMEDOUT){
			return -ETIMEDOUT;
		}
		fprintf(stderr, "Error in IPMI wrapper at getPower, getSensorReading.\n");
		free(sdr);
		return -1;
	}
	power = sensorReading[1];

	int M = (((sdr[SDR_M_MS_BITS] & 0xC0) << 2) | sdr[SDR_M_LS_BITS]);
	//twos complement negative
	if((M & 0x200)!=0){
		M = -((~M+1) & 0x3FF);
	}

	int B = (((sdr[SDR_B_MS_BITS] & 0xC0) << 2) | sdr[SDR_B_LS_BITS]);
	//twos complement negative
	if((B & 0x200)!=0){
		B = -((~B+1) & 0x3FF);
	}

	int Rexp = (sdr[SDR_EXPONENTS] & 0xF0)>>4;
	//twos complement negative
	if((Rexp & 0x8)!=0){
		Rexp = -((~Rexp+1) & 0xF);
	}
	
	int Bexp = (sdr[SDR_EXPONENTS] & 0x0F);
	//twos complement negative
	if((Bexp & 0x8)!=0){
		Bexp = -((~Bexp+1) & 0xF);
	}

	power = (M * power + B*pow(10, Bexp))*pow(10,Rexp);
	free(sdr);
	return power;
}

int getIPMITimeout(){
	int rv;
	unsigned long ipmi_timeout;
	
	rv = ioctl(fd, IOC_GET_IPMI_TIMEOUT, &ipmi_timeout);
	if(rv){
		fprintf(stderr, "Error in IPMI wrapper couldn't get IPMI timeout.\n");
		return -1;
	}
	
	return ipmi_timeout;
}

int setIPMITimeoutIOCTL(uint64_t mode, uint32_t ipmi_timeout){
	int rv;
	unsigned long timeout = ipmi_timeout;
	
	rv = ioctl(fd, mode, &timeout);
	if(rv < 0){
		fprintf(stderr, "Error in IPMI wrapper couldn't set IPMI timeout.\n");
	}
	
	return rv;
}

int getSDR(int record_id, unsigned char* datarc, int size){
	int rv;
	errno = 0;
	lseek(fd, IPMI_GET_SDR | record_id, ipmi);
	rv = read(fd, datarc, size);
	if(rv < 1){
		return -errno;
	}
	return rv;
}

int getSensorReading(int sensor_id, unsigned char* datarc, int size){
	int rv;
	errno = 0;
	lseek(fd, IPMI_GET_SENSOR_READING | sensor_id, ipmi);
	rv = read(fd, datarc, size);
	if(rv < 1){
		return -errno;
	}
	return rv;
}
