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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularised measurement struct
 */

namespace NLibMeasure {
	template <int TVariant>
	CMeasureMaxelerThread<TVariant>::CMeasureMaxelerThread(CLogger& rLogger, CSemaphore& rStartSem, void *pMsMeasurement, CMeasureAbstractResource& rMeasureRes) :
		CMeasureAbstractThread(rLogger, rStartSem, pMsMeasurement, rMeasureRes)
		{
		mThreadType = "maxeler";
		mTimer.setThreadName("maxeler timer");
		mTimer.setTimer(&(((MS_MEASUREMENT_FPGA *)pMsMeasurement)->maxeler_time_wait));
		mTimer.shareMutex(&mMutexTimer);
	}
	
	template <int TVariant>
	CMeasureMaxelerThread<TVariant>::~CMeasureMaxelerThread() {
		// nothing todo
	}
	
	template <int TVariant>
	void CMeasureMaxelerThread<TVariant>::run(void) {
		MS_MEASUREMENT_FPGA *pMsMeasurementFpga = (MS_MEASUREMENT_FPGA *) mpMsMeasurement;
		mThreadStateRun		= true;
		mThreadStateStop	= false;
		
		uint64_t skip_ms_cnt = 0;
		
		mrLog.lock();
		mThreadNum = CThread::sNumOfThreads++;
		mrLog() << ">>> 'maxeler thread' (thread #" << mThreadNum << "): init" << std::endl
				<< "     sampling rate: " << mTimer.getTimerHertz() / pMsMeasurementFpga->maxeler_check_for_exit_interrupts << " Hz / "
				<< mTimer.getTimerMillisecond() * pMsMeasurementFpga->maxeler_check_for_exit_interrupts << " ms" << std::endl;
		mrLog.unlock();
		
		mMutexTimer.lock();
		
		// initialize some values
		pMsMeasurementFpga->maxeler_time_runtime		= 0.0;
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			pMsMeasurementFpga->maxeler_energy_acc[i]	= 0.0;
		}
		for (int i=0; i<MAX_NUM_TEMPERATURE; ++i) {
			pMsMeasurementFpga->maxeler_temperature_max[i]	= 0.0;
		}
		pMsMeasurementFpga->maxeler_util_comp_acc	= 0.0;
		
		mpMutexStart->unlock();
		
		mrStartSem.wait();
		
		// first initial measurement (no energy consumption calculated, store current time stamp)
		mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
		clock_gettime(CLOCK_REALTIME, &(pMsMeasurementFpga->internal.maxeler_time_cur));
		pMsMeasurementFpga->internal.maxeler_time_temp.tv_sec	= pMsMeasurementFpga->internal.maxeler_time_cur.tv_sec;
		pMsMeasurementFpga->internal.maxeler_time_temp.tv_nsec	= pMsMeasurementFpga->internal.maxeler_time_cur.tv_nsec;
		
		while (!mThreadStateStop) {
			mMutexTimer.lock();
			
			if(!(skip_ms_cnt++ % pMsMeasurementFpga->maxeler_check_for_exit_interrupts)){
				mrMeasureResource.measure(mpMsMeasurement, mThreadNum);
			}
			
			// calculated diff time
			calcTimeDiff(&(pMsMeasurementFpga->internal.maxeler_time_cur), &(pMsMeasurementFpga->internal.maxeler_time_temp), &(pMsMeasurementFpga->internal.maxeler_time_diff), &(pMsMeasurementFpga->internal.maxeler_time_diff_double));
			
			// result: runtime
			pMsMeasurementFpga->maxeler_time_runtime += pMsMeasurementFpga->internal.maxeler_time_diff_double;
			
			// result: energy consumption
			for (int i=0; i<MAX_NUM_POWER; ++i) {
				pMsMeasurementFpga->maxeler_energy_acc[i] += pMsMeasurementFpga->maxeler_power_cur[i] * pMsMeasurementFpga->internal.maxeler_time_diff_double;
			}
			if(TVariant == VARIANT_FULL) {
				// result: maximum temperatures
				for (int i=0; i<MAX_NUM_TEMPERATURE; ++i) {
					if (pMsMeasurementFpga->maxeler_temperature_cur[i] > pMsMeasurementFpga->maxeler_temperature_max[i]) {
						pMsMeasurementFpga->maxeler_temperature_max[i] = pMsMeasurementFpga->maxeler_temperature_cur[i];
					}
				}
				
				// result: utlization
				pMsMeasurementFpga->maxeler_util_comp_acc	+= pMsMeasurementFpga->maxeler_util_comp_cur * pMsMeasurementFpga->internal.maxeler_time_diff_double;
			}
			
#if 0
			for (int i=0; i<8; ++i)
			mrLog()<< "m: " << pMsMeasurementFpga->maxeler_power_cur[i] << " mW\n";
			for (int i=0; i<2; ++i)
			mrLog()<< "t: " << pMsMeasurementFpga->maxeler_temperature_cur[i] << " \u00b0C\n";
			mrLog()<< "\n";
#endif
		}
		
		// result: average power consumption
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			pMsMeasurementFpga->maxeler_power_avg[i] = (pMsMeasurementFpga->maxeler_energy_acc[i]) / pMsMeasurementFpga->maxeler_time_runtime;
		}
		
		if(TVariant == VARIANT_FULL) {
			pMsMeasurementFpga->maxeler_util_comp_avg = pMsMeasurementFpga->maxeler_util_comp_acc / pMsMeasurementFpga->maxeler_time_runtime;
		}
		
#ifdef DEBUG
		 const char* const powerName[] = {"vcc1v0_core", "vcc1v5_ddr", "vcc2v5_aux", "imgt_1v0", "imgt_1v2", "mgt_1v0", "mgt_1v2", "power_usage"};
		 const char* const tempName[] = {"main_fpga_temp", "ifce_fpga_temp"}; 

		mrLog.lock();
		mrLog()
		<< "ooo 'maxeler thread' (thread #" << mThreadNum << "):" << std::endl
		<< "     time period                     : " << pMsMeasurementFpga->maxeler_time_runtime << " s" << std::endl;
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			mrLog()
			<< "     consumed energy (" << powerName[i] << "): "
				<< pMsMeasurementFpga->maxeler_energy_acc[i] << " mWs" << std::endl;
		}
		for (int i=0; i<MAX_NUM_POWER; ++i) {
			mrLog()
			<< "     average power   (" << powerName[i] << "): "
				<< pMsMeasurementFpga->maxeler_power_avg[i] << " mW" << std::endl;
		}
		for (int i=0; i<MAX_NUM_TEMPERATURE; ++i) {
			mrLog()
			<< "     temperature max (" << tempName[i] << "): "
				<< pMsMeasurementFpga->maxeler_temperature_max[i] << " \u00b0C" << std::endl;
		}
		mrLog()
		<< "     utilization average             : " << pMsMeasurementFpga->maxeler_util_comp_avg << " %%" << std::endl;
		mrLog.unlock();
#endif /* DEBUG */
		
		exit();
	}
}