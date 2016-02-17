/*
 * ms_fpga_maxeler_max3a.h
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
 * created: 12/10/15
 * version: 0.7.0 - modularized measurement struct
 *          0.7.1 - move functions to query measurement results into the modules
 *          0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 *          0.7.4 - add query for currently active processes to libmeasure and show them in msmonitor
 */

#ifndef __MS_FPGA_MAXELER_MAX3A_H__
#define __MS_FPGA_MAXELER_MAX3A_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <time.h>

#include "ms_list.h"
#include "ms_driver.h"

#define MAX_NUM_POWER 8
#define MAX_NUM_TEMPERATURE 2
#define MAXELER_BUFFER_SIZE 128

enum maxeler_power {
	VCC10,
	VCC15,
	VCC25,
	IMGT10,
	IMGT12,
	MGT10,
	MGT12,
	POWER
};

enum maxeler_temperature {
	MTEMP,
	ITEMP
};

// Only for library internal usage!
typedef struct __measurement_internal_fpga {
	struct timespec maxeler_time_cur;
	struct timespec maxeler_time_temp;
	struct timespec maxeler_time_diff;
	double maxeler_time_diff_double;
} MEASUREMENT_INTERNAL_FPGA;

typedef struct __ms_measurement_fpga {
	// onyl for internal usage
	MEASUREMENT_INTERNAL_FPGA internal;
	
	struct timespec maxeler_time_wait;
	uint32_t maxeler_check_for_exit_interrupts;
	
	double maxeler_time_runtime;
	
	double maxeler_power_cur[MAX_NUM_POWER];
	double maxeler_power_avg[MAX_NUM_POWER];
	double maxeler_energy_acc[MAX_NUM_POWER];
	
	double maxeler_temperature_cur[MAX_NUM_TEMPERATURE];
	double maxeler_temperature_max[MAX_NUM_TEMPERATURE];
	
	double maxeler_util_comp_cur;
	double maxeler_util_comp_acc;
	double maxeler_util_comp_avg;
	
	uint32_t maxeler_active_processes_pid;
	char maxeler_active_process_name[MAXELER_BUFFER_SIZE];
	char maxeler_active_process_user[MAXELER_BUFFER_SIZE];
} MS_MEASUREMENT_FPGA;

double fpga_time_total(MS_LIST *ms_list);
double fpga_energy_total_vcc1v0_core(MS_LIST *ms_list);
double fpga_energy_total_vcc1v5_ddr(MS_LIST *ms_list);
double fpga_energy_total_vcc2v5_aux(MS_LIST *ms_list);
double fpga_energy_total_imgt_1v0(MS_LIST *ms_list);
double fpga_energy_total_imgt_1v2(MS_LIST *ms_list);
double fpga_energy_total_mgt_1v0(MS_LIST *ms_list);
double fpga_energy_total_mgt_1v2(MS_LIST *ms_list);
double fpga_energy_total_power_usage(MS_LIST *ms_list);
double fpga_power_avg_vcc1v0_core(MS_LIST *ms_list);
double fpga_power_avg_vcc1v5_ddr(MS_LIST *ms_list);
double fpga_power_avg_vcc2v5_aux(MS_LIST *ms_list);
double fpga_power_avg_imgt_1v0(MS_LIST *ms_list);
double fpga_power_avg_imgt_1v2(MS_LIST *ms_list);
double fpga_power_avg_mgt_1v0(MS_LIST *ms_list);
double fpga_power_avg_mgt_1v2(MS_LIST *ms_list);
double fpga_power_avg_power_usage(MS_LIST *ms_list);
double fpga_temp_max_ifpga(MS_LIST *ms_list);
double fpga_temp_max_mfpga(MS_LIST *ms_list);
double fpga_util_avg_comp(MS_LIST *ms_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_FPGA_MAXELER_MAX3A_H__ */
