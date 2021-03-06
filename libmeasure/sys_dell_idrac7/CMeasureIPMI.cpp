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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularized measurement struct
 *          0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

namespace NLibMeasure {
	template <int TSkipMs, int TVariant>
	CMeasureIPMI<TSkipMs, TVariant>::CMeasureIPMI(CLogger& rLogger, ipmi_timeout_setting timeout_setting) :
		CMeasureAbstractResource(rLogger),
		mTimeoutSetting(timeout_setting)
		{
		init();
	}
	
	template <int TSkipMs, int TVariant>
	CMeasureIPMI<TSkipMs, TVariant>::~CMeasureIPMI() {
		destroy();
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::init(void) {
		if (TVariant == VARIANT_FULL) {
			mrLog()
			<< ">>> 'ipmi' (full version)" << std::endl;
		} else {
			mrLog()
			<< ">>> 'ipmi' (light version)" << std::endl;
		}
		
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
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::destroy(void) {
		close_ipmi_wrapper();
	}
	
	template <int TSkipMs, int TVariant>
	int CMeasureIPMI<TSkipMs, TVariant>::getVariant() {
		return TVariant;
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::resetEnergyCounter(int32_t& rThreadNum) {
		measureRawMsgDellResetEnergy(rThreadNum);
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::measure(void* pMsMeasurement, int32_t& rThreadNum) {
		MS_MEASUREMENT_SYS *pMsMeasurementSys = (MS_MEASUREMENT_SYS *) pMsMeasurement;
		measureRecordIDs(pMsMeasurementSys, rThreadNum);
		measureRawMsgs(pMsMeasurementSys, rThreadNum);
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::measureRecordIDs(MS_MEASUREMENT_SYS *pMsMeasurementSys, int32_t& rThreadNum) {
		double value = 0;
		
		if (TVariant == VARIANT_FULL) {
			if(!(mMeasureCounter % TSkipMs)) {
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
					pMsMeasurementSys->ipmi_temperature_sysboard_cur = value;
				}
			}
		}
		
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
			pMsMeasurementSys->ipmi_power_sysboard_cur = value;
		}
		
		if (TVariant == VARIANT_FULL) {
			if(!(mMeasureCounter++ % TSkipMs)) {
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
					pMsMeasurementSys->ipmi_temperature_cur[0] = value;
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
					pMsMeasurementSys->ipmi_temperature_cur[1] = value;
				}
			}
		}
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::measureRawMsgs(MS_MEASUREMENT_SYS *pMsMeasurementSys, int32_t& rThreadNum) {
		measureRawMsgDellCumulativeEnergy(pMsMeasurementSys, rThreadNum);
		measureRawMsgDellCurrentPower(pMsMeasurementSys, rThreadNum);
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::measureRawMsgDellResetEnergy(int32_t& rThreadNum) {
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
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::measureRawMsgDellCumulativeEnergy(MS_MEASUREMENT_SYS *pMsMeasurementSys, int32_t& rThreadNum) {
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
		
		pMsMeasurementSys->ipmi_energy_server_acc_since_reset	= result_energy;
		pMsMeasurementSys->ipmi_power_server_avg_since_reset		= result_power;
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::measureRawMsgDellCurrentPower(MS_MEASUREMENT_SYS *pMsMeasurementSys, int32_t& rThreadNum) {	
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
		
		pMsMeasurementSys->ipmi_power_server_cur = (uint32_t)result_power_current;
	}
	
	template <int TSkipMs, int TVariant>
    void CMeasureIPMI<TSkipMs, TVariant>::setIPMITimeout(uint32_t& timeout, uint32_t& rThreadNum) {
		int rv;
		uint64_t timeout_setting;
		if(mTimeoutSetting == IPMI_SET_AND_LOCK_TIMEOUT) {
			timeout_setting = IOC_SET_AND_LOCK_IPMI_TIMEOUT;
		} else {
			timeout_setting =  IOC_SET_IPMI_TIMEOUT;
		}
		rv = setIPMITimeoutIOCTL(timeout_setting, timeout);
		
		if(rv == ERROR_IPMI_TIMEOUT_LOCKED){
			mrLog.lock();
			mrLog() << ">>> 'ipmi thread' (thread #" << rThreadNum << "): Warning: IPMI timeout is locked." << std::endl;
			mrLog.unlock();
		} else if(rv == ERROR_IPMI_TIMEOUT_MAX){
			mrLog.lock();
			mrLog() << ">>> 'ipmi thread' (thread #" << rThreadNum << "): Warning: IPMI timeout is too large or 0. Set timeout to default." << std::endl;
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
		mrLog()<<  ">>> 'ipmi thread' (thread #" << rThreadNum << "): Current IPMI timeout: " << rv << " ms" << std::endl;
		mrLog.unlock();
	}
	
	template <int TSkipMs, int TVariant>
	void CMeasureIPMI<TSkipMs, TVariant>::trigger_resource_custom(void* pParams) {
		uint32_t* pArgs = (uint32_t*) pParams;
		setIPMITimeout(pArgs[0], pArgs[1]);
	}
}
