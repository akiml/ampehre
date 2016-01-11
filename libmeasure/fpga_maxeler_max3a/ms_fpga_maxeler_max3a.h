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
 */

#ifndef __MS_FPGA_MAXELER_MAX3A_H__
#define __MS_FPGA_MAXELER_MAX3A_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_NUM_POWER 8
#define MAX_NUM_TEMPERATURE 2

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
} MS_MEASUREMENT_FPGA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_FPGA_MAXELER_MAX3A_H__ */