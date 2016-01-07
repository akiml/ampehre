/*
 * list.c
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
 * created: 10/27/14
 * version: 0.1.19 - add a hettime based idle power measurement tool
 *          0.7.0 - modularised measurement struct
 */

#include "list.h"

#include <float.h>

void append_list_create_element(IDLE_POWER **ip_list, ARGUMENTS *settings, MS_LIST *m) {
	IDLE_POWER *ip = create_element(settings, m);
	
	append_list(ip_list, settings, ip);
}

void append_list(IDLE_POWER **ip_list, ARGUMENTS *settings, IDLE_POWER *ip) {
	if (NULL == *ip_list) {
		*ip_list = ip;
		return;
	}
	
	IDLE_POWER *temp = *ip_list;
	
	while (NULL != temp->next) {
		temp = temp->next;
	}
	
	temp->next 	= ip;
	ip->next	= NULL;
	ip->prev	= temp;
}

IDLE_POWER *create_element(ARGUMENTS *settings, MS_LIST  *m) {
	IDLE_POWER* ip = malloc(sizeof(IDLE_POWER));
	if (NULL == ip) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	
	ip->idle_power_cpu_0_pkg	= 0.0;
	ip->idle_power_cpu_0_dram	= 0.0;
	ip->idle_power_cpu_1_pkg	= 0.0;
	ip->idle_power_cpu_1_dram	= 0.0;
	ip->idle_power_cpu			= 0.0;
	ip->idle_power_gpu			= 0.0;
	ip->idle_power_fpga			= 0.0;
	ip->idle_power_mic			= 0.0;
	ip->idle_power_sys			= 0.0;
	ip->idle_power_all			= 0.0;
	ip->next = NULL;
	ip->prev = NULL;
	
	if (settings->idle_measurements & MEASURE_IDLE_CPU) {
		ip->idle_power_cpu_0_pkg	+= cpu_power_avg_pkg(m, 0);
		ip->idle_power_cpu_0_dram	+= cpu_power_avg_dram(m, 0);
		ip->idle_power_cpu_1_pkg	+= cpu_power_avg_pkg(m, 1);
		ip->idle_power_cpu_1_dram	+= cpu_power_avg_dram(m, 1);
		ip->idle_power_cpu			+= ip->idle_power_cpu_0_pkg + ip->idle_power_cpu_0_dram;
		ip->idle_power_cpu			+= ip->idle_power_cpu_1_pkg + ip->idle_power_cpu_1_dram;
		ip->idle_power_all			+= ip->idle_power_cpu;
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_GPU) {
		ip->idle_power_gpu	+= gpu_power_avg(m);
		ip->idle_power_all	+= ip->idle_power_gpu;
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_FPGA) {
		ip->idle_power_fpga	+= fpga_power_avg_power_usage(m);
		ip->idle_power_all	+= ip->idle_power_fpga;
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_MIC) {
		ip->idle_power_mic	+= mic_power_avg_power_usage(m);
		ip->idle_power_all	+= ip->idle_power_mic;
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_SYS) {
		ip->idle_power_sys	+= system_power_avg(m)*1000.0;
		ip->idle_power_sys	-= ip->idle_power_cpu + ip->idle_power_gpu + ip->idle_power_fpga;
		ip->idle_power_all	+= ip->idle_power_sys;
	}
	
	return ip;
}

void sort_list (IDLE_POWER **ip_list, ARGUMENTS *settings) {
	IDLE_POWER *sorted_ip_list	= NULL;
	IDLE_POWER *temp_min		= NULL;
	IDLE_POWER *temp			= *ip_list;
	double min					= DBL_MAX;
	
	int i;
	for (i=0; i<MEASURE_IDLE_ITERATIONS; ++i) {
		temp	= *ip_list;
		min		= DBL_MAX;
		
		while (NULL != temp) {
			if (temp->idle_power_all < min) {
				min			= temp->idle_power_all;
				temp_min	= temp;
			}
			
			temp = temp->next;
		}
		
		if (NULL != temp_min->prev) {
			temp_min->prev->next	= temp_min->next;
		} else {
			*ip_list = temp_min->next;
		}
		if (NULL != temp_min->next) {
			temp_min->next->prev	= temp_min->prev;
		}
		
		temp_min->next = NULL;
		temp_min->prev = NULL;
		
		append_list(&sorted_ip_list, settings, temp_min);
	}
	
	*ip_list = sorted_ip_list;
}

IDLE_POWER *get_median(IDLE_POWER **ip_list, ARGUMENTS *settings) {
	sort_list(ip_list, settings);
	
	IDLE_POWER *temp = *ip_list;
	if (NULL == temp) {
		return NULL;
	}
	
	int i;
	for (i=0; i<MEASURE_IDLE_ITERATIONS/2; ++i) {
		temp = temp->next;
	}
	
	return temp;
}

void delete_list(IDLE_POWER **ip_list) {
	IDLE_POWER *temp = *ip_list;
	
	while (NULL != temp) {
		if (NULL != temp->next) {
			temp = temp->next;
			free(temp->prev);
		} else {
			free(temp);
			temp = NULL;
		}
	}
	
	*ip_list = NULL;
}
