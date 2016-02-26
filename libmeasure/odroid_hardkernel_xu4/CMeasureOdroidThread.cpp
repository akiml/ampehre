/*
 * CMeasureOdroidThread.cpp
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 2/25/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#include "CMeasureOdroidThread.hpp"

namespace NLibMeasure {
	CMeasureOdroidThread::CMeasureOdroidThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes):
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		
		mThreadType = "odroid";
		mTimer.setThreadName("odroid timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_ODROID *)pMsMeasurement)->odroid_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	CMeasureOdroidThread::~CMeasureOdroidThread() {
		// nothing todo
	}
	
	void CMeasureOdroidThread::run(void) {
		MS_MEASUREMENT_ODROID *pMsMeasurementOdroid = (MS_MEASUREMENT_ODROID *) mpMsMeasurement;
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'odroid thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     sampling rate: " << mTimer.getTimerHertz() << " Hz / "
				<< mTimer.getTimerMillisecond() << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		uint32_t i;
		
		// initialize some variables
		pMsMeasurementOdroid->odroid_time_runtime			= 0.0;
		for (i=0; i<ODROID_NUM_POWER; ++i) {
			pMsMeasurementOdroid->odroid_energy_acc[i]		= 0.0;
			pMsMeasurementOdroid->odroid_power_avg[i]		= 0.0;
		}
		for (i=0; i<ODROID_NUM_TEMP; ++i) {
			pMsMeasurementOdroid->odroid_temperature_max[i]	= 0;
		}
		for (i=0; i<ODROID_NUM_FREQ; ++i) {
			pMsMeasurementOdroid->odroid_clock_acc[i]		= 0.0;
			pMsMeasurementOdroid->odroid_clock_avg[i]		= 0.0;
		}
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(pMsMeasurementOdroid->internal.odroid_time_cur));
		pMsMeasurementOdroid->internal.odroid_time_temp.tv_sec	= pMsMeasurementOdroid->internal.odroid_time_cur.tv_sec;
		pMsMeasurementOdroid->internal.odroid_time_temp.tv_nsec	= pMsMeasurementOdroid->internal.odroid_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			// Retrieve data from sensors
			mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
			
			// Calculate time span since last iteration
			calcTimeDiff(&(pMsMeasurementOdroid->internal.odroid_time_cur), &(pMsMeasurementOdroid->internal.odroid_time_temp), &(pMsMeasurementOdroid->internal.odroid_time_diff), &(pMsMeasurementOdroid->internal.odroid_time_diff_double));
			
			// Result: runtime
			pMsMeasurementOdroid->odroid_time_runtime			+= pMsMeasurementOdroid->internal.odroid_time_diff_double;
			
			// result: energy consumption
			for (i=0; i<ODROID_NUM_POWER; ++i) {
				pMsMeasurementOdroid->odroid_energy_acc[i]		+=
					pMsMeasurementOdroid->odroid_power_cur[i] * pMsMeasurementOdroid->internal.odroid_time_diff_double;
			}
			
			// result: maximum temperature
			for (i=0; i<ODROID_NUM_TEMP; ++i) {
				pMsMeasurementOdroid->odroid_temperature_max[i]	=
					(pMsMeasurementOdroid->odroid_temperature_cur[i] > pMsMeasurementOdroid->odroid_temperature_max[i]) ?
					pMsMeasurementOdroid->odroid_temperature_cur[i] : pMsMeasurementOdroid->odroid_temperature_max[i];
			}
			
			// result: "accumulated frequencies"
			for (i=0; i<ODROID_NUM_FREQ; ++i) {
				pMsMeasurementOdroid->odroid_clock_acc[i]		+=
					pMsMeasurementOdroid->odroid_clock_cur[i] * pMsMeasurementOdroid->internal.odroid_time_diff_double;
			}
			
			// result: average clock speeds
			for (i=0; i<ODROID_NUM_FREQ; ++i) {
				pMsMeasurementOdroid->odroid_clock_avg[i]		=
					(pMsMeasurementOdroid->odroid_clock_acc[i])/pMsMeasurementOdroid->odroid_time_runtime;
			}
		}
		
		// result: average power consumption
		for (i=0; i<ODROID_NUM_POWER; ++i) {
			pMsMeasurementOdroid->odroid_power_avg[i] 				=
				(pMsMeasurementOdroid->odroid_energy_acc[i])/pMsMeasurementOdroid->odroid_time_runtime;
		}
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'odroid thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period             : " << pMsMeasurementOdroid->odroid_time_runtime << " s" << std::endl;
		for (i=0; i<ODROID_NUM_POWER; ++i) {
			mrLog()
			<< "     consumed energy device " << i << ": " << pMsMeasurementOdroid->odroid_energy_acc[i] << " mWs" << std::endl;
		}
		for (i=0; i<ODROID_NUM_POWER; ++i) {
			mrLog()
			<< "     average power   device " << i << ": " << pMsMeasurementOdroid->odroid_power_avg[i] << " mW" << std::endl;
		}
		for (i=0; i<ODROID_NUM_TEMP; ++i) {
			mrLog()
			<< "     max temperature device " << i << ": " << pMsMeasurementOdroid->odroid_temperature_max[i] << " \u00b0C" << std::endl;
		}
		for (i=0; i<ODROID_NUM_FREQ; ++i) {
			mrLog()
			<< "     avg frequency   device " << i << ": " << pMsMeasurementOdroid->odroid_clock_avg[i] << " MHz" << std::endl;
		}
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}
