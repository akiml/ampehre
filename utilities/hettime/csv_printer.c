/*
 * csv_printer.c
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
 * created: 7/29/14
 * version: 0.1.11 - add a seperate csv printer file to hettime tool
 *          0.5.6 - extended hettime csv printer
 */

#include "printer.h"

static void print_csv_cpu(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m);
static void print_csv_gpu(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m);
static void print_csv_fpga(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m);
static void print_csv_mic(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m);
static void print_csv_sysboard(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m);
static void print_csv_server(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m);
static void print_csv_settings(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, ARGUMENTS* settings);

void print_csv(FILE *csv, ARGUMENTS* settings, MEASUREMENT *m, EXEC_TIME *exec_time) {
	int cur_caption_pos = 0;
	int cur_unit_pos 	= 0;
	int cur_value_pos 	= 0;
	
	char* captions 	= malloc(MAX_HEADER_LENGTH*sizeof(char));
	char* units		= malloc(MAX_UNITS_LENGTH);
	char* values 	= malloc(MAX_VALUES_LENGTH*sizeof(char));
	
	memset(captions, 0, MAX_HEADER_LENGTH*sizeof(char));
	memset(units, 0, MAX_UNITS_LENGTH*sizeof(char));
	memset(values, 0, MAX_VALUES_LENGTH*sizeof(char));
		
	cur_caption_pos += snprintf(captions + cur_caption_pos, MAX_HEADER_LENGTH - cur_caption_pos, "time_total_exec_child;"
								"unix_timestamp;");
	cur_value_pos += snprintf(values + cur_value_pos, MAX_VALUES_LENGTH - cur_value_pos, "%lf;%lu;", exec_time->exec_time_diff, (uint64_t)time(NULL));
	cur_unit_pos += snprintf(units + cur_unit_pos, MAX_UNITS_LENGTH - cur_unit_pos,"s;s;");
	
	print_csv_cpu(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, m);
	print_csv_gpu(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, m);
	print_csv_fpga(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, m);
	print_csv_mic(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, m);
	print_csv_sysboard(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, m);
	print_csv_server(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, m);
	print_csv_settings(csv, captions, &cur_caption_pos, units, &cur_unit_pos, values, &cur_value_pos, settings);
		
	if (ftell(csv) == 0) {
		fprintf(csv,"%s\n", captions);
		fprintf(csv,"%s\n", units);
	}
	
	fprintf(csv,"%s\n", values);
	
	free(captions);
	free(values);
	free(units);
}

static void print_csv_cpu(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m){
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "CPU;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_total_measure_cpu;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", cpu_time_total(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_cpu0_pkg;"
								"energy_total_cpu0_pp0;"
								"energy_total_cpu0_dram;"
								"power_avg_cpu0_pkg;"
								"power_avg_cpu0_pp0;"
								"power_avg_cpu0_dram;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;%lf;%lf;%lf;", cpu_energy_total_pkg(m, 0),
								cpu_energy_total_pp0(m, 0),
								cpu_energy_total_dram(m, 0),
								cpu_power_avg_pkg(m, 0),
								cpu_power_avg_pp0(m, 0),
								cpu_power_avg_dram(m, 0));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mWs;mWs;mWs;mW;mW;mW;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "temp_max_cpu0_pkg;"
								"temp_max_cpu0_c0;"
								"temp_max_cpu0_c1;"
								"temp_max_cpu0_c2;"
								"temp_max_cpu0_c4;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;%u;%u;%u;%u;", cpu_temp_max_pkg(m, 0),
								cpu_temp_max_core(m, 0, 0),
								cpu_temp_max_core(m, 0, 1),
								cpu_temp_max_core(m, 0, 2),
								cpu_temp_max_core(m, 0, 3));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"\u00b0C;\u00b0C;\u00b0C;\u00b0C;\u00b0C;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "freq_avg_cpu0_c0;"
								"freq_avg_cpu0_c1;"
								"freq_avg_cpu0_c2;"
								"freq_avg_cpu0_c3;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.1lf;%.1lf;%.1lf;%.1lf;", cpu_freq_avg_core(m, 0, 0),
								cpu_freq_avg_core(m, 0, 1),
								cpu_freq_avg_core(m, 0, 2),
								cpu_freq_avg_core(m, 0, 3));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"MHz;MHz;MHz;MHz;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "freq_eff_cpu0_c0;"
								"freq_eff_cpu0_c1;"
								"freq_eff_cpu0_c2;"
								"freq_eff_cpu0_c3;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.1lf;%.1lf;%.1lf;%.1lf;", cpu_freq_eff_core(m, 0, 0),
								cpu_freq_eff_core(m, 0, 1),
								cpu_freq_eff_core(m, 0, 2),
								cpu_freq_eff_core(m, 0, 3));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"MHz;MHz;MHz;MHz;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_cpu1_pkg;"
								"energy_total_cpu1_pp0;"
								"energy_total_cpu1_dram;"
								"power_avg_cpu1_pkg;"
								"power_avg_cpu1_pp0;"
								"power_avg_cpu1_dram;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;%lf;%lf;%lf;", cpu_energy_total_pkg(m, 1),
								cpu_energy_total_pp0(m, 1),
								cpu_energy_total_dram(m, 1),
								cpu_power_avg_pkg(m, 1),
								cpu_power_avg_pp0(m, 1),
								cpu_power_avg_dram(m, 1));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mWs;mWs;mWs;mW;mW;mW;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "temp_max_cpu1_pkg;"
								"temp_max_cpu1_c0;"
								"temp_max_cpu1_c1;"
								"temp_max_cpu1_c2;"
								"temp_max_cpu1_c4;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;%u;%u;%u;%u;", cpu_temp_max_pkg(m, 1),
								cpu_temp_max_core(m, 1, 0),
								cpu_temp_max_core(m, 1, 1),
								cpu_temp_max_core(m, 1, 2),
								cpu_temp_max_core(m, 1, 3));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"\u00b0C;\u00b0C;\u00b0C;\u00b0C;\u00b0C;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "freq_avg_cpu1_c0;"
								"freq_avg_cpu1_c1;"
								"freq_avg_cpu1_c2;"
								"freq_avg_cpu1_c3;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.1lf;%.1lf;%.1lf;%.1lf;", cpu_freq_avg_core(m, 1, 0),
								cpu_freq_avg_core(m, 1, 1),
								cpu_freq_avg_core(m, 1, 2),
								cpu_freq_avg_core(m, 1, 3));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"MHz;MHz;MHz;MHz;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "freq_eff_cpu1_c0;"
								"freq_eff_cpu1_c1;"
								"freq_eff_cpu1_c2;"
								"freq_eff_cpu1_c3;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.1lf;%.1lf;%.1lf;%.1lf;", cpu_freq_eff_core(m, 1, 0),
								cpu_freq_eff_core(m, 1, 1),
								cpu_freq_eff_core(m, 1, 2),
								cpu_freq_eff_core(m, 1, 3));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"MHz;MHz;MHz;MHz;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_avg_cpus_work;"
								"time_avg_cpus_idle;"
								"util_avg_cpus;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.2lf;%.2lf;%.2lf;", cpu_active_avg_all(m),
								cpu_idle_avg_all(m),
								cpu_util_avg_all(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;s;%%;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "memory_total_cpu;"
								"memory_max_cpu_used;"
								"memory_max_cpu_free;"
								"swap_total_cpu;"
								"swap_max_cpu_used;"
								"swap_max_cpu_free;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%d;%d;%d;%d;%d;%d;", cpu_memory_total(m),
								cpu_memory_used_max(m),
								cpu_memory_free_max(m),
								cpu_swap_total(m),
								cpu_swap_used_max(m),
								cpu_swap_free_max(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"kiB;kiB;kiB;kiB;kiB;kiB;");
}

static void print_csv_gpu(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m){
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "GPU;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_total_measure_gpu;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", gpu_time_total(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_gpu;"
								"power_avg_gpu;"
								"temp_max_gpu;"
								"freq_avg_gpu_graph;"
								"freq_avg_gpu_sm;"
								"freq_avg_gpu_mem;"
								"util_avg_gpu_gpu;"
								"util_avg_gpu_mem;"
								"memory_total_gpu;"
								"memory_max_gpu_used;"
								"memory_max_gpu_free;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%u;%lf;%lf;%lf;%lf;%lf;%d;%d;%d;", gpu_energy_total(m),
								gpu_power_avg(m),
								gpu_temp_max(m),
								gpu_freq_avg_graph(m),
								gpu_freq_avg_sm(m),
								gpu_freq_avg_mem(m),
								gpu_util_avg_gpu(m),
								gpu_util_avg_mem(m),
								gpu_memory_total(m),
								gpu_memory_used_max(m),
								gpu_memory_free_max(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mWs;mW;\u00b0C;MHz;MHz;MHz;%%;%%;kiB;kiB;kiB;");
}

static void print_csv_fpga(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m){
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "FPGA;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_total_measure_fpga;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", fpga_time_total(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_vcc1v0;"
								"energy_total_vcc1v5_ddr;"
								"energy_total_vcc2v5_aux;"
								"energy_total_imgt_1v0;"
								"energy_total_imgt_1v2;"
								"energy_total_mgt_1v0;"
								"energy_total_mgt_1v2;"
								"energy_total_fpga;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;", fpga_energy_total_vcc1v0_core(m),
								fpga_energy_total_vcc1v5_ddr(m),
								fpga_energy_total_vcc2v5_aux(m),
								fpga_energy_total_imgt_1v0(m),
								fpga_energy_total_imgt_1v2(m),
								fpga_energy_total_mgt_1v0(m),
								fpga_energy_total_mgt_1v2(m),
								fpga_energy_total_power_usage(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mWs;mWs;mWs;mWs;mWs;mWs;mWs;mWs;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "power_avg_vcc1v0;"
								"power_avg_vcc1v5_ddr;"
								"power_avg_vcc2v5_aux;"
								"power_avg_imgt_1v0;"
								"power_avg_imgt_1v2;"
								"power_avg_mgt_1v0;"
								"power_avg_mgt_1v2;"
								"power_avg_fpga;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;", fpga_power_avg_vcc1v0_core(m),
								fpga_power_avg_vcc1v5_ddr(m),
								fpga_power_avg_vcc2v5_aux(m),
								fpga_power_avg_imgt_1v0(m),
								fpga_power_avg_imgt_1v2(m),
								fpga_power_avg_mgt_1v0(m),
								fpga_power_avg_mgt_1v2(m),
								fpga_power_avg_power_usage(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mW;mW;mW;mW;mW;mW;mW;mW;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "temp_max_fpga_imgt;"
								"temp_max_fpga_imgt;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;", fpga_temp_max_ifpga(m),
								fpga_temp_max_mfpga(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"\u00b0C;\u00b0C;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "util_avg_fpga_comp;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", fpga_util_avg_comp(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"%%;");
}

static void print_csv_mic(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m){
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "MIC;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_total_measure_mic;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", mic_time_total(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_pcie;"
								"energy_total_c2x3;"
								"energy_total_c2x4;"
								"energy_total_vccp;"
								"energy_total_vddg;"
								"energy_total_vddq;"
								"energy_total_mic;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;", mic_energy_total_pcie(m),
								mic_energy_total_c2x3(m),
								mic_energy_total_c2x4(m),
								mic_energy_total_vccp(m),
								mic_energy_total_vddg(m),
								mic_energy_total_vddq(m),
								mic_energy_total_power_usage(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mWs;mWs;mWs;mWs;mWs;mWs;mWs;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "power_avg_pcie;"
								"power_avg_c2x3;"
								"power_avg_c2x4;"
								"power_avg_vccp;"
								"power_avg_vddg;"
								"power_avg_vddq;"
								"power_avg_mic;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;", mic_power_avg_pcie(m),
								mic_power_avg_c2x3(m),
								mic_power_avg_c2x4(m),
								mic_power_avg_vccp(m),
								mic_power_avg_vddg(m),
								mic_power_avg_vddq(m),
								mic_power_avg_power_usage(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"mW;mW;mW;mW;mW;mW;mW;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "temp_max_die;"
								"temp_max_gddr;"
								"temp_max_fan_in;"
								"temp_max_fan_out;"
								"temp_max_vccp;"
								"temp_max_vddg;"
								"temp_max_vddg;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;%u;%u;%u;%u;%u;%u;", mic_temp_max_die(m),
								mic_temp_max_gddr(m),
								mic_temp_max_fan_in(m),
								mic_temp_max_fan_out(m),
								mic_temp_max_vccp(m),
								mic_temp_max_vddg(m),
								mic_temp_max_vddq(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"\u00b0C;\u00b0C;\u00b0C;\u00b0C;\u00b0C;\u00b0C;\u00b0C;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "freq_avg_mic_core;"
								"freq_avg_mic_mem;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.1lf;%.1lf;", mic_freq_avg_core(m),
								mic_freq_avg_mem(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"MHz;MHz;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_avg_mic_active;"
								"time_avg_mic_idle;"
								"util_avg_mic;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%.2lf;%.2lf;%.2lf;", mic_active_avg_all(m),
								mic_idle_avg_all(m),
								mic_util_avg_all(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;s;%%;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "memory_total_mic;"
								"memory_max_mic_used;"
								"memory_max_mic_free;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%d;%d;%d;", mic_memory_total(m),
								mic_memory_used_max(m),
								mic_memory_free_max(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"kiB;kiB;kiB;");
}

static void print_csv_sysboard(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m){

	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "SYSTEM_BOARD;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_total_measure_sysb;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", sysboard_time_total(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_sysb;"
								"power_avg_sysb;"
								"temp_max_sysb;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;%lf;", sysboard_energy_total(m),
								sysboard_power_avg(m),
								sysboard_temp_max(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"Ws;W;\u00b0C;");
}

static void print_csv_server(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, MEASUREMENT* m){
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "SERVER;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "time_total_measure_serv;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;", server_time_total(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"s;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "energy_total_serv;"
								"power_avg_serv;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%lf;%lf;", server_energy_total(m),
								server_power_avg(m));
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"Ws;W;");
}

static void print_csv_settings(FILE *csv, char* captions, int* cur_caption_pos, char* units, int* cur_unit_pos, char* values, int* cur_value_pos, ARGUMENTS* settings){
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "SETTINGS;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, ";");
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,";");
	
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "sampling_rate_cpu;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;", settings->sample_rate_cpu);
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"ms;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "sampling_rate_gpu;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;", settings->sample_rate_gpu);
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"ms;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "sampling_rate_fpga;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;", settings->sample_rate_fpga);
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"ms;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "sampling_rate_mic;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;", settings->sample_rate_mic);
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"ms;");
	*cur_caption_pos += snprintf(captions + *cur_caption_pos, MAX_HEADER_LENGTH - *cur_caption_pos, "sampling_rate_system;");
	*cur_value_pos += snprintf(values + *cur_value_pos, MAX_VALUES_LENGTH - *cur_value_pos, "%u;", settings->sample_rate_sys);
	*cur_unit_pos += snprintf(units + *cur_unit_pos, MAX_UNITS_LENGTH - *cur_unit_pos,"ms;");	
}
