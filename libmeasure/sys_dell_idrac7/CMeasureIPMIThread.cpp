/*
 * CMeasureIPMIThread.cpp
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
 * created: 3/20/14
 * version: 0.1.0 - initial implementation
 *          0.1.4 - add signals for measuring system start and stop
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularized measurement struct
 */

namespace NLibMeasure {
	template <int TVariant>
	CMeasureIPMIThread<TVariant>::CMeasureIPMIThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		mThreadType = "ipmi";
		mTimer.setThreadName("ipmi timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_SYS *)pMsMeasurement)->ipmi_time_wait));
		mTimer.shareMutex(&mMutexTimer);
		
	}
	
	template <int TVariant>
	CMeasureIPMIThread<TVariant>::~CMeasureIPMIThread() {
		// nothing todo
	}
	
	template <int TVariant>
	void CMeasureIPMIThread<TVariant>::run(void) {
		MS_MEASUREMENT_SYS *pMsMeasurementSys = (MS_MEASUREMENT_SYS *) mpMsMeasurement;
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'ipmi thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     sampling rate: " << mTimer.getTimerHertz() / pMsMeasurementSys->ipmi_check_for_exit_interrupts << " Hz / "
				<< mTimer.getTimerMillisecond() * pMsMeasurementSys->ipmi_check_for_exit_interrupts << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		pMsMeasurementSys->ipmi_time_runtime				= 0.0;
		pMsMeasurementSys->ipmi_temperature_sysboard_max	= 0.0;
		pMsMeasurementSys->ipmi_energy_sysboard_acc			= 0.0;
		for (int i=0; i<CPUS; ++i) {
			pMsMeasurementSys->ipmi_temperature_max[i]		= 0.0;
		}
		
		pMsMeasurementSys->ipmi_energy_server_acc			= 0;
		
		//static_cast<NLibMeasure::CMeasureIPMI&>(mrMeasureResource).setIPMITimeout((pMsMeasurementSys->ipmi_time_wait.tv_nsec/1000000) + (pMsMeasurementSys->ipmi_time_wait.tv_sec * 1000) - 10, mThreadNum);
		uint32_t params[2];
		params[0] = ((pMsMeasurementSys->ipmi_time_wait.tv_nsec/1000000) + (pMsMeasurementSys->ipmi_time_wait.tv_sec * 1000)) * pMsMeasurementSys->ipmi_check_for_exit_interrupts - 10;
		params[1] = mThreadNum;
		
		mrMeasureResource.trigger_resource_custom((void*)params);
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(pMsMeasurementSys->internal.ipmi_time_cur));
		pMsMeasurementSys->internal.ipmi_time_temp.tv_sec	= pMsMeasurementSys->internal.ipmi_time_cur.tv_sec;
		pMsMeasurementSys->internal.ipmi_time_temp.tv_nsec	= pMsMeasurementSys->internal.ipmi_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(TVariant == VARIANT_FULL) {
				if(!(skip_ms_cnt++ % pMsMeasurementSys->ipmi_check_for_exit_interrupts)){
					mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
				}
				
				// calculated diff time (use internal struct for that)
				calcTimeDiff(&(pMsMeasurementSys->internal.ipmi_time_cur), &(pMsMeasurementSys->internal.ipmi_time_temp), &(pMsMeasurementSys->internal.ipmi_time_diff), &(pMsMeasurementSys->internal.ipmi_time_diff_double));
				
				// result: runtime
				pMsMeasurementSys->ipmi_time_runtime += pMsMeasurementSys->internal.ipmi_time_diff_double;
				
				// result: energy consumption
				pMsMeasurementSys->ipmi_energy_sysboard_acc	+=
					pMsMeasurementSys->ipmi_power_sysboard_cur * pMsMeasurementSys->internal.ipmi_time_diff_double;
				pMsMeasurementSys->ipmi_energy_server_acc	+=
					pMsMeasurementSys->ipmi_power_server_cur * pMsMeasurementSys->internal.ipmi_time_diff_double;
				
				// result: maximum temperatures
				pMsMeasurementSys->ipmi_temperature_sysboard_max =
					(pMsMeasurementSys->ipmi_temperature_sysboard_cur > pMsMeasurementSys->ipmi_temperature_sysboard_max) ?
					pMsMeasurementSys->ipmi_temperature_sysboard_cur : pMsMeasurementSys->ipmi_temperature_sysboard_max;
				for (int i=0; i<CPUS; ++i) {
					pMsMeasurementSys->ipmi_temperature_max[i] =
						(pMsMeasurementSys->ipmi_temperature_cur[i] > pMsMeasurementSys->ipmi_temperature_max[i]) ?
						pMsMeasurementSys->ipmi_temperature_cur[i] : pMsMeasurementSys->ipmi_temperature_max[i];
				}
			}
			
#if 0
			mrLog.lock();
			mrLog() << "t_board: " << pMsMeasurementSys->ipmi_temperature_sysboard_cur << ", t_cpu0: " << pMsMeasurementSys->ipmi_temperature_cur[0] << ", t_cpu1: " << pMsMeasurementSys->ipmi_temperature_cur[1] << std::endl;
			mrLog() << "p_board: " << pMsMeasurementSys->ipmi_power_sysboard_cur << ", p_server: " << pMsMeasurementSys->ipmi_power_server_cur << std::endl;
			mrLog() << "e_server_since_reset: " << pMsMeasurementSys->ipmi_energy_server_acc_since_reset << std::endl;
			mrLog.unlock();
#endif
		}
		
		if(TVariant == VARIANT_FULL) {
			// result: average power consumption
			pMsMeasurementSys->ipmi_power_sysboard_avg	= pMsMeasurementSys->ipmi_energy_sysboard_acc/pMsMeasurementSys->ipmi_time_runtime;
			pMsMeasurementSys->ipmi_power_server_avg	= pMsMeasurementSys->ipmi_energy_server_acc/pMsMeasurementSys->ipmi_time_runtime;
		}
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'ipmi thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period              : " << pMsMeasurementSys->ipmi_time_runtime << " s" << std::endl
		<< "     sys board energy accu.   : " << pMsMeasurementSys->ipmi_energy_sysboard_acc << " Ws" << std::endl
		<< "     sys board power avg      : " << pMsMeasurementSys->ipmi_power_sysboard_avg << " W" << std::endl
		<< "     server energy accumulated: " << pMsMeasurementSys->ipmi_energy_server_acc << " Ws" << std::endl
		<< "     server power average     : " << pMsMeasurementSys->ipmi_power_server_avg << " W" << std::endl
		<< "     server energy counter acc: " << pMsMeasurementSys->ipmi_energy_server_acc_since_reset << " Wh" << std::endl
		<< "     server power counter avg : " << pMsMeasurementSys->ipmi_power_server_avg_since_reset << " W" << std::endl
		<< "     sys board temperature max: " << pMsMeasurementSys->ipmi_temperature_sysboard_max << " \u00b0C" << std::endl;
		for (int i=0; i<CPUS; ++i) {
			mrLog()
			<< "     cpu " << i << " temperature max    : " << pMsMeasurementSys->ipmi_temperature_max[i] << " \u00b0C" << std::endl;
		}
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
};
