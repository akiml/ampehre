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
 */

#ifndef __MS_SYS_DELL_IDRAC7_H__
#define __MS_SYS_DELL_IDRAC7_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_SYS_DELL_IDRAC7_H__ */