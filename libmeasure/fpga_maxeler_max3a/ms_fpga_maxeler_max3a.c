/*
 * ms_fpga_maxeler_max3a.c
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

#include "ms_fpga_maxeler_max3a.h"

double fpga_time_total(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_time_runtime;
}

double fpga_energy_total_vcc1v0_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC10];
}

double fpga_energy_total_vcc1v5_ddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC15];
}

double fpga_energy_total_vcc2v5_aux(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[VCC25];
}

double fpga_energy_total_imgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[IMGT10];
}

double fpga_energy_total_imgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[IMGT12];
}

double fpga_energy_total_mgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[MGT10];
}

double fpga_energy_total_mgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[MGT12];
}

double fpga_energy_total_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_energy_acc[POWER];
}

double fpga_power_avg_vcc1v0_core(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC10];
}

double fpga_power_avg_vcc1v5_ddr(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC15];
}

double fpga_power_avg_vcc2v5_aux(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[VCC25];
}

double fpga_power_avg_imgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[IMGT10];
}

double fpga_power_avg_imgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[IMGT12];
}

double fpga_power_avg_mgt_1v0(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[MGT10];
}

double fpga_power_avg_mgt_1v2(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[MGT12];
}

double fpga_power_avg_power_usage(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_power_avg[POWER];
}

double fpga_temp_max_ifpga(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_temperature_max[ITEMP];
}

double fpga_temp_max_mfpga(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_temperature_max[MTEMP];
}

double fpga_util_avg_comp(MS_LIST *ms_list) {
	MS_MEASUREMENT_FPGA *ms_measurement_fpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&ms_list, FPGA);
	NULL_CHECK(ms_measurement_fpga);
	
	return ms_measurement_fpga->maxeler_util_comp_avg;
}
