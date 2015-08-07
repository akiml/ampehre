/*
 * CMeasureMaxeler.cpp
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
 * created: 3/05/14
 * version: 0.1.0 - initial implementation
 *          0.1.1 - add functionality to force FPGA to idle
 *          0.1.9 - add FPGA utilization measurements
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#include "CMeasureMaxeler.hpp"

#include "maxdsd.h"
#include "../../cjson/cJSON.h"

namespace NLibMeasure {
	CMeasureMaxeler::CMeasureMaxeler(CLogger& rLogger) :
		CMeasureAbstractResource(rLogger)
		{
		
		init();
	}
	
	CMeasureMaxeler::~CMeasureMaxeler() {
		destroy();
	}
	
	void CMeasureMaxeler::init(void) {
		mMaxDaemonFildes = max_daemon_connect("maxelerosd.sock");
		if (mMaxDaemonFildes < 0) {
			mrLog(CLogger::scErr) << "!!! 'maxeler' (thread main): cannot connect to maxeler os daemon. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mMaxPower[VCC10]		= std::make_pair("vcc1v0_core"   , 0.85);
		mMaxPower[VCC15]		= std::make_pair("vcc1v5_ddr"    , 0.85);
		mMaxPower[VCC25]		= std::make_pair("vcc2v5_aux"    , 0.85);
		mMaxPower[IMGT10]		= std::make_pair("imgt_1v0"      , 0.65);
		mMaxPower[IMGT12]		= std::make_pair("imgt_1v2"      , 0.65);
		mMaxPower[MGT10]		= std::make_pair("mgt_1v0"       , 0.65);
		mMaxPower[MGT12]		= std::make_pair("mgt_1v2"       , 0.65);
		mMaxPower[POWER]		= std::make_pair("power_usage"   , 1.0 );
		
		mMaxTemperature[MTEMP]	= std::make_pair("main_fpga_temp", 1.0 );
		mMaxTemperature[ITEMP]	= std::make_pair("ifce_fpga_temp", 1.0 );
		
		mrLog()
		<< ">>> 'maxeler' (thread main): initialization done." << std::endl
		<< std::endl;
	}
	
	void CMeasureMaxeler::destroy(void) {
		max_daemon_disconnect(mMaxDaemonFildes);
	}
	
	const std::string& CMeasureMaxeler::getPowerName(enum maxeler_power name) const {
		return mMaxPower[name].first;
	}
	
	const std::string& CMeasureMaxeler::getTemperatureName(enum maxeler_temperature name) const {
		return mMaxTemperature[name].first;
	}
	
	void CMeasureMaxeler::forceIdle (void) {
		mrLog()
		<< ">>> 'maxeler' (thread main): forcing fpga to idle." << std::endl
		<< std::endl;
		
		max_daemon_force_idle(mMaxDaemonFildes, 0);
	}
	
	void CMeasureMaxeler::measure(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		measurePower(pMeasurement, rThreadNum);
#ifndef LIGHT
		measureTemperature(pMeasurement, rThreadNum);
		measureUtilization(pMeasurement, rThreadNum);
#endif /* LIGHT */
	}
	
	void CMeasureMaxeler::measurePower(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		cJSON *json_response		= NULL;
		cJSON *json_measurements	= NULL;
		cJSON *json_single			= NULL;
		
		const char *jstr_request =
			"[{ \"request_type\": \"get_power_data\", \"args\": { \"acceptable_age\": \"0\" } }]";
		//	" { \"request_type\": \"get_computation_utilization_data\" },"
		//	" { \"request_type\": \"get_memory_utilization_data\" }]";
		
		char *jstr_response = NULL;
		
		int ret = max_daemon_exchange_json_string(mMaxDaemonFildes, &jstr_response, jstr_request);
		if (ret || NULL == jstr_response) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'maxeler thread' (thread #" << rThreadNum << "): Error: cannot obtain power measurement values. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		json_response = cJSON_Parse(jstr_response);
		
#if 0
		printf("%s\n", cJSON_Print(json_response));
#endif
		
		int success = cJSON_GetObjectItem(cJSON_GetArrayItem(json_response, 0), "success")->type;
		if (!success) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'maxeler thread' (thread #" << rThreadNum << "): Error: cannot obtain power measurement values. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		json_measurements = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetArrayItem(json_response, 0), "data"), "engine0");
		pMeasurement->maxeler_power_cur[POWER] = cJSON_GetObjectItem(json_measurements, mMaxPower[POWER].first.c_str())->valuedouble*1000;
		
		json_measurements = cJSON_GetObjectItem(json_measurements, "measurements");
		for (int i=0; i<cJSON_GetArraySize(json_measurements); ++i) {
			json_single = cJSON_GetArrayItem(json_measurements, i);
			if (strncmp(cJSON_GetObjectItem(json_single, "rail_name")->valuestring,
					mMaxPower[i].first.c_str(), mMaxPower[i].first.length()) == 0) {
				pMeasurement->maxeler_power_cur[i] = cJSON_GetObjectItem(json_single, "voltage")->valuedouble * cJSON_GetObjectItem(json_single, "current")->valuedouble*1000;
				pMeasurement->maxeler_power_cur[i] /= mMaxPower[i].second;
			} else {
				mrLog.lock();
				mrLog(CLogger::scErr) << "!!! 'maxeler thread' (thread #" << rThreadNum << "): Error: json string order changed. did you upgrade maxeler os? (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				mrLog.unlock();
				cJSON_Delete(json_response);
				exit(EXIT_FAILURE);
			}
		}
		
		cJSON_Delete(json_response);
	}
	
	void CMeasureMaxeler::measureTemperature(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		char *response;
		max_daemon_device_hw_monitor(mMaxDaemonFildes, 0, &response);
		
#if 0
		for (int i=0; i<strlen(response); ++i) if (response[i] == ',') response[i] = '\n';
		printf("%s\n", response);
		max_daemon_device_hw_monitor(fildes, 0, &response);
#endif
		
		char *mfpga	= strstr(response, "MAIN_FPGA_TEMPERATURE=");
		char *meq	= strchr(mfpga, '=');
		char *mend	= strchr(meq++, ',');

		char *ifpga	= strstr(response, "MAX3_IFCEFPGA_TEMPERATURE=");
		char *ieq	= strchr(ifpga, '=');
		char *iend	= strchr(ieq++, ',');
		
		*mend		= '\0';
		*iend		= '\0';
		
		sscanf(meq, "%lf", pMeasurement->maxeler_temperature_cur+MTEMP);
		sscanf(ieq, "%lf", pMeasurement->maxeler_temperature_cur+ITEMP);
	}
	
	void CMeasureMaxeler::measureUtilization(MEASUREMENT *pMeasurement, int32_t &rThreadNum) {
		cJSON *json_response		= NULL;
		cJSON *json_measurements	= NULL;
		
		const char *jstr_request =
			"[{ \"request_type\": \"get_computation_utilization_data\" }]";
		
		char *jstr_response = NULL;
		
		int ret = max_daemon_exchange_json_string(mMaxDaemonFildes, &jstr_response, jstr_request);
		if (ret || NULL == jstr_response) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'maxeler thread' (thread #" << rThreadNum << "): Error: cannot obtain power measurement values. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		json_response = cJSON_Parse(jstr_response);
		
#if 0
		printf("%s\n", cJSON_Print(json_response));
#endif
		
		int success = cJSON_GetObjectItem(cJSON_GetArrayItem(json_response, 0), "success")->type;
		if (!success) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'maxeler thread' (thread #" << rThreadNum << "): Error: cannot obtain power measurement values. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		json_measurements = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetArrayItem(json_response, 0), "data"), "engine0");
		
		pMeasurement->maxeler_util_comp_cur = cJSON_GetObjectItem(json_measurements, "usage")->valuedouble;
		
		cJSON_Delete(json_response);
	}
}
