/*
 * CMgmt.hpp
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
 * created: 3/31/14
 * version: 0.1.0 - initial implementation
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.1 - modularised libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.4 - add dynamic loading of resource specific libraries
 *          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.7.0 - modularised measurement struct
 */

#ifndef __CMGMT_HPP__
#define __CMGMT_HPP__

#include <map>
#include <signal.h>
#include <dlfcn.h>
#include <math.h>
#include <boost/foreach.hpp>
#include "CLogger.hpp"
#include "CSemaphore.hpp"
#include "CMeasureAbstractResource.hpp"
#include "CMeasureAbstractThread.hpp"
#include "CResourceLibraryHandler.hpp"
#include "../../include/ms_measurement.h"
#include "CMutex.hpp"

typedef std::map<int , NLibMeasure::CResourceLibraryHandler*> map_type;

class CMgmt {
	private:
		NLibMeasure::CLogger mLogger;
		
		map_type mResources;
		
		NLibMeasure::CSemaphore mStartSem;
		
		struct sigaction *mpActionStart;
		struct sigaction *mpActionStop;
		
		lib_variant mLibVariant;
		
	public:
		CMgmt(cpu_governor cpuGovernor, uint64_t cpuFrequencyMin, uint64_t cpuFrequencyMax, gpu_frequency gpuFrequency, uint64_t ipmi_timeout_setting, skip_ms_rate skip_ms_rate,  lib_variant variant);
		~CMgmt();
		
		void initMaxelerForceIdle(void);
		
		void regSighandlerStart(void(*signal_handler)(int));
		void regSighandlerStop(void(*signal_handler)(int));
		
		void lockResourceMutexStart(int res);
		void unlockResourceMutexStart(int res);
		void postStartSem(int count);
		void initMeasureThread(int res, MS_LIST* pMsList);
		void finiMeasureThread(int res);
		void startMeasureThread(int res);
		void stopMeasureThread(int res);
		void joinMeasureThread(int res);
};

#endif /* __CMGMT_HPP__ */
