/*
 * CMgmt.cpp
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
 *          0.1.1 - add functionality to force FPGA to idle
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.4 - add dynamic loading of resource specific libraries
 *          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 */

#include "CMgmt.hpp"

typedef void (*trigger_resource_custom_t)(void*);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
void emptySighandler(int signal) {
        // nothing todo
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

CMgmt::CMgmt(cpu_governor cpuGovernor, uint64_t cpuFrequencyMin, uint64_t cpuFrequencyMax, gpu_frequency gpuFrequency) :
	mLogger(),
	mResources(),
	mStartSem()
	{
	uint64_t params_cpu[] = {cpuGovernor, cpuFrequencyMin, cpuFrequencyMax};
	uint64_t params_gpu[] = {gpuFrequency};
	
	mResources[CPU] 	= new NLibMeasure::CResourceLibraryHandler(mLogger, CPU_LIB_NAME, params_cpu);
	mResources[GPU] 	= new NLibMeasure::CResourceLibraryHandler(mLogger, GPU_LIB_NAME, params_gpu);
	mResources[FPGA] 	= new NLibMeasure::CResourceLibraryHandler(mLogger, FPGA_LIB_NAME, NULL);
	mResources[SYSTEM]	= new NLibMeasure::CResourceLibraryHandler(mLogger, SYS_LIB_NAME, NULL);
	mResources[MIC] 	= new NLibMeasure::CResourceLibraryHandler(mLogger, MIC_LIB_NAME, NULL);
	
	mpActionStart	= NULL;
	mpActionStop	= NULL;
	
#ifdef SIGNALS
	regSighandlerStart(&emptySighandler);
	regSighandlerStop(&emptySighandler);
#endif /* SIGNALS */
}

CMgmt::~CMgmt() {
	BOOST_FOREACH(map_type::value_type &i, mResources)
	{
		delete i.second;
	}
	
	
#ifndef LIGHT
#ifdef SIGNALS
	if (mpActionStart) {
		sigaction(SIGUSR1, 0, 0);
		delete mpActionStart;
	}
	if (mpActionStop) {
		sigaction(SIGUSR2, 0, 0);
		delete mpActionStop;
	}
#endif /* SIGNALS */
#endif /* LIGHT */

}

void CMgmt::initMaxelerForceIdle (void) {
	trigger_resource_custom_t maxeler_force_idle = (trigger_resource_custom_t)mResources[FPGA]->loadFunction("trigger_resource_custom");
	
	maxeler_force_idle(mResources[FPGA]->getResource());
}

void CMgmt::regSighandlerStart(void(*signal_handler)(int)) {
	struct sigaction *temp	= new struct sigaction;
	temp->sa_sigaction		= 0;
	temp->sa_handler		= signal_handler;
	temp->sa_flags			= 0;
	temp->sa_restorer		= 0;
	sigemptyset(&temp->sa_mask);
	
	if (sigaction(SIGUSR1, temp, 0) == 0) {
		if (mpActionStart) {
			delete mpActionStart;
		}
		mpActionStart = temp;
		
		mLogger.lock();
		mLogger()
		<< ">>> 'mgmt' (thread main): Signal handler for SIGUSR1 installed (measuring system start)." << std::endl
		<< std::endl;
		mLogger.unlock();
	} else {
		mLogger.lock();
		mLogger()
		<< "!!! 'mgmt' (thread main): Warning: Couldn't install signal handler for SIGUSR1' (measuring system start) (file: " << __FILE__ << ", line: " << __LINE__ << ")." << std::endl
		<< std::endl;
		mLogger.unlock();
	}
}

void CMgmt::regSighandlerStop(void(*signal_handler)(int)) {
	struct sigaction *temp	= new struct sigaction;
	temp->sa_sigaction		= 0;
	temp->sa_handler		= signal_handler;
	temp->sa_flags			= 0;
	temp->sa_restorer		= 0;
	sigemptyset(&temp->sa_mask);
	
	if (sigaction(SIGUSR2, temp, 0) == 0) {
		if (mpActionStop) {
			delete mpActionStop;
		}
		mpActionStop = temp;
		
		mLogger.lock();
		mLogger()
		<< ">>> 'mgmt' (thread main): Signal handler for SIGUSR2 installed (measuring system stop)." << std::endl
		<< std::endl;
		mLogger.unlock();
	} else {
		mLogger.lock();
		mLogger()
		<< "!!! 'mgmt' (thread main): Warning: Couldn't install signal handler for SIGUSR2 (measuring system stop) (file: " << __FILE__ << ", line: " << __LINE__ << ")." << std::endl
		<< std::endl;
		mLogger.unlock();
	}
}

void CMgmt::lockResourceMutexStart(int res) {
	if(mResources[res]->getResourceThread()){
		mResources[res]->getMutexStartResource().lock();
	}
}

void CMgmt::unlockResourceMutexStart(int res) {
	if(mResources[res]->getResourceThread()){
		mResources[res]->getMutexStartResource().unlock();
	}
}

void CMgmt::postStartSem(int count){
	mStartSem.post(count);
}

void CMgmt::initMeasureThread(int res, MEASUREMENT* pMeasurement) {
	mResources[res]->initResourceThread(mStartSem, pMeasurement);
}

void CMgmt::finiMeasureThread(int res) {
	if(mResources[res]->getResourceThread()){
		mResources[res]->finiResourceThread();
	}
}

void CMgmt::startMeasureThread(int res) {
	if(mResources[res]->getResourceThread()){
		mResources[res]->getResourceThread()->start(&(mResources[res]->getMutexStartResource()));
	}
}

void CMgmt::stopMeasureThread(int res) {
	if(mResources[res]->getResourceThread()){
		mResources[res]->getResourceThread()->stop();
	}
}

void CMgmt::joinMeasureThread(int res) {
	if(mResources[res]->getResourceThread()){
		mResources[res]->getResourceThread()->join();
	}
}