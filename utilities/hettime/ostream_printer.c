/*
 * ostream_printer.c
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
 * version: 0.1.0 - initial implementation
 *          0.1.2 - add output of GPU frequency measurements
 *          0.1.3 - add output of CPU frequency measurements
 *          0.1.5 - add CPU performance counters
 *          0.1.7 - add CPU utilization measurements
 *          0.1.8 - add GPU utilization measurements
 *          0.1.9 - add FPGA utilization measurements
 *          0.1.10 - add child execution time measurement
 *          0.1.11 - add a seperate csv printer file to hettime tool
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.7.0 - modularized measurement struct
 *          0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 */

#include "printer.h"

static void print_ostream_cpu(FILE *file, ARGUMENTS *settings, MS_LIST *m);
static void print_ostream_gpu(FILE *file, ARGUMENTS *settings, MS_LIST *m);
static void print_ostream_fpga(FILE *file, ARGUMENTS *settings, MS_LIST *m);
static void print_ostream_mic(FILE *file, ARGUMENTS *settings, MS_LIST *m);
static void print_ostream_system(FILE *file, ARGUMENTS *settings, MS_LIST *m);
static void print_ostream_time(FILE *file, EXEC_TIME *exec_time);

void print_ostream(FILE *file, ARGUMENTS *settings, MS_LIST *m, EXEC_TIME *exec_time) {
	fprintf(file,
		"RESULTS:\n========\n"
		"time total exec child   [ s ]: %lf\n"
		"unix timestamp          [ s ]: %lu\n",
		exec_time->exec_time_diff,
		(uint64_t)time(NULL)
	);
	
	fprintf(file,
		"\nCPU:\n====\n"
	);
	print_ostream_cpu(file, settings, m);
	
	fprintf(file,
		"\nGPU:\n====\n"
	);
	print_ostream_gpu(file, settings, m);
	
	fprintf(file,
		"\nFPGA:\n=====\n"
	);
	print_ostream_fpga(file, settings, m);
	
	fprintf(file,
		"\nMIC:\n=====\n"
	);
	print_ostream_mic(file, settings, m);
	
	fprintf(file,
		"\nSystem:\n=======\n"
	);
	print_ostream_system(file, settings, m);
	
	fprintf(file,
		"\nTime:\n=======\n"
	);
	print_ostream_time(file, exec_time);
}

static void print_ostream_cpu(FILE *file, ARGUMENTS *settings, MS_LIST *m) {
	fprintf(file,
		"time total measure cpu  [ s ]: %lf\n\n",
		cpu_time_total(m)
	);
	
	fprintf(file,
		"energy total cpu 0 pkg  [mWs]: %lf\n"
		"energy total cpu 0 pp0  [mWs]: %lf\n"
		"energy total cpu 0 dram [mWs]: %lf\n"
		"power  avg   cpu 0 pkg  [mW ]: %lf\n"
		"power  avg   cpu 0 pp0  [mW ]: %lf\n"
		"power  avg   cpu 0 dram [mW ]: %lf\n",
		cpu_energy_total_pkg(m, 0),
		cpu_energy_total_pp0(m, 0),
		cpu_energy_total_dram(m, 0),
		cpu_power_avg_pkg(m, 0),
		cpu_power_avg_pp0(m, 0),
		cpu_power_avg_dram(m, 0)
	);
	
	fprintf(file,
		"temp   max   cpu 0 pkg  [\u00b0C ]: %u\n"
		"temp   max   cpu 0 c 0  [\u00b0C ]: %u\n"
		"temp   max   cpu 0 c 1  [\u00b0C ]: %u\n"
		"temp   max   cpu 0 c 2  [\u00b0C ]: %u\n"
		"temp   max   cpu 0 c 3  [\u00b0C ]: %u\n",
		cpu_temp_max_pkg(m, 0),
		cpu_temp_max_core(m, 0, 0),
		cpu_temp_max_core(m, 0, 1),
		cpu_temp_max_core(m, 0, 2),
		cpu_temp_max_core(m, 0, 3)
	);
	
	fprintf(file,
		"freq   avg   cpu 0 c 0  [MHz]: %.1lf\n"
		"freq   avg   cpu 0 c 1  [MHz]: %.1lf\n"
		"freq   avg   cpu 0 c 2  [MHz]: %.1lf\n"
		"freq   avg   cpu 0 c 3  [MHz]: %.1lf\n",
		cpu_freq_avg_core(m, 0, 0),
		cpu_freq_avg_core(m, 0, 1),
		cpu_freq_avg_core(m, 0, 2),
		cpu_freq_avg_core(m, 0, 3)
	);
	
	fprintf(file,
		"freq   eff   cpu 0 c 0  [MHz]: %.1lf\n"
		"freq   eff   cpu 0 c 1  [MHz]: %.1lf\n"
		"freq   eff   cpu 0 c 2  [MHz]: %.1lf\n"
		"freq   eff   cpu 0 c 3  [MHz]: %.1lf\n",
		cpu_freq_eff_core(m, 0, 0),
		cpu_freq_eff_core(m, 0, 1),
		cpu_freq_eff_core(m, 0, 2),
		cpu_freq_eff_core(m, 0, 3)
	);
	
	fprintf(file,
		"energy total cpu 1 pkg  [mWs]: %lf\n"
		"energy total cpu 1 pp0  [mWs]: %lf\n"
		"energy total cpu 1 dram [mWs]: %lf\n"
		"power  avg   cpu 1 pkg  [mW ]: %lf\n"
		"power  avg   cpu 1 pp0  [mW ]: %lf\n"
		"power  avg   cpu 1 dram [mW ]: %lf\n",
		cpu_energy_total_pkg(m, 1),
		cpu_energy_total_pp0(m, 1),
		cpu_energy_total_dram(m, 1),
		cpu_power_avg_pkg(m, 1),
		cpu_power_avg_pp0(m, 1),
		cpu_power_avg_dram(m, 1)
	);
	
	fprintf(file,
		"temp   max   cpu 1 pkg  [\u00b0C ]: %u\n"
		"temp   max   cpu 1 c 0  [\u00b0C ]: %u\n"
		"temp   max   cpu 1 c 1  [\u00b0C ]: %u\n"
		"temp   max   cpu 1 c 2  [\u00b0C ]: %u\n"
		"temp   max   cpu 1 c 3  [\u00b0C ]: %u\n",
		cpu_temp_max_pkg(m, 1),
		cpu_temp_max_core(m, 1, 0),
		cpu_temp_max_core(m, 1, 1),
		cpu_temp_max_core(m, 1, 2),
		cpu_temp_max_core(m, 1, 3)
	);
	
	fprintf(file,
		"freq   avg   cpu 1 c 0  [MHz]: %.1lf\n"
		"freq   avg   cpu 1 c 1  [MHz]: %.1lf\n"
		"freq   avg   cpu 1 c 2  [MHz]: %.1lf\n"
		"freq   avg   cpu 1 c 3  [MHz]: %.1lf\n",
		cpu_freq_avg_core(m, 1, 0),
		cpu_freq_avg_core(m, 1, 1),
		cpu_freq_avg_core(m, 1, 2),
		cpu_freq_avg_core(m, 1, 3)
	);
	
	
	fprintf(file,
		"freq   eff   cpu 1 c 0  [MHz]: %.1lf\n"
		"freq   eff   cpu 1 c 1  [MHz]: %.1lf\n"
		"freq   eff   cpu 1 c 2  [MHz]: %.1lf\n"
		"freq   eff   cpu 1 c 3  [MHz]: %.1lf\n",
		cpu_freq_eff_core(m, 1, 0),
		cpu_freq_eff_core(m, 1, 1),
		cpu_freq_eff_core(m, 1, 2),
		cpu_freq_eff_core(m, 1, 3)
	);
	
	fprintf(file,
		"time   avg   cpus  work [ s ]: %.2lf\n"
		"time   avg   cpus  idle [ s ]: %.2lf\n"
		"util   avg   cpus       [ %% ]: %.2lf\n",
		cpu_active_avg_all(m),
		cpu_idle_avg_all(m),
		cpu_util_avg_all(m)
	);
	
	fprintf(file,
		"memory total cpu        [kiB]: %d\n"
		"memory max   cpu used   [kiB]: %d\n"
		"memory max   cpu free   [kiB]: %d\n"
		"swap   total cpu        [kiB]: %d\n"
		"swap   max   cpu used   [kiB]: %d\n"
		"swap   max   cpu free   [kiB]: %d\n",
		cpu_memory_total(m),
		cpu_memory_used_max(m),
		cpu_memory_free_max(m),
		cpu_swap_total(m),
		cpu_swap_used_max(m),
		cpu_swap_free_max(m)
	);
}

