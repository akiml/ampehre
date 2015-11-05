/*
 * mgmt.cpp
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
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.4.0 - MIC integration into libmeasure
 *          0.5.1 - modularised libmeasure
 *          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#include "../../include/measurement.h"
#include "CMgmt.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <signal.h>

#define UINT64_MAX (0xffffffffffffffff)

MSYSTEM *ms_init(MS_VERSION* version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max, gpu_frequency gpu_freq, uint64_t ipmi_timeout_setting) {
	
	if((version->major != MS_MAJOR_VERSION) || (version->minor != MS_MINOR_VERSION) || (version->revision != MS_REVISION_VERSION)){
		std::cout << "!!! 'mgmt' (thread main): Error: Wrong version number! libmeasure version " << MS_MAJOR_VERSION << "." << MS_MINOR_VERSION << "." << MS_REVISION_VERSION  << " is called from tool with version" << version->major << "." << version->minor << "." << version->revision  << " (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	CMgmt *mgmt = new CMgmt(cpu_gov, cpu_freq_min, cpu_freq_max, gpu_freq, ipmi_timeout_setting);
	
	return (MSYSTEM *)mgmt;
}

void ms_init_fpga_force_idle(MSYSTEM *mgmt) {
	CMgmt *ms = (CMgmt *)mgmt;
	
	ms->initMaxelerForceIdle();
}

void ms_fini(MSYSTEM *mgmt) {
	delete (CMgmt *)mgmt;
}

MEASUREMENT* ms_alloc_measurement(void) {
	MEASUREMENT *measurement = new MEASUREMENT;
	
	memset(measurement, 0, sizeof(MEASUREMENT));
	
	measurement->msr_time_wait.tv_sec		= UINT64_MAX;
	measurement->msr_time_wait.tv_nsec		= UINT64_MAX;
	measurement->nvml_time_wait.tv_sec		= UINT64_MAX;
	measurement->nvml_time_wait.tv_nsec		= UINT64_MAX;
	measurement->maxeler_time_wait.tv_sec	= UINT64_MAX;
	measurement->maxeler_time_wait.tv_nsec	= UINT64_MAX;
	measurement->ipmi_time_wait.tv_sec		= UINT64_MAX;
	measurement->ipmi_time_wait.tv_nsec		= UINT64_MAX;
	measurement->mic_time_wait.tv_sec		= UINT64_MAX;
	measurement->mic_time_wait.tv_nsec		= UINT64_MAX;
	
	measurement->msr_skip_ms_rate		= 1;
	measurement->nvml_skip_ms_rate		= 1;
	measurement->maxeler_skip_ms_rate	= 1;
	measurement->ipmi_skip_ms_rate		= 1;
	measurement->mic_skip_ms_rate		= 1;
	
	return measurement;
}

void ms_free_measurement(MEASUREMENT* measurement) {
	delete measurement;
}

void ms_set_timer(MEASUREMENT* measurement, int flag, uint64_t sec, uint64_t nsec, uint32_t skip_ms_rate) {
	if(skip_ms_rate <= 0){
		skip_ms_rate = 1;
	}
	
	switch (flag) {
		case CPU:
			measurement->msr_time_wait.tv_sec		= sec;
			measurement->msr_time_wait.tv_nsec		= nsec;
			measurement->msr_skip_ms_rate			= skip_ms_rate;
			break;
		case GPU:
			measurement->nvml_time_wait.tv_sec		= sec;
			measurement->nvml_time_wait.tv_nsec		= nsec;
			measurement->nvml_skip_ms_rate			= skip_ms_rate;
			break;
		case FPGA:
			measurement->maxeler_time_wait.tv_sec	= sec;
			measurement->maxeler_time_wait.tv_nsec	= nsec;
			measurement->maxeler_skip_ms_rate		= skip_ms_rate;
			break;
		case SYSTEM:
			measurement->ipmi_time_wait.tv_sec		= sec;
			measurement->ipmi_time_wait.tv_nsec		= nsec;
			measurement->ipmi_skip_ms_rate			= skip_ms_rate;
			break;
		case MIC:
			measurement->mic_time_wait.tv_sec		= sec;
			measurement->mic_time_wait.tv_nsec		= nsec;
			measurement->mic_skip_ms_rate			= skip_ms_rate;
			break;
		default:
			std::cout << "!!! 'mgmt' (thread main): Error: cannot set measurement timer. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
	}
}

void ms_init_measurement(MSYSTEM *msystem, MEASUREMENT* measurement, int flags) {
	CMgmt *ms = (CMgmt *)msystem;
	
	if (flags & CPU) {
		ms->initMeasureThread(CPU, measurement);
	}
	
	if (flags & GPU) {
		ms->initMeasureThread(GPU, measurement);
	}
	
	if (flags & FPGA) {
		ms->initMeasureThread(FPGA, measurement);
	}
	
	if (flags & SYSTEM) {
		ms->initMeasureThread(SYSTEM, measurement);
	}
	
	if (flags & MIC) {
		ms->initMeasureThread(MIC, measurement);
	}
}

void ms_fini_measurement(MSYSTEM *msystem, MEASUREMENT* measurement) {
	CMgmt *ms = (CMgmt *)msystem;
	
	for(int i = 0; i < (log2(ALL+1)); i++) {
		ms->finiMeasureThread(1<<i);
	}
}

void ms_start_measurement(MSYSTEM *msystem, MEASUREMENT* measurement) {
	CMgmt *ms = (CMgmt *)msystem;
	
	// start threads
	for(int i = 0; i < (log2(ALL+1)); i++) {
		ms->lockResourceMutexStart(1<<i);
		ms->startMeasureThread(1<<i);
	}
	
	// synchronize threads by mutex locks
	for(int i = 0; i < (log2(ALL+1)); i++) {
		ms->lockResourceMutexStart(1<<i);
	}
	
	ms->postStartSem(2*log2(ALL+1));
	
#ifdef SIGNALS
	if (kill(getpid(), SIGUSR1)) {
		std::cout << "!!! 'mgmt' (thread main): Warning: Couldn't raise signal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
	}
#endif /* SIGNALS */
	
	// unlock after signal raise
	for(int i = 0; i < (log2(ALL+1)); i++) {
		ms->unlockResourceMutexStart(1<<i);
	}
}

void ms_stop_measurement(MSYSTEM *msystem, MEASUREMENT* measurement) {
	CMgmt *ms = (CMgmt *)msystem;
	
#ifdef SIGNALS
	if (kill(getpid(), SIGUSR2)) {
		std::cout << "!!! 'mgmt' (thread main): Warning: Couldn't raise signal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
	}
#endif /* SIGNALS */
	
	for(int i = 0; i < (log2(ALL+1)); i++) {
		ms->stopMeasureThread(1<<i);
	}
}

void ms_join_measurement(MSYSTEM *msystem, MEASUREMENT* measurement) {
	CMgmt *ms = (CMgmt *)msystem;
	
	for(int i = 0; i < (log2(ALL+1)); i++) {
		ms->joinMeasureThread(1<<i);
	}
}

void ms_reg_sighandler_start(MSYSTEM *mgmt, void(*signal_handler)(int)) {
#ifdef SIGNALS
	CMgmt *ms = (CMgmt *)mgmt;
	
	ms->regSighandlerStart(signal_handler);
#else /* NOT(SIGNALS) */
	std::cout << "!!! 'mgmt' (thread main): Warning: You attempt to add signal handlers to a libmeasure compiled without this feature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
#endif /* SIGNALS */
}

void ms_reg_sighandler_stop(MSYSTEM *mgmt, void(*signal_handler)(int)) {
#ifdef SIGNALS
	CMgmt *ms = (CMgmt *)mgmt;
	
	ms->regSighandlerStart(signal_handler);
#else /* NOT(SIGNALS) */
	std::cout << "!!! 'mgmt' (thread main): Warning: You attempt to add signal handlers to a libmeasure compiled without this feature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
#endif /* SIGNALS */
}
