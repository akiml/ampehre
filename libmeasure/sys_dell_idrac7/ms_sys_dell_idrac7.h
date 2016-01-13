/*
 * ms_sys_dell_idrac7.h
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

#ifndef __MS_SYS_DELL_IDRAC7_H__
#define __MS_SYS_DELL_IDRAC7_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ms_list.h"

// Only for library internal usage!
typedef struct __measurement_internal_sys {
	struct timespec ipmi_time_cur;
	struct timespec ipmi_time_temp;
	struct timespec ipmi_time_diff;
	double ipmi_time_diff_double;
} MEASUREMENT_INTERNAL_SYS;

typedef struct __ms_measurement_sys {
	// onyl for internal usage
	MEASUREMENT_INTERNAL_SYS internal;
	
	struct timespec ipmi_time_wait;
	uint32_t ipmi_check_for_exit_interrupts;
	
	double ipmi_time_runtime;
	
	double ipmi_temperature_sysboard_cur;
	double ipmi_temperature_sysboard_max;
	double ipmi_power_sysboard_cur;
	double ipmi_power_sysboard_avg;
	double ipmi_energy_sysboard_acc;
	double ipmi_temperature_cur[CPUS];
	double ipmi_temperature_max[CPUS];
	
	uint32_t ipmi_power_server_cur;
	double ipmi_power_server_avg;
	uint32_t ipmi_energy_server_acc;
	
	uint32_t ipmi_energy_server_acc_since_reset;
	double ipmi_power_server_avg_since_reset;
} MS_MEASUREMENT_SYS;

inline double system_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_time_runtime;
}

inline double system_energy_board(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_energy_sysboard_acc;
}

inline double system_power_board_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_power_sysboard_avg;
}

inline double system_temp_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_temperature_sysboard_max;
}

inline double system_energy_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_energy_server_acc;
}

inline double system_power_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_power_server_avg;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_SYS_DELL_IDRAC7_H__ */