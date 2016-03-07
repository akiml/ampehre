/*
 * json_printer.c
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 12/2/15
 * version: 0.6.1 - add json printer to hettime
 *          0.7.0 - modularized measurement struct
 *          0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 */

#include "printer.h"
#include "../../cjson/cJSON.h"

static void add_config(cJSON *config, ARGUMENTS *settings);

static void add_cpu_values(cJSON *values, MS_LIST *m);
static void add_cpu_energy(cJSON *val_cpu, MS_LIST *m);
static void add_cpu_power(cJSON *val_cpu, MS_LIST *m);
static void add_cpu_temperature(cJSON *val_cpu, MS_LIST *m);
static void add_cpu_frequency(cJSON *val_cpu, MS_LIST *m);
static void add_cpu_memory(cJSON *val_cpu, MS_LIST *m);

static void add_gpu_values(cJSON *values, MS_LIST *m);

static void add_fpga_values(cJSON *values, MS_LIST *m);
static void add_fpga_energy(cJSON *val_fpga, MS_LIST *m);
static void add_fpga_power(cJSON *val_fpga, MS_LIST *m);

static void add_mic_values(cJSON *values, MS_LIST *m);
static void add_mic_energy(cJSON *val_mic, MS_LIST *m);
static void add_mic_power(cJSON* val_mic, MS_LIST *m);
static void add_mic_temperature(cJSON *val_mic, MS_LIST *m);

static void add_system_values(cJSON *values, MS_LIST *m);

static void add_odroid_values(cJSON *values, MS_LIST *m);
static void add_odroid_energy(cJSON *val_odroid, MS_LIST *m);
static void add_odroid_power(cJSON *val_odroid, MS_LIST *m);
static void add_odroid_temperature(cJSON *val_odroid, MS_LIST *m);
static void add_odroid_frequency(cJSON *val_odroid, MS_LIST *m);
static void add_odroid_utlization(cJSON *val_odroid, MS_LIST *m);

static void add_time_values(cJSON *values, EXEC_TIME *exec_time);

static void add_json_leaf_to_object(cJSON *parent, const char *name, double value, const char *unit);
static void add_json_leaf_to_array(cJSON *array, double value, const char *unit);

void print_json(FILE *json, ARGUMENTS *settings, MS_LIST *m, EXEC_TIME *exec_time){
	cJSON *root;
	cJSON *config;
	cJSON *values;
		
	root=cJSON_CreateObject();
	cJSON_AddItemToObject(root, "config", config = cJSON_CreateArray());
	cJSON_AddItemToObject(root, "values", values = cJSON_CreateArray());
	
	add_config(config, settings);
	
	if (NULL != getMeasurement(&m, CPU)) {
		add_cpu_values(values, m);
	}
	if (NULL != getMeasurement(&m, GPU)) {
		add_gpu_values(values, m);
	}
	if (NULL != getMeasurement(&m, FPGA)) {
		add_fpga_values(values, m);
	}
	if (NULL != getMeasurement(&m, MIC)) {
		add_mic_values(values, m);
	}
	if (NULL != getMeasurement(&m, SYSTEM)) {
		add_system_values(values, m);
	}
	if (NULL != getMeasurement(&m, ODROID)) {
		add_odroid_values(values, m);
	}
	
	add_time_values(values, exec_time);
	
	fprintf(json, "%s", cJSON_Print(root));
	
	cJSON_Delete(root);
}

static void add_json_leaf_to_object(cJSON *parent, const char *name, double value, const char *unit){
	cJSON *child;
	
	cJSON_AddItemToObject(parent, name, child = cJSON_CreateObject());
	cJSON_AddNumberToObject(child, "value", value);
	cJSON_AddStringToObject(child, "unit", unit);
}

static void add_json_leaf_to_array(cJSON *array, double value, const char *unit){
	cJSON *child;
	
	cJSON_AddItemToArray(array, child = cJSON_CreateObject());
	cJSON_AddNumberToObject(child, "value", value);
	cJSON_AddStringToObject(child, "unit", unit);
}

static void add_config(cJSON *config, ARGUMENTS *settings){
	cJSON *config_general;
	cJSON *config_cpu;
	cJSON *config_gpu;
	cJSON *config_fpga;
	cJSON *config_mic;
	cJSON *config_system;
	
	const char *variant;
	const char *skip_ms;
	const char *cpu_governor;
	const char *gpu_freq;
	
	switch(settings->variant){
		case VARIANT_LIGHT:
			variant = "VARIANT_LIGHT" ;
			break;
		case VARIANT_FULL:
		default:
			variant = "VARIANT_FULL";
	}
	
	switch(settings->skip_ms){
		case SKIP_PERIODIC:
			skip_ms = "SKIP_PERIODIC" ;
			break;
		case SKIP_NEVER:
		default:
			skip_ms = "SKIP_NEVER";
	}
	
	switch(settings->cpu_gov){
		case CPU_GOVERNOR_POWERSAVE:
			cpu_governor = "POWERSAVE";
			break;
		case CPU_GOVERNOR_CONSERVATIVE:
			cpu_governor = "CONSERVATIVE";
			break;
		case CPU_GOVERNOR_PERFORMANCE:
			cpu_governor = "PERFORMANCE";
			break;
		case CPU_GOVERNOR_ONDEMAND:
		default:
			cpu_governor = "ONDEMAND";
	}
	
	switch(settings->gpu_freq){
		case GPU_FREQUENCY_MIN:
			gpu_freq = "MIN";
			break;
		case GPU_FREQUENCY_MAX:
			gpu_freq = "MAX";
			break;
		case GPU_FREQUENCY_CUR:
		default:
			gpu_freq = "CUR";
	}
	
	cJSON_AddItemToArray(config, config_general = cJSON_CreateObject());
	cJSON_AddStringToObject(config_general, "variant", variant);
	cJSON_AddStringToObject(config_general, "skip_ms_freq", skip_ms);
	
	cJSON_AddItemToArray(config, config_cpu = cJSON_CreateObject());
	cJSON_AddStringToObject(config_cpu, "resource", "cpu");
	cJSON_AddNumberToObject(config_cpu, "sampling_rate", settings->sample_rate_cpu);
	cJSON_AddNumberToObject(config_cpu, "check_for_exit_interrupts", settings->check_for_exit_interrupts_cpu);
	cJSON_AddStringToObject(config_cpu, "governor", cpu_governor);
	cJSON_AddNumberToObject(config_cpu, "frequency_min", settings->cpu_freq_min);
	cJSON_AddNumberToObject(config_cpu, "frequency_max", settings->cpu_freq_max);
	
	cJSON_AddItemToArray(config, config_gpu = cJSON_CreateObject());
	cJSON_AddStringToObject(config_gpu, "resource", "gpu");
	cJSON_AddNumberToObject(config_gpu, "sampling_rate", settings->sample_rate_gpu);
	cJSON_AddNumberToObject(config_gpu, "check_for_exit_interrupts", settings->check_for_exit_interrupts_gpu);
	cJSON_AddStringToObject(config_gpu, "frequency", gpu_freq);
	
	cJSON_AddItemToArray(config, config_fpga = cJSON_CreateObject());
	cJSON_AddStringToObject(config_fpga, "resource", "fpga");
	cJSON_AddNumberToObject(config_fpga, "sampling_rate", settings->sample_rate_fpga);
	cJSON_AddNumberToObject(config_fpga, "check_for_exit_interrupts", settings->check_for_exit_interrupts_fpga);
	cJSON_AddBoolToObject(config_fpga, "force_idle", settings->force_idle_fpga);
	
	cJSON_AddItemToArray(config, config_system = cJSON_CreateObject());
	cJSON_AddStringToObject(config_system, "resource", "mic");
	cJSON_AddNumberToObject(config_system, "sampling_rate", settings->sample_rate_mic);
	cJSON_AddNumberToObject(config_system, "check_for_exit_interrupts", settings->check_for_exit_interrupts_mic);
	
	cJSON_AddItemToArray(config, config_mic = cJSON_CreateObject());
	cJSON_AddStringToObject(config_mic, "resource", "system");
	cJSON_AddNumberToObject(config_mic, "sampling_rate", settings->sample_rate_sys);
	cJSON_AddNumberToObject(config_mic, "check_for_exit_interrupts", settings->check_for_exit_interrupts_sys);
}


static void add_cpu_values(cJSON *values, MS_LIST *m){
	cJSON *val_cpu;
	cJSON *item;
	
	cJSON_AddItemToArray(values, val_cpu = cJSON_CreateObject());
	
	cJSON_AddStringToObject(val_cpu, "resource", "cpu");
	add_json_leaf_to_object(val_cpu, "runtime", cpu_time_total(m), "s");
	
	add_cpu_energy(val_cpu, m);
	add_cpu_power(val_cpu, m);
	add_cpu_temperature(val_cpu, m);
	add_cpu_frequency(val_cpu, m);
	
	cJSON_AddItemToObject(val_cpu, "utilization", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "time_avg_work",		cpu_active_avg_all(m),	"s");
	add_json_leaf_to_object(item, "time_avg_idle",		cpu_idle_avg_all(m),	"s");
	add_json_leaf_to_object(item, "utilization_avg",	cpu_util_avg_all(m),	"%");
	
	add_cpu_memory(val_cpu, m);
}

static void add_cpu_energy(cJSON *val_cpu, MS_LIST *m){
	cJSON *item;
	cJSON *array;
	
	cJSON_AddItemToObject(val_cpu, "energy_total", array = cJSON_CreateArray());
	
	for (int i = 0; i < CPUS; i++) {
		cJSON_AddItemToArray(array, item = cJSON_CreateObject());
		cJSON_AddNumberToObject(item, "cpu_no", i);
		add_json_leaf_to_object(item, "pkg",	cpu_energy_total_pkg(m, i),		"mWs");
		add_json_leaf_to_object(item, "pp0",	cpu_energy_total_pp0(m, i),		"mWs");
		add_json_leaf_to_object(item, "dram",	cpu_energy_total_dram(m, i),	"mWs");
	}
}
static void add_cpu_power(cJSON *val_cpu, MS_LIST *m){
	cJSON *item;
	cJSON *array;
	
	cJSON_AddItemToObject(val_cpu, "power_avg", array = cJSON_CreateArray());
	
	for (int i = 0; i < CPUS; i++) {
		cJSON_AddItemToArray(array, item = cJSON_CreateObject());
		cJSON_AddNumberToObject(item, "cpu_no", i);
		add_json_leaf_to_object(item, "pkg",	cpu_power_avg_pkg(m, i),	"mW");
		add_json_leaf_to_object(item, "pp0",	cpu_power_avg_pp0(m, i),	"mW");
		add_json_leaf_to_object(item, "dram",	cpu_power_avg_dram(m, i),	"mW");
	}
}

static void add_cpu_temperature(cJSON *val_cpu, MS_LIST *m){
	cJSON *item;
	cJSON *array;
	cJSON *array2;
	
	cJSON_AddItemToObject(val_cpu, "temperature_max", array = cJSON_CreateArray());
	
	for (int i = 0; i < CPUS; i++) {
		cJSON_AddItemToArray(array, item = cJSON_CreateObject());
		cJSON_AddNumberToObject(item, "cpu_no", i);
		add_json_leaf_to_object(item, "pkg", cpu_temp_max_pkg(m, i), "\u00b0C");
		cJSON_AddItemToObject(item, "core", array2 = cJSON_CreateArray());
		for(int j = 0; j < CORES; j++) {
			add_json_leaf_to_array(array2, cpu_temp_max_core(m, i, j), "\u00b0C");
		}
	}
}

static void add_cpu_frequency(cJSON *val_cpu, MS_LIST *m){
	cJSON *item;
	cJSON *subitem;
	cJSON *array;
	cJSON *array2;
	
	cJSON_AddItemToObject(val_cpu, "frequency", item = cJSON_CreateObject());
	cJSON_AddItemToObject(item, "average", array = cJSON_CreateArray());
	for (int i = 0; i < CPUS; i++) {
		cJSON_AddItemToArray(array, subitem = cJSON_CreateObject());
		cJSON_AddNumberToObject(subitem, "cpu_no", i);
		cJSON_AddItemToObject(subitem, "core", array2 = cJSON_CreateArray());
		for(int j = 0; j < CORES; j++) {
			add_json_leaf_to_array(array2, cpu_freq_avg_core(m, i, j), "MHz");
		}
	}
	cJSON_AddItemToObject(item, "effective", array = cJSON_CreateArray());
	for (int i = 0; i < CPUS; i++) {
		cJSON_AddItemToArray(array, subitem = cJSON_CreateObject());
		cJSON_AddNumberToObject(subitem, "cpu_no", i);
		cJSON_AddItemToObject(subitem, "core", array2 = cJSON_CreateArray());
		for(int j = 0; j < CORES; j++) {
			add_json_leaf_to_array(array2, cpu_freq_eff_core(m, i, j), "MHz");
		}
	}
}

static void add_cpu_memory(cJSON *val_cpu, MS_LIST *m){
	cJSON *item;
	
	cJSON_AddItemToObject(val_cpu, "memory", item = cJSON_CreateObject());
	
	add_json_leaf_to_object(item, "total",		cpu_memory_total(m),	"kiB");
	add_json_leaf_to_object(item, "max_used",	cpu_memory_used_max(m),	"kiB");
	add_json_leaf_to_object(item, "max_free",	cpu_memory_free_max(m),	"kiB");
	
	cJSON_AddItemToObject(val_cpu, "swap", item = cJSON_CreateObject());
	
	add_json_leaf_to_object(item, "total",		cpu_swap_total(m),		"kiB");
	add_json_leaf_to_object(item, "max_used",	cpu_swap_used_max(m),	"kiB");
	add_json_leaf_to_object(item, "max_free",	cpu_swap_free_max(m),	"kiB");
}

static void add_gpu_values(cJSON *values, MS_LIST *m){
	cJSON *val_gpu;
	cJSON *item;
	
	cJSON_AddItemToArray(values, val_gpu = cJSON_CreateObject());
	
	cJSON_AddStringToObject(val_gpu, "resource", "gpu");
	
	add_json_leaf_to_object(val_gpu, "runtime",			gpu_time_total(m),		"s");
	add_json_leaf_to_object(val_gpu, "energy_total", 	gpu_energy_total(m),	"mWs");
	add_json_leaf_to_object(val_gpu, "power_avg",		gpu_power_avg(m),		"mW");
	add_json_leaf_to_object(val_gpu, "temperature_max",	gpu_temp_max(m),		"\u00b0C");
	
	cJSON_AddItemToObject(val_gpu, "frequency_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "graphics",	gpu_freq_avg_graph(m),	"MHz");
	add_json_leaf_to_object(item, "sm",			gpu_freq_avg_sm(m),		"MHz");
	add_json_leaf_to_object(item, "memory",		gpu_freq_avg_mem(m),	"MHz");
	
	cJSON_AddItemToObject(val_gpu, "utilization_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "gpu",	gpu_util_avg_gpu(m),	"%");
	add_json_leaf_to_object(item, "memory",	gpu_util_avg_mem(m),	"%");
	
	cJSON_AddItemToObject(val_gpu, "memory", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "total",		gpu_memory_total(m),	"kiB");
	add_json_leaf_to_object(item, "max_used",	gpu_memory_used_max(m),	"kiB");
	add_json_leaf_to_object(item, "max_free",	gpu_memory_free_max(m),	"kiB");	
}

static void add_fpga_values(cJSON *values, MS_LIST *m){
	cJSON *val_fpga;
	cJSON *item;
	
	cJSON_AddItemToArray(values, val_fpga = cJSON_CreateObject());
	
	cJSON_AddStringToObject(val_fpga, "resource", "fpga");
	
	add_json_leaf_to_object(val_fpga, "runtime", fpga_time_total(m), "s");
	
	add_fpga_energy(val_fpga, m);
	add_fpga_power(val_fpga, m);
	
	cJSON_AddItemToObject(val_fpga, "temperature_max", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "imgt",	fpga_temp_max_ifpga(m),	"\u00b0C");
	add_json_leaf_to_object(item, "mgt",	fpga_temp_max_mfpga(m),	"\u00b0C");
	
	add_json_leaf_to_object(val_fpga, "utilization_avg", fpga_util_avg_comp(m),  "%");

}

static void add_fpga_energy(cJSON *val_fpga, MS_LIST *m){
	cJSON *item;
	
	cJSON_AddItemToObject(val_fpga, "energy_total", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "vcc1v0_c",	fpga_energy_total_vcc1v0_core(m),	"mWs");
	add_json_leaf_to_object(item, "vcc1v5_ddr",	fpga_energy_total_vcc1v5_ddr(m),	"mWs");
	add_json_leaf_to_object(item, "vcc2v5_aux",	fpga_energy_total_vcc2v5_aux(m),	"mWs");
	add_json_leaf_to_object(item, "imgt_1v0",	fpga_energy_total_imgt_1v0(m),		"mWs");
	add_json_leaf_to_object(item, "imgt_1v2",	fpga_energy_total_imgt_1v2(m),		"mWs");
	add_json_leaf_to_object(item, "mgt_1v0",	fpga_energy_total_mgt_1v0(m),		"mWs");
	add_json_leaf_to_object(item, "mgt_1v2", 	fpga_energy_total_mgt_1v2(m), 		"mWs");
	add_json_leaf_to_object(item, "total",		fpga_energy_total_power_usage(m),	"mWs");
}

static void add_fpga_power(cJSON *val_fpga, MS_LIST *m){
	cJSON *item;
	
	cJSON_AddItemToObject(val_fpga, "power_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "vcc1v0_c",	fpga_power_avg_vcc1v0_core(m),	"mW");
	add_json_leaf_to_object(item, "vcc1v5_ddr",	fpga_power_avg_vcc1v5_ddr(m),	"mW");
	add_json_leaf_to_object(item, "vcc2v5_aux", fpga_power_avg_vcc2v5_aux(m),	"mW");
	add_json_leaf_to_object(item, "imgt_1v0",	fpga_power_avg_imgt_1v0(m),		"mW");
	add_json_leaf_to_object(item, "imgt_1v2",	fpga_power_avg_imgt_1v2(m),		"mW");
	add_json_leaf_to_object(item, "mgt_1v0",	fpga_power_avg_mgt_1v0(m),		"mW");
	add_json_leaf_to_object(item, "mgt_1v2",	fpga_power_avg_mgt_1v2(m),		"mW");
	add_json_leaf_to_object(item, "total",		fpga_power_avg_power_usage(m),	"mW");
}

static void add_mic_values(cJSON *values, MS_LIST *m){
	cJSON *val_mic;
	cJSON *item;
	
	cJSON_AddItemToArray(values, val_mic = cJSON_CreateObject());
	
	cJSON_AddStringToObject(val_mic, "resource", "mic");
	
	add_json_leaf_to_object(val_mic, "runtime", mic_time_total(m), "s");
	
	add_mic_energy(val_mic, m);
	add_mic_power(val_mic, m);
	add_mic_temperature(val_mic, m);
	
	cJSON_AddItemToObject(val_mic, "frequency_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "core",	mic_freq_avg_core(m),	"MHz");
	add_json_leaf_to_object(item, "memory",	mic_freq_avg_mem(m),	"MHz");
	
	cJSON_AddItemToObject(val_mic, "utilization", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "time_avg_active",	mic_active_avg_all(m),	"s");
	add_json_leaf_to_object(item, "time_avg_idle", 		mic_idle_avg_all(m),	"s");
	add_json_leaf_to_object(item, "utilization_avg",	mic_util_avg_all(m),	"%");
	
	cJSON_AddItemToObject(val_mic, "memory", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "total",		mic_memory_total(m),	"kiB");
	add_json_leaf_to_object(item, "max_used",	mic_memory_used_max(m),	"kiB");
	add_json_leaf_to_object(item, "max_free", 	mic_memory_free_max(m),	"kiB");
	
}

static void add_mic_energy(cJSON *val_mic, MS_LIST *m){
	cJSON *item;
	
	cJSON_AddItemToObject(val_mic, "energy_total", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "pcie", 	mic_energy_total_pcie(m),		"mWs");
	add_json_leaf_to_object(item, "c2x3",	mic_energy_total_c2x3(m),		"mWs");
	add_json_leaf_to_object(item, "c2x4",	mic_energy_total_c2x4(m),		"mWs");
	add_json_leaf_to_object(item, "vccp",	mic_energy_total_vccp(m),		"mWs");
	add_json_leaf_to_object(item, "vddg",	mic_energy_total_vddg(m),		"mWs");
	add_json_leaf_to_object(item, "vddq",	mic_energy_total_vddq(m),		"mWs");
	add_json_leaf_to_object(item, "total",	mic_energy_total_power_usage(m),"mWs");
}

static void add_mic_power(cJSON* val_mic, MS_LIST *m){
	cJSON *item;
	
	cJSON_AddItemToObject(val_mic, "power_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "pcie",	mic_power_avg_pcie(m),			"mW");
	add_json_leaf_to_object(item, "c2x3",	mic_power_avg_c2x3(m),			"mW");
	add_json_leaf_to_object(item, "c2x4",	mic_power_avg_c2x4(m),			"mW");
	add_json_leaf_to_object(item, "vccp",	mic_power_avg_vccp(m),			"mW");
	add_json_leaf_to_object(item, "vddg",	mic_power_avg_vddg(m),			"mW");
	add_json_leaf_to_object(item, "vddq",	mic_power_avg_vddq(m),			"mW");
	add_json_leaf_to_object(item, "total",	mic_power_avg_power_usage(m),	"mW");
}

static void add_mic_temperature(cJSON *val_mic, MS_LIST *m){
	cJSON *item;
	
	cJSON_AddItemToObject(val_mic, "temperature_max", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "die",		mic_temp_max_die(m),		"\u00b0C");
	add_json_leaf_to_object(item, "gddr",		mic_temp_max_gddr(m),		"\u00b0C");
	add_json_leaf_to_object(item, "fan in",		mic_temp_max_fan_in(m),		"\u00b0C");
	add_json_leaf_to_object(item, "fan out",	mic_temp_max_fan_out(m),	"\u00b0C");
	add_json_leaf_to_object(item, "vccp",		mic_temp_max_vccp(m),		"\u00b0C");
	add_json_leaf_to_object(item, "vddg",		mic_temp_max_vddg(m),		"\u00b0C");
	add_json_leaf_to_object(item, "vddq",		mic_temp_max_vddq(m),		"\u00b0C");
}

static void add_system_values(cJSON *values, MS_LIST *m){
	cJSON *val_sys;
	cJSON *item;
	
	cJSON_AddItemToArray(values, val_sys = cJSON_CreateObject());
	cJSON_AddStringToObject(val_sys, "resource", "system");
	
	add_json_leaf_to_object(val_sys, "runtime", system_time_total(m), "s");
	
	cJSON_AddItemToObject(val_sys, "energy_total", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "board",	system_energy_board(m),	"Ws");
	add_json_leaf_to_object(item, "total",	system_energy_total(m),	"Ws");
	
	cJSON_AddItemToObject(val_sys, "power_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "board",	system_power_board_avg(m),	"W");
	add_json_leaf_to_object(item, "total",	system_power_avg(m),		"W");
	
	add_json_leaf_to_object(val_sys, "temperature_max",	system_temp_max(m),	"\u00b0C");
}

static void add_odroid_values(cJSON *values, MS_LIST *m){
	cJSON *val_odroid;
	
	cJSON_AddItemToArray(values, val_odroid = cJSON_CreateObject());
	
	cJSON_AddStringToObject(val_odroid, "resource", "odroid");
	
	add_json_leaf_to_object(val_odroid, "runtime", odroid_time_total(m), "s");
	
	add_odroid_energy(val_odroid, m);
	add_odroid_power(val_odroid, m);
	add_odroid_temperature(val_odroid, m);
	add_odroid_frequency(val_odroid, m);
	add_odroid_utlization(val_odroid, m);
}

static void add_odroid_energy(cJSON* val_odroid, MS_LIST* m) {
	cJSON *item;
	
	cJSON_AddItemToObject(val_odroid, "energy_total", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "a15"	, odroid_energy_total_a15(m)	, "mWs");
	add_json_leaf_to_object(item, "a7"	, odroid_energy_total_a7(m)		, "mWs");
	add_json_leaf_to_object(item, "mali", odroid_energy_total_mali(m)	, "mWs");
	add_json_leaf_to_object(item, "mem"	, odroid_energy_total_mem(m)	, "mWs");
}

static void add_odroid_power(cJSON* val_odroid, MS_LIST* m) {
	cJSON *item;
	
	cJSON_AddItemToObject(val_odroid, "power_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "a15"	, odroid_power_avg_a15(m)	, "mW");
	add_json_leaf_to_object(item, "a7"	, odroid_power_avg_a7(m)	, "mW");
	add_json_leaf_to_object(item, "mali", odroid_power_avg_mali(m)	, "mW");
	add_json_leaf_to_object(item, "mem"	, odroid_power_avg_mem(m)	, "mW");
}

static void add_odroid_temperature(cJSON* val_odroid, MS_LIST* m) {
	cJSON *item;
	
	cJSON_AddItemToObject(val_odroid, "temperature_max", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "a15_core_0"	, odroid_temp_max_a15(m, ODROID_TEMP_A15_CORE_0), "\u00b0C");
	add_json_leaf_to_object(item, "a15_core_1"	, odroid_temp_max_a15(m, ODROID_TEMP_A15_CORE_1), "\u00b0C");
	add_json_leaf_to_object(item, "a15_core_2"	, odroid_temp_max_a15(m, ODROID_TEMP_A15_CORE_2), "\u00b0C");
	add_json_leaf_to_object(item, "a15_core_3"	, odroid_temp_max_a15(m, ODROID_TEMP_A15_CORE_3), "\u00b0C");
	add_json_leaf_to_object(item, "mali"		, odroid_temp_max_mali(m)						, "\u00b0C");
}

static void add_odroid_frequency(cJSON* val_odroid, MS_LIST* m) {
	cJSON *item;
	
	cJSON_AddItemToObject(val_odroid, "frequency_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "a15"	, odroid_freq_avg_a15(m)	, "MHz");
	add_json_leaf_to_object(item, "a7"	, odroid_freq_avg_a7(m)		, "MHz");
	add_json_leaf_to_object(item, "mali", odroid_freq_avg_mali(m)	, "MHz");
}

static void add_odroid_utlization(cJSON* val_odroid, MS_LIST* m) {
	cJSON *item;
	
	cJSON_AddItemToObject(val_odroid, "utilization_avg", item = cJSON_CreateObject());
	add_json_leaf_to_object(item, "a15"	, odroid_util_avg_a15(m)	, "%");
	add_json_leaf_to_object(item, "a7"	, odroid_util_avg_a7(m)		, "%");
	add_json_leaf_to_object(item, "mali", odroid_util_avg_mali(m)	, "%");
}

static void add_time_values(cJSON *values, EXEC_TIME *exec_time) {
	cJSON *val_time;
	
	cJSON_AddItemToArray(values, val_time = cJSON_CreateObject());
	cJSON_AddStringToObject(val_time, "resource", "time");
	
	add_json_leaf_to_object(val_time, "real", exec_time->real, "s");
	add_json_leaf_to_object(val_time, "user", exec_time->user, "s");
	add_json_leaf_to_object(val_time, "sys", exec_time->sys, "s");
	
}