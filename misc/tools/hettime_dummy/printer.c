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
 * created: 10/13/14
 * version: 0.1.17 - add a hettime dummy executable for some hetsched measurements
 *          0.1.18 - hettime dummy tool can be used similar to ordinary "sleep" command
 */

#include <stdio.h>

#include "hettime.h"

static void print_ostream_cpu(FILE *file, ARGUMENTS *settings, MEASUREMENT *m);
static void print_ostream_gpu(FILE *file, ARGUMENTS *settings, MEASUREMENT *m);
static void print_ostream_fpga(FILE *file, ARGUMENTS *settings, MEASUREMENT *m);

void print_ostream(FILE *file, ARGUMENTS *settings, MEASUREMENT *m) {
	fprintf(file,
		"RESULTS:\n========\n"
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
}

static void print_ostream_cpu(FILE *file, ARGUMENTS *settings, MEASUREMENT *m) {
	fprintf(file,
		"energy total cpu 0 pkg  [mWs]: %lf\n"
		"energy total cpu 0 dram [mWs]: %lf\n"
		"power  avg   cpu 0 pkg  [mW ]: %lf\n"
		"power  avg   cpu 0 dram [mW ]: %lf\n",
		cpu_energy_total_pkg(m, 0),
		cpu_energy_total_dram(m, 0),
		cpu_power_avg_pkg(m, 0),
		cpu_power_avg_dram(m, 0)
	);
	
	fprintf(file,
		"energy total cpu 1 pkg  [mWs]: %lf\n"
		"energy total cpu 1 dram [mWs]: %lf\n"
		"power  avg   cpu 1 pkg  [mW ]: %lf\n"
		"power  avg   cpu 1 dram [mW ]: %lf\n",
		cpu_energy_total_pkg(m, 1),
		cpu_energy_total_dram(m, 1),
		cpu_power_avg_pkg(m, 1),
		cpu_power_avg_dram(m, 1)
	);
}

static void print_ostream_gpu(FILE *file, ARGUMENTS *settings, MEASUREMENT *m) {
	fprintf(file,
		"energy total gpu        [mWs]: %lf\n"
		"power  avg   gpu        [mW ]: %lf\n",
		gpu_energy_total(m),
		gpu_power_avg(m)
	);
}

static void print_ostream_fpga(FILE *file, ARGUMENTS *settings, MEASUREMENT *m) {
	fprintf(file,
		"energy total fpga       [mWs]: %lf\n"
		"power  avg   fpga       [mW ]: %lf\n",
		fpga_energy_total_power_usage(m),
		fpga_power_avg_power_usage(m)
	);
	
}
