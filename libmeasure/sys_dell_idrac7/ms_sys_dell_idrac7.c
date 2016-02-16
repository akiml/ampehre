/*
 * ms_sys_dell_idrac7.c
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
 * author: Achim Lösch
 * created: 01/25/16
 * version: 0.7.2 - add real, user and sys time to hettime plus bugfixing result query functions
 */

#include "ms_sys_dell_idrac7.h"

double system_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_time_runtime;
}

double system_energy_board(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_energy_sysboard_acc;
}

double system_power_board_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_power_sysboard_avg;
}

double system_temp_max(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_temperature_sysboard_max;
}

double system_energy_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_energy_server_acc;
}

double system_power_avg(MS_LIST *ms_list) {
	MS_MEASUREMENT_SYS *ms_measurement_sys = (MS_MEASUREMENT_SYS *) getMeasurement(&ms_list, SYSTEM);
	NULL_CHECK(ms_measurement_sys);
	
	return ms_measurement_sys->ipmi_power_server_avg;
}
