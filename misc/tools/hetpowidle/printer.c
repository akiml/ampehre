/*
 * printer.c
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
 */

#include "printer.h"

#include <time.h>
#include "../../../cjson/cJSON.h"

// This is awful code...
// TODO: rewrite print_json()

void print_json(FILE *file, ARGUMENTS* settings, IDLE_POWER *ip_list, IDLE_POWER *ip_median) {
	cJSON *json_root		= cJSON_CreateObject();
	cJSON *json_host		= cJSON_CreateString(settings->hostname);
	cJSON *json_unixtime	= cJSON_CreateNumber((double)time(NULL));
	
	cJSON_AddItemToObject(json_root, "Hostname", json_host);
	cJSON_AddItemToObject(json_root, "UnixTimeStamp", json_unixtime);
	
	if (settings->idle_measurements & MEASURE_IDLE_CPU) {
		cJSON *json_res				= cJSON_CreateArray();
		cJSON *json_res_cpu_0_pkg	= cJSON_CreateArray();
		cJSON *json_res_cpu_0_dram	= cJSON_CreateArray();
		cJSON *json_res_cpu_1_pkg	= cJSON_CreateArray();
		cJSON *json_res_cpu_1_dram	= cJSON_CreateArray();
		
		cJSON_AddItemToArray(json_res           , cJSON_CreateNumber(ip_median->idle_power_cpu       ));
		cJSON_AddItemToArray(json_res_cpu_0_pkg , cJSON_CreateNumber(ip_median->idle_power_cpu_0_pkg ));
		cJSON_AddItemToArray(json_res_cpu_0_dram, cJSON_CreateNumber(ip_median->idle_power_cpu_0_dram));
		cJSON_AddItemToArray(json_res_cpu_1_pkg , cJSON_CreateNumber(ip_median->idle_power_cpu_1_pkg ));
		cJSON_AddItemToArray(json_res_cpu_1_dram, cJSON_CreateNumber(ip_median->idle_power_cpu_1_dram));
		
		cJSON *json_arr				= cJSON_CreateArray();
		cJSON *json_arr_cpu_0_pkg	= cJSON_CreateArray();
		cJSON *json_arr_cpu_0_dram	= cJSON_CreateArray();
		cJSON *json_arr_cpu_1_pkg	= cJSON_CreateArray();
		cJSON *json_arr_cpu_1_dram	= cJSON_CreateArray();
		
		IDLE_POWER *temp = ip_list;
		while (NULL != temp) {
			cJSON_AddItemToArray(json_arr           , cJSON_CreateNumber(temp->idle_power_cpu       ));
			cJSON_AddItemToArray(json_arr_cpu_0_pkg , cJSON_CreateNumber(temp->idle_power_cpu_0_pkg ));
			cJSON_AddItemToArray(json_arr_cpu_0_dram, cJSON_CreateNumber(temp->idle_power_cpu_0_dram));
			cJSON_AddItemToArray(json_arr_cpu_1_pkg , cJSON_CreateNumber(temp->idle_power_cpu_1_pkg ));
			cJSON_AddItemToArray(json_arr_cpu_1_dram, cJSON_CreateNumber(temp->idle_power_cpu_1_dram));
			temp = temp->next;
		}
		
		cJSON_AddItemToArray(json_res           , json_arr           );
		cJSON_AddItemToArray(json_res_cpu_0_pkg , json_arr_cpu_0_pkg );
		cJSON_AddItemToArray(json_res_cpu_0_dram, json_arr_cpu_0_dram);
		cJSON_AddItemToArray(json_res_cpu_1_pkg , json_arr_cpu_1_pkg );
		cJSON_AddItemToArray(json_res_cpu_1_dram, json_arr_cpu_1_dram);
		
		cJSON_AddItemToObject(json_root, "cpu_power_avg"      , json_res           );
		cJSON_AddItemToObject(json_root, "cpu0_power_avg_pkg" , json_res_cpu_0_pkg );
		cJSON_AddItemToObject(json_root, "cpu0_power_avg_dram", json_res_cpu_0_dram);
		cJSON_AddItemToObject(json_root, "cpu1_power_avg_pkg" , json_res_cpu_1_pkg );
		cJSON_AddItemToObject(json_root, "cpu1_power_avg_dram", json_res_cpu_1_dram);
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_GPU) {
		cJSON *json_res		= cJSON_CreateArray();
		cJSON_AddItemToArray(json_res, cJSON_CreateNumber(ip_median->idle_power_gpu));
		cJSON *json_arr		= cJSON_CreateArray();
		
		IDLE_POWER *temp = ip_list;
		while (NULL != temp) {
			cJSON_AddItemToArray(json_arr, cJSON_CreateNumber(temp->idle_power_gpu));
			temp = temp->next;
		}
		cJSON_AddItemToArray(json_res, json_arr);
		cJSON_AddItemToObject(json_root, "gpu_power_avg", json_res);
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_FPGA) {
		cJSON *json_res		= cJSON_CreateArray();
		cJSON_AddItemToArray(json_res, cJSON_CreateNumber(ip_median->idle_power_fpga));
		cJSON *json_arr		= cJSON_CreateArray();
		
		IDLE_POWER *temp = ip_list;
		while (NULL != temp) {
			cJSON_AddItemToArray(json_arr, cJSON_CreateNumber(temp->idle_power_fpga));
			temp = temp->next;
		}
		cJSON_AddItemToArray(json_res, json_arr);
		cJSON_AddItemToObject(json_root, "fpga_power_avg", json_res);
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_MIC) {
		cJSON *json_res		= cJSON_CreateArray();
		cJSON_AddItemToArray(json_res, cJSON_CreateNumber(ip_median->idle_power_mic));
		cJSON *json_arr		= cJSON_CreateArray();
		
		IDLE_POWER *temp = ip_list;
		while (NULL != temp) {
			cJSON_AddItemToArray(json_arr, cJSON_CreateNumber(temp->idle_power_mic));
			temp = temp->next;
		}
		cJSON_AddItemToArray(json_res, json_arr);
		cJSON_AddItemToObject(json_root, "mic_power_avg", json_res);
	}
	
	if (settings->idle_measurements & MEASURE_IDLE_SYS) {
		cJSON *json_res		= cJSON_CreateArray();
		cJSON_AddItemToArray(json_res, cJSON_CreateNumber(ip_median->idle_power_sys));
		cJSON *json_arr		= cJSON_CreateArray();
		
		IDLE_POWER *temp = ip_list;
		while (NULL != temp) {
			cJSON_AddItemToArray(json_arr, cJSON_CreateNumber(temp->idle_power_sys));
			temp = temp->next;
		}
		cJSON_AddItemToArray(json_res, json_arr);
		cJSON_AddItemToObject(json_root, "sys_power_avg", json_res);
	}
	
	cJSON *json_res		= cJSON_CreateArray();
	cJSON_AddItemToArray(json_res, cJSON_CreateNumber(ip_median->idle_power_all));
	cJSON *json_arr		= cJSON_CreateArray();
	
	IDLE_POWER *temp = ip_list;
	while (NULL != temp) {
		cJSON_AddItemToArray(json_arr, cJSON_CreateNumber(temp->idle_power_all));
		temp = temp->next;
	}
	cJSON_AddItemToArray(json_res, json_arr);
	cJSON_AddItemToObject(json_root, "all_power_avg", json_res);
	
	char *result = cJSON_Print(json_root);
	
	fwrite(result, strlen(result), 1, file);
	fflush(file);
}
