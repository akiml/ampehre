/*
 * CMeasureMaxelerThread.hpp
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
 * created: 3/06/14
 * version: 0.1.0 - initial implementation
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.9 - add FPGA utilization measurements
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#include "CMeasureMaxelerThread.hpp"

#include <iomanip>

namespace NLibMeasure {
	CMeasureMaxelerThread::CMeasureMaxelerThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMeasurement, rMeasureRes)
		{
		mThreadType = "maxeler";
		mTimer.setThreadName("maxeler timer");
		mTimer.setTimer(&(pMeasurement->maxeler_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	CMeasureMaxelerThread::~CMeasureMaxelerThread() {
		// nothing todo
	}
	
	void CMeasureMaxelerThread::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'maxeler thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     effective sampling rate: " << mTimer.getTimerHertz() / mpMeasurement->maxeler_skip_ms_rate << " Hz / "
				<< mTimer.getTimerMillisecond() * mpMeasurement->maxeler_skip_ms_rate << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		mpMeasurement->maxeler_time_runtime		= 0.0;
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			mpMeasurement->maxeler_energy_acc[i]	= 0.0;
		}
		for (int i=0; i<MAX_NUM_TEMPERATURE; ++i) {
			mpMeasurement->maxeler_temperature_max[i]	= 0.0;
		}
		mpMeasurement->maxeler_util_comp_acc	= 0.0;
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(mpMeasurement->internal.maxeler_time_cur));
		mpMeasurement->internal.maxeler_time_temp.tv_sec	= mpMeasurement->internal.maxeler_time_cur.tv_sec;
		mpMeasurement->internal.maxeler_time_temp.tv_nsec	= mpMeasurement->internal.maxeler_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(!(skip_ms_cnt % mpMeasurement->maxeler_skip_ms_rate)){
				mrMeasureResource.measure(mpMeasurement, mThreadNum);
			}
			
			if(skip_ms_cnt == UINT64_MAX){
				skip_ms_cnt = 0;
			} else {
				skip_ms_cnt++;
			}
			
			// calculated diff time
			calcTimeDiff(&(mpMeasurement->internal.maxeler_time_cur), &(mpMeasurement->internal.maxeler_time_temp), &(mpMeasurement->internal.maxeler_time_diff), &(mpMeasurement->internal.maxeler_time_diff_double));
			
			// result: runtime
			mpMeasurement->maxeler_time_runtime += mpMeasurement->internal.maxeler_time_diff_double;
			
			// result: energy consumption
			for (int i=0; i<MAX_NUM_POWER; ++i) {
				mpMeasurement->maxeler_energy_acc[i] += mpMeasurement->maxeler_power_cur[i] * mpMeasurement->internal.maxeler_time_diff_double;
			}
#ifndef LIGHT
			// result: maximum temperatures
			for (int i=0; i<MAX_NUM_TEMPERATURE; ++i) {
				if (mpMeasurement->maxeler_temperature_cur[i] > mpMeasurement->maxeler_temperature_max[i]) {
					mpMeasurement->maxeler_temperature_max[i] = mpMeasurement->maxeler_temperature_cur[i];
				}
			}
			
			// result: utlization
			mpMeasurement->maxeler_util_comp_acc	+= mpMeasurement->maxeler_util_comp_cur * mpMeasurement->internal.maxeler_time_diff_double;
#endif /* LIGHT */
			
#if 0
			for (int i=0; i<8; ++i)
			mrLog()<< "m: " << mpMeasurement->maxeler_power_cur[i] << " mW\n";
			for (int i=0; i<2; ++i)
			mrLog()<< "t: " << mpMeasurement->maxeler_temperature_cur[i] << " \u00b0C\n";
			mrLog()<< "\n";
#endif
		}
		
		// result: average power consumption
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			mpMeasurement->maxeler_power_avg[i] = (mpMeasurement->maxeler_energy_acc[i]) / mpMeasurement->maxeler_time_runtime;
		}
		
#ifndef LIGHT
		mpMeasurement->maxeler_util_comp_avg = mpMeasurement->maxeler_util_comp_acc / mpMeasurement->maxeler_time_runtime;
#endif /* LIGHT */
		
#ifdef DEBUG
		 const char* const powerName[] = {"vcc1v0_core", "vcc1v5_ddr", "vcc2v5_aux", "imgt_1v0", "imgt_1v2", "mgt_1v0", "mgt_1v2", "power_usage"};
		 const char* const tempName[] = {"main_fpga_temp", "ifce_fpga_temp"}; 

		mrLog.lock();
		mrLog()
		<< "ooo 'maxeler thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period                     : " << mpMeasurement->maxeler_time_runtime << " s" << std::endl;
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			mrLog()
			<< "     consumed energy (" << powerName[i] << "): "
				<< mpMeasurement->maxeler_energy_acc[i] << " mWs" << std::endl;
		}
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			mrLog()
			<< "     average power   (" << powerName[i] << "): "
				<< mpMeasurement->maxeler_power_avg[i] << " mW" << std::endl;
		}
		for (int i=0; i<MAX_NUM_TEMPERATURE; ++i) {
			mrLog()
			<< "     temperature max (" << tempName[i] << "): "
				<< mpMeasurement->maxeler_temperature_max[i] << " \u00b0C" << std::endl;
		}
		mrLog()
		<< "     utilization average             : " << mpMeasurement->maxeler_util_comp_avg << " %%" << std::endl;
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}