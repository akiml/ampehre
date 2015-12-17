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
 */

namespace NLibMeasure {
	template <int TVariant>
	CMeasureIPMIThread<TVariant>::CMeasureIPMIThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMeasurement, rMeasureRes)
		{
		mThreadType = "ipmi";
		mTimer.setThreadName("ipmi timer");
		mTimer.setTimer(&(pMeasurement->ipmi_time_wait));
		mTimer.shareMutex(&mMutexTimer);
		
	}
	
	template <int TVariant>
	CMeasureIPMIThread<TVariant>::~CMeasureIPMIThread() {
		// nothing todo
	}
	
	template <int TVariant>
	void CMeasureIPMIThread<TVariant>::run(void) {
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'ipmi thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     sampling rate: " << mTimer.getTimerHertz() / mpMeasurement->ipmi_check_for_exit_interrupts << " Hz / "
				<< mTimer.getTimerMillisecond() * mpMeasurement->ipmi_check_for_exit_interrupts << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		mpMeasurement->maxeler_time_runtime				= 0.0;
		mpMeasurement->ipmi_temperature_sysboard_max	= 0.0;
		mpMeasurement->ipmi_energy_sysboard_acc			= 0.0;
		for (int i=0; i<CPUS; ++i) {
			mpMeasurement->ipmi_temperature_max[i]		= 0.0;
		}
		
		mpMeasurement->ipmi_energy_server_acc			= 0;
		
		//static_cast<NLibMeasure::CMeasureIPMI&>(mrMeasureResource).setIPMITimeout((mpMeasurement->ipmi_time_wait.tv_nsec/1000000) + (mpMeasurement->ipmi_time_wait.tv_sec * 1000) - 10, mThreadNum);
		uint32_t params[2];
		params[0] = ((mpMeasurement->ipmi_time_wait.tv_nsec/1000000) + (mpMeasurement->ipmi_time_wait.tv_sec * 1000)) * mpMeasurement->ipmi_check_for_exit_interrupts - 10;
		params[1] = mThreadNum;
		
		mrMeasureResource.trigger_resource_custom((void*)params);
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(mpMeasurement->internal.ipmi_time_cur));
		mpMeasurement->internal.ipmi_time_temp.tv_sec	= mpMeasurement->internal.ipmi_time_cur.tv_sec;
		mpMeasurement->internal.ipmi_time_temp.tv_nsec	= mpMeasurement->internal.ipmi_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(TVariant == VARIANT_FULL) {
				if(!(skip_ms_cnt++ % mpMeasurement->ipmi_check_for_exit_interrupts)){
					mrMeasureResource.measure(mpMeasurement, mThreadNum);
				}
				
				// calculated diff time (use internal struct for that)
				calcTimeDiff(&(mpMeasurement->internal.ipmi_time_cur), &(mpMeasurement->internal.ipmi_time_temp), &(mpMeasurement->internal.ipmi_time_diff), &(mpMeasurement->internal.ipmi_time_diff_double));
				
				// result: runtime
				mpMeasurement->ipmi_time_runtime += mpMeasurement->internal.ipmi_time_diff_double;
				
				// result: energy consumption
				mpMeasurement->ipmi_energy_sysboard_acc	+=
					mpMeasurement->ipmi_power_sysboard_cur * mpMeasurement->internal.ipmi_time_diff_double;
				mpMeasurement->ipmi_energy_server_acc	+=
					mpMeasurement->ipmi_power_server_cur * mpMeasurement->internal.ipmi_time_diff_double;
				
				// result: maximum temperatures
				mpMeasurement->ipmi_temperature_sysboard_max =
					(mpMeasurement->ipmi_temperature_sysboard_cur > mpMeasurement->ipmi_temperature_sysboard_max) ?
					mpMeasurement->ipmi_temperature_sysboard_cur : mpMeasurement->ipmi_temperature_sysboard_max;
				for (int i=0; i<CPUS; ++i) {
					mpMeasurement->ipmi_temperature_max[i] =
						(mpMeasurement->ipmi_temperature_cur[i] > mpMeasurement->ipmi_temperature_max[i]) ?
						mpMeasurement->ipmi_temperature_cur[i] : mpMeasurement->ipmi_temperature_max[i];
				}
			}
			
#if 0
			mrLog.lock();
			mrLog() << "t_board: " << mpMeasurement->ipmi_temperature_sysboard_cur << ", t_cpu0: " << mpMeasurement->ipmi_temperature_cur[0] << ", t_cpu1: " << mpMeasurement->ipmi_temperature_cur[1] << std::endl;
			mrLog() << "p_board: " << mpMeasurement->ipmi_power_sysboard_cur << ", p_server: " << mpMeasurement->ipmi_power_server_cur << std::endl;
			mrLog() << "e_server_since_reset: " << mpMeasurement->ipmi_energy_server_acc_since_reset << std::endl;
			mrLog.unlock();
#endif
		}
		
		if(TVariant == VARIANT_FULL) {
			// result: average power consumption
			mpMeasurement->ipmi_power_sysboard_avg	= mpMeasurement->ipmi_energy_sysboard_acc/mpMeasurement->ipmi_time_runtime;
			mpMeasurement->ipmi_power_server_avg	= mpMeasurement->ipmi_energy_server_acc/mpMeasurement->ipmi_time_runtime;
		}
		
#ifdef DEBUG
		mrLog.lock();
		mrLog()
		<< "ooo 'ipmi thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period              : " << mpMeasurement->ipmi_time_runtime << " s" << std::endl
		<< "     sys board energy accu.   : " << mpMeasurement->ipmi_energy_sysboard_acc << " Ws" << std::endl
		<< "     sys board power avg      : " << mpMeasurement->ipmi_power_sysboard_avg << " W" << std::endl
		<< "     server energy accumulated: " << mpMeasurement->ipmi_energy_server_acc << " Ws" << std::endl
		<< "     server power average     : " << mpMeasurement->ipmi_power_server_avg << " W" << std::endl
		<< "     server energy counter acc: " << mpMeasurement->ipmi_energy_server_acc_since_reset << " Wh" << std::endl
		<< "     server power counter avg : " << mpMeasurement->ipmi_power_server_avg_since_reset << " W" << std::endl
		<< "     sys board temperature max: " << mpMeasurement->ipmi_temperature_sysboard_max << " \u00b0C" << std::endl;
		for (int i=0; i<CPUS; ++i) {
			mrLog()
			<< "     cpu " << i << " temperature max    : " << mpMeasurement->ipmi_temperature_max[i] << " \u00b0C" << std::endl;
		}
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
};
