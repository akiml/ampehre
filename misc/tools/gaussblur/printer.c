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
 * created: 1/11/15
 * version: 0.2.5 - add gaussblur example application
 *          0.7.0 - modularized measurement struct
 */

#include "printer.h"

#include "../../../cjson/cJSON.h"

void print_json(FILE *file, ARGUMENTS *settings, MS_LIST *m) {
	cJSON *json		= cJSON_CreateObject();
	
	double energy	= 0.0;
	double runtime	= 0.0;
	
	energy	+= cpu_energy_total_pkg(m, 0);
	energy	+= cpu_energy_total_dram(m, 0);
	energy	+= cpu_energy_total_pkg(m, 1);
	energy	+= cpu_energy_total_dram(m, 1);
	energy	+= gpu_energy_total(m);
	energy	+= fpga_energy_total_power_usage(m);
	
	energy	/= settings->energy_loops;
	
	runtime	+= gpu_time_total(m);
	
	runtime	/= settings->energy_loops;
	
	cJSON_AddItemToObject(json, "runtime", cJSON_CreateNumber(runtime));
	cJSON_AddItemToObject(json, "energy", cJSON_CreateNumber(energy));
	
	char *result = cJSON_Print(json);
	
	fwrite(result, strlen(result), 1, file);
	fflush(file);
}