static void print_ostream_gpu(FILE *file, ARGUMENTS *settings, MS_LIST *m) {
	fprintf(file,
		"time total measure gpu  [ s ]: %lf\n\n",
		gpu_time_total(m)
	);
	
	fprintf(file,
		"energy total gpu        [mWs]: %lf\n"
		"power  avg   gpu        [mW ]: %lf\n"
		"temp   max   gpu        [\u00b0C ]: %u\n"
		"freq   avg   gpu graph  [MHz]: %lf\n"
		"freq   avg   gpu sm     [MHz]: %lf\n"
		"freq   avg   gpu mem    [MHz]: %lf\n"
		"util   avg   gpu gpu    [ %% ]: %lf\n"
		"util   avg   gpu mem    [ %% ]: %lf\n"
		"memory total gpu        [kiB]: %d\n"
		"memory max   gpu used   [kiB]: %d\n"
		"memory max   gpu free   [kiB]: %d\n",
		gpu_energy_total(m),
		gpu_power_avg(m),
		gpu_temp_max(m),
		gpu_freq_avg_graph(m),
		gpu_freq_avg_sm(m),
		gpu_freq_avg_mem(m),
		gpu_util_avg_gpu(m),
		gpu_util_avg_mem(m),
		gpu_memory_total(m),
		gpu_memory_used_max(m),
		gpu_memory_free_max(m)
	);
}

static void print_ostream_fpga(FILE *file, ARGUMENTS *settings, MS_LIST *m) {
	fprintf(file,
		"time total measure fpga [ s ]: %lf\n\n",
		fpga_time_total(m)
	);
	
	fprintf(file,
		"energy total vcc1v0_c   [mWs]: %lf\n"
		"energy total vcc1v5_ddr [mWs]: %lf\n"
		"energy total vcc2v5_aux [mWs]: %lf\n"
		"energy total imgt_1v0   [mWs]: %lf\n"
		"energy total imgt_1v2   [mWs]: %lf\n"
		"energy total mgt_1v0    [mWs]: %lf\n"
		"energy total mgt_1v2    [mWs]: %lf\n"
		"energy total fpga       [mWs]: %lf\n",
		fpga_energy_total_vcc1v0_core(m),
		fpga_energy_total_vcc1v5_ddr(m),
		fpga_energy_total_vcc2v5_aux(m),
		fpga_energy_total_imgt_1v0(m),
		fpga_energy_total_imgt_1v2(m),
		fpga_energy_total_mgt_1v0(m),
		fpga_energy_total_mgt_1v2(m),
		fpga_energy_total_power_usage(m)
	);
	
	fprintf(file,
		"power  avg   vcc1v0_c   [mW ]: %lf\n"
		"power  avg   vcc1v5_ddr [mW ]: %lf\n"
		"power  avg   vcc2v5_aux [mW ]: %lf\n"
		"power  avg   imgt_1v0   [mW ]: %lf\n"
		"power  avg   imgt_1v2   [mW ]: %lf\n"
		"power  avg   mgt_1v0    [mW ]: %lf\n"
		"power  avg   mgt_1v2    [mW ]: %lf\n"
		"power  avg   fpga       [mW ]: %lf\n",
		fpga_power_avg_vcc1v0_core(m),
		fpga_power_avg_vcc1v5_ddr(m),
		fpga_power_avg_vcc2v5_aux(m),
		fpga_power_avg_imgt_1v0(m),
		fpga_power_avg_imgt_1v2(m),
		fpga_power_avg_mgt_1v0(m),
		fpga_power_avg_mgt_1v2(m),
		fpga_power_avg_power_usage(m)
	);
	
	fprintf(file,
		"temp   max   fpga  imgt [\u00b0C ]: %lf\n"
		"temp   max   fpga  mmgt [\u00b0C ]: %lf\n",
		fpga_temp_max_ifpga(m),
		fpga_temp_max_mfpga(m)
	);
	
	fprintf(file,
		"util   avg   fpga  comp [ %% ]: %lf\n",
		 fpga_util_avg_comp(m)
	);
}

static void print_ostream_mic(FILE *file, ARGUMENTS *settings, MS_LIST *m){
	fprintf(file,
		"time total measure mic  [ s ]: %lf\n\n",
		mic_time_total(m)
	);
	
	fprintf(file,
		"energy total pcie       [mWs]: %lf\n"
		"energy total c2x3       [mWs]: %lf\n"
		"energy total c2x4       [mWs]: %lf\n"
		"energy total vccp       [mWs]: %lf\n"
		"energy total vddg       [mWs]: %lf\n"
		"energy total vddq       [mWs]: %lf\n"
		"energy total mic        [mWs]: %lf\n",
		mic_energy_total_pcie(m),
		mic_energy_total_c2x3(m),
		mic_energy_total_c2x4(m),
		mic_energy_total_vccp(m),
		mic_energy_total_vddg(m),
		mic_energy_total_vddq(m),
		mic_energy_total_power_usage(m)
	);
	
	fprintf(file,
		"power  avg   pcie       [mW ]: %lf\n"
		"power  avg   c2x3       [mW ]: %lf\n"
		"power  avg   c2x4       [mW ]: %lf\n"
		"power  avg   vccp       [mW ]: %lf\n"
		"power  avg   vddg       [mW ]: %lf\n"
		"power  avg   vddq       [mW ]: %lf\n"
		"power  avg   mic        [mW ]: %lf\n",
		mic_power_avg_pcie(m),
		mic_power_avg_c2x3(m),
		mic_power_avg_c2x4(m),
		mic_power_avg_vccp(m),
		mic_power_avg_vddg(m),
		mic_power_avg_vddq(m),
		mic_power_avg_power_usage(m)
	);
	
	fprintf(file,
		"temp   max   die        [\u00b0C ]: %u\n"
		"temp   max   gddr       [\u00b0C ]: %u\n"
		"temp   max   fan in     [\u00b0C ]: %u\n"
		"temp   max   fan out    [\u00b0C ]: %u\n"
		"temp   max   vccp       [\u00b0C ]: %u\n"
		"temp   max   vddg       [\u00b0C ]: %u\n"
		"temp   max   vddq       [\u00b0C ]: %u\n",
		mic_temp_max_die(m),
		mic_temp_max_gddr(m),
		mic_temp_max_fan_in(m),
		mic_temp_max_fan_out(m),
		mic_temp_max_vccp(m),
		mic_temp_max_vddg(m),
		mic_temp_max_vddq(m)
	);
	
	fprintf(file,
		"freq   avg   mic core   [MHz]: %.1lf\n"
		"freq   avg   mic mem    [MHz]: %.1lf\n",
		mic_freq_avg_core(m),
		mic_freq_avg_mem(m)
	);
	
	fprintf(file,
		"time   avg   mic active [ s ]: %.2lf\n"
		"time   avg   mic idle   [ s ]: %.2lf\n"
		"util   avg   mic        [ %% ]: %.2lf\n",
		mic_active_avg_all(m),
		mic_idle_avg_all(m),
		mic_util_avg_all(m)
	);
	
	fprintf(file,
		"memory total mic        [kiB]: %d\n"
		"memory max   mic used   [kiB]: %d\n"
		"memory max   mic free   [kiB]: %d\n",
		mic_memory_total(m),
		mic_memory_used_max(m),
		mic_memory_free_max(m)
	);
}

static void print_ostream_system(FILE *file, ARGUMENTS *settings, MS_LIST *m) {
	fprintf(file,
		"time total measure sys  [ s ]: %lf\n\n",
		system_time_total(m)
	);
	
	fprintf(file,
		"energy total board      [Ws ]: %lf\n"
		"energy total system     [Ws ]: %lf\n"
		"power  avg   board      [ W ]: %lf\n"
		"power  avg   system     [ W ]: %lf\n"
		"temp   max   system     [\u00b0C ]: %lf\n",
		system_energy_board(m),
		system_energy_total(m),
		system_power_board_avg(m),
		system_power_avg(m),
		system_temp_max(m)
	);
}

static void print_ostream_time(FILE *file, EXEC_TIME *exec_time) {
	fprintf(file,
		"real                    [ s ]: %lf\n"
		"user                    [ s ]: %lf\n"
		"sys                     [ s ]: %lf\n",
		 exec_time->real,
		 exec_time->user,
		 exec_time->sys
	);
}