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
 */

#ifndef __MS_FPGA_MAXELER_MAX3A_H__
#define __MS_FPGA_MAXELER_MAX3A_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ms_list.h"

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

inline double fpga_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_time_runtime;
}

inline double fpga_energy_total_vcc1v0_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC10];
}

inline double fpga_energy_total_vcc1v5_ddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC15];
}

inline double fpga_energy_total_vcc2v5_aux(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC25];
}

inline double fpga_energy_total_imgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[IMGT10];
}

inline double fpga_energy_total_imgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[IMGT12];
}

inline double fpga_energy_total_mgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[MGT10];
}

inline double fpga_energy_total_mgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[MGT12];
}

inline double fpga_energy_total_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[POWER];
}

inline double fpga_power_avg_vcc1v0_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC10];
}

inline double fpga_power_avg_vcc1v5_ddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC15];
}

inline double fpga_power_avg_vcc2v5_aux(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC25];
}

inline double fpga_power_avg_imgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[IMGT10];
}

inline double fpga_power_avg_imgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[IMGT12];
}

inline double fpga_power_avg_mgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[MGT10];
}

inline double fpga_power_avg_mgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[MGT12];
}

inline double fpga_power_avg_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[POWER];
}

inline double fpga_temp_max_ifpga(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_temperature_max[ITEMP];
}

inline double fpga_temp_max_mfpga(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_temperature_max[MTEMP];
}

inline double fpga_util_avg_comp(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_util_comp_avg;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_FPGA_MAXELER_MAX3A_H__ */