/*
 * CMeasureIPMI.cpp
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
 * created: 3/08/14
 * version: 0.1.0 - initial implementation
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl call to driver to configure the ipmi timeout
 */

#include "CMeasureIPMI.hpp"

#include <ctime>

namespace NLibMeasure {
	CMeasureIPMI::CMeasureIPMI(CLogger& rLogger, uint64_t ipmi_timeout_setting) :
		CMeasureAbstractResource(rLogger),
		mTimeoutSetting(ipmi_timeout_setting)
		{
		init();
	}
	
	CMeasureIPMI::~CMeasureIPMI() {
		destroy();
	}
	
	void CMeasureIPMI::init(void) {
#ifdef LIGHT
		mrLog()
		<< ">>> 'ipmi' (light version)" << std::endl;
#else
		mrLog()
		<< ">>> 'ipmi' (full version)" << std::endl;
#endif
		
		MS_VERSION version;
		version.major = MS_MAJOR_VERSION;
		version.minor = MS_MINOR_VERSION;
		version.revision = MS_REVISION_VERSION;
		init_ipmi_wrapper(&version);
		
		mrLog() << ">>> 'ipmi' (thread main): reset server energy counter." << std::endl;
		int32_t pseudo_thread_num = -1;
		resetEnergyCounter(pseudo_thread_num);
		
		for(int i=0; i<5; i++){		
			getTemperature(18);
			getPower(90);
			getTemperature(153);
			getTemperature(154);
		}
		
		mrLog()
		<< ">>> 'ipmi' (thread main): initialization done." << std::endl
		<< std::endl;
	}
	
	void CMeasureIPMI::destroy(void) {
		close_ipmi_wrapper();
	}
	
	void CMeasureIPMI::resetEnergyCounter(int32_t& rThreadNum) {
		measureRawMsgDellResetEnergy(rThreadNum);
	}
	
	void CMeasureIPMI::measure(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		measureRecordIDs(pMeasurement, rThreadNum);
		measureRawMsgs(pMeasurement, rThreadNum);
	}
	
	void CMeasureIPMI::measureRecordIDs(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		double value = 0;
		
#ifndef LIGHT
		value = getTemperature(18);
		if(value < 0 && value != -ETIMEDOUT){
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: in getTemperature record id 18 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		if(value == -ETIMEDOUT) {
			mrLog.lock();
			mrLog() << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Warning: ipmi request timeout in getTemperature record id 18 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
		} else {
			pMeasurement->ipmi_temperature_sysboard_cur = value;
		}
#endif /* LIGHT */
		
		value = getPower(90);
		if(value < 0 && value != -ETIMEDOUT){
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: in getPower record id 90 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		if(value == -ETIMEDOUT) {
			mrLog.lock();
			mrLog() << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Warning: ipmi request timeout getPower record id 90 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
		} else {
			pMeasurement->ipmi_power_sysboard_cur = value;
		}
		
#ifndef LIGHT
		value = getTemperature(153);
		if(value < 0 && value != -ETIMEDOUT){
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: in getTemperature record id 153 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		if(value == -ETIMEDOUT) {
			mrLog.lock();
			mrLog() << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Warning: ipmi request timeout in getTemperature record id 153 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
		} else {
			pMeasurement->ipmi_temperature_cur[0] = value;
		}
		
		value = getTemperature(154);
		if(value < 0 && value != -ETIMEDOUT){
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: in getTemperature record id 154 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		if(value == -ETIMEDOUT) {
			mrLog.lock();
			mrLog() << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Warning: ipmi request timeout in getTemperature record id 154 (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
		} else {
			pMeasurement->ipmi_temperature_cur[1] = value;
		}
#endif /* LIGHT */
	}
	
	void CMeasureIPMI::measureRawMsgs(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		measureRawMsgDellCumulativeEnergy(pMeasurement, rThreadNum);
		measureRawMsgDellCurrentPower(pMeasurement, rThreadNum);
	}
	
	void CMeasureIPMI::measureRawMsgDellResetEnergy(int32_t& rThreadNum) {
		int32_t completion_code;
		
		completion_code = dellResetEnergyCounter();
		
		if (completion_code) {
			mrLog.lock();
			if (rThreadNum >= 0) {
				mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: cannot reset energy counter completion code: " << std::hex << completion_code << ") (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			} else {
				mrLog(CLogger::scErr) << "!!! 'ipmi' (thread main):  Error: cannot reset energy counter completion code: " << std::hex << completion_code << ") (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			}
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
	}
	
	void CMeasureIPMI::measureRawMsgDellCumulativeEnergy(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		int32_t completion_code;
		
		uint32_t result_energy;
		uint32_t result_energy_start_time;
		uint32_t result_energy_cur_time;
		uint32_t result_energy_diff_time;
		
		double result_power;
		
		completion_code = dellCumulativeEnergy(&result_energy_start_time, &result_energy);
		
		result_energy_cur_time	= (uint32_t)time(NULL);
		
		if(completion_code == -ETIMEDOUT) {
			mrLog.lock();
			mrLog() << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Warning: ipmi request timeout in measureRawMsgDellCumulativeEnergy (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			return;
		}
		if (completion_code) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: cannot get cumulative energy, completion code: " << std::hex << completion_code << ") (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		result_energy_diff_time	= result_energy_cur_time - result_energy_start_time;
		result_power			= (double)(result_energy * 3600) / (double)result_energy_diff_time;
		
		pMeasurement->ipmi_energy_server_acc_since_reset	= result_energy;
		pMeasurement->ipmi_power_server_avg_since_reset		= result_power;
	}
	
	void CMeasureIPMI::measureRawMsgDellCurrentPower(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {	
		int32_t completion_code;		
		uint16_t result_power_current;
		
		completion_code = dellCurrentPower(&result_power_current);
		
		if(completion_code == -ETIMEDOUT) {
			mrLog.lock();
			mrLog() << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Warning: ipmi request timeout in measureRawMsgDellCurrentPower (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			return;
		}
		if (completion_code) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'ipmi thread' (thread #" << rThreadNum << "): Error: cannot get current power, completion code: " << std::hex << completion_code << ") (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		pMeasurement->ipmi_power_server_cur = (uint32_t)result_power_current;
	}
	
    void CMeasureIPMI::setIPMITimeout(uint32_t timeout, int32_t& rThreadNum) {
		int rv;
		rv = setIPMITimeoutIOCTL(mTimeoutSetting, timeout);
		
		if(rv == ERROR_IPMI_TIMEOUT_LOCKED){
			mrLog.lock();
			mrLog() << ">>> 'ipmi thread' (thread #" << rThreadNum << "): Warning: IPMI timeout is locked." << std::endl;
			mrLog.unlock();
		} else if(rv == ERROR_IPMI_TIMEOUT_MAX){
			mrLog.lock();
			mrLog() << ">>> 'ipmi thread' (thread #" << rThreadNum << "): Warning: IPMI timeout is too large. Set timeout to default." << std::endl;
			mrLog.unlock();
		} else if(rv){
			mrLog.lock();
			mrLog(CLogger::scErr) << ">>> 'ipmi thread' (thread #" << rThreadNum << "): Error: couldn't set IPMI timeout." << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		rv = getIPMITimeout();
		if(rv < 0){
			mrLog.lock();
			mrLog(CLogger::scErr) << ">>> 'ipmi thread' (thread #" << rThreadNum << "): Error: in getIPMITImeout, (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		mrLog.lock();
		mrLog()<<  ">>> 'ipmi thread' (thread #" << rThreadNum << "): Current IPMI timeout: " << rv << " ms." << std::endl;
		mrLog.unlock();
	}
}
