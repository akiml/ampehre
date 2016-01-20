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
 *          0.5.1 - modularized libmeasure
 *          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library. 
 *          0.7.0 - modularized measurement struct
 */

#include "../../include/ms_measurement.h"
#include "CMgmt.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <signal.h>

static void divide_sampling_rates(struct timespec *time_wait, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts);

MS_SYSTEM *ms_init(MS_VERSION* version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
				 gpu_frequency gpu_freq, uint64_t ipmi_timeout_setting, enum skip_ms_rate skip_ms_rate, enum lib_variant variant) {
	
	if((version->major != MS_MAJOR_VERSION) || (version->minor != MS_MINOR_VERSION) || (version->revision != MS_REVISION_VERSION)){
		std::cout << "!!! 'mgmt' (thread main): Error: Wrong version number! libmeasure version " << MS_MAJOR_VERSION << "." << MS_MINOR_VERSION << "." << MS_REVISION_VERSION  << " is called from tool with version" << version->major << "." << version->minor << "." << version->revision  << " (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	CMgmt *mgmt = new CMgmt(cpu_gov, cpu_freq_min, cpu_freq_max, gpu_freq, ipmi_timeout_setting, skip_ms_rate, variant);
	
	MS_SYSTEM *ms_system = new MS_SYSTEM;
	
	ms_system->mgmt = (void*) mgmt;
	
	ms_system->config = new MS_CONFIG;
	
#ifdef CPU_LIB
	ms_system->config->cpu_enabled = 1;
#else
	ms_system->config->cpu_enabled = 0;
#endif
	
#ifdef GPU_LIB
	ms_system->config->gpu_enabled = 1;
#else
	ms_system->config->gpu_enabled = 0;
#endif
	
#ifdef FPGA_LIB
	ms_system->config->fpga_enabled = 1;
#else
	ms_system->config->fpga_enabled = 0;
#endif

#ifdef SYS_LIB
	ms_system->config->sys_enabled = 1;
#else
	ms_system->config->sys_enabled = 0;
#endif

#ifdef MIC_LIB
	ms_system->config->mic_enabled = 1;
#else
	ms_system->config->mic_enabled = 0;
#endif
	
	return ms_system;
}

void ms_init_fpga_force_idle(MS_SYSTEM *ms_system) {
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	mgmt->initMaxelerForceIdle();
}

void ms_fini(MS_SYSTEM *ms_system) {
	delete ms_system->config;
	delete (CMgmt *)ms_system->mgmt;
	delete ms_system;
}

MS_LIST *ms_alloc_measurement(MS_SYSTEM *ms_system) {
	MS_LIST *ms_list = NULL;
	MS_CONFIG *config = ms_system->config;
	
	if(config->cpu_enabled) {
		appendList(&ms_list, CPU);
	}
	
	if(config->gpu_enabled) {
		appendList(&ms_list, GPU);
	}
	
	if(config->fpga_enabled) {
		appendList(&ms_list, FPGA);
	}
	
	if(config->sys_enabled) {
		appendList(&ms_list, SYSTEM);
	}
	
	if(config->mic_enabled) {
		appendList(&ms_list, MIC);
	}
	
	return ms_list;
}

void ms_free_measurement(MS_LIST *ms_list) {
	deleteList(&ms_list);
}

void ms_set_timer(MS_LIST *ms_list, int flag, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts) {
	if(check_for_exit_interrupts <= 0){
		check_for_exit_interrupts = 1;
	}
	void* ms_measurement = NULL;
	
	switch(flag) {
		case CPU:
			ms_measurement = getMeasurement(&ms_list, CPU);
			if(ms_measurement!=NULL) {
				divide_sampling_rates(&(((MS_MEASUREMENT_CPU *)ms_measurement)->msr_time_wait), sec, nsec, check_for_exit_interrupts);
				((MS_MEASUREMENT_CPU *)ms_measurement)->msr_check_for_exit_interrupts = check_for_exit_interrupts;
			}
			break;
		case GPU:
			ms_measurement = getMeasurement(&ms_list, GPU);
			if(ms_measurement!=NULL) {
				divide_sampling_rates(&(((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_time_wait), sec, nsec, check_for_exit_interrupts);
				((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_check_for_exit_interrupts = check_for_exit_interrupts;
			}
			break;
		case FPGA:
			ms_measurement = getMeasurement(&ms_list, FPGA);
			if(ms_measurement!=NULL) {
				divide_sampling_rates(&(((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_time_wait), sec, nsec, check_for_exit_interrupts);
				((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_check_for_exit_interrupts = check_for_exit_interrupts;
			}
			break;
		case MIC:
			ms_measurement = getMeasurement(&ms_list, MIC);
			if(ms_measurement!=NULL) {
				divide_sampling_rates(&(((MS_MEASUREMENT_MIC *)ms_measurement)->mic_time_wait), sec, nsec, check_for_exit_interrupts);
				((MS_MEASUREMENT_MIC *)ms_measurement)->mic_check_for_exit_interrupts = check_for_exit_interrupts;
			}
			break;
		case SYSTEM:
			ms_measurement = getMeasurement(&ms_list, SYSTEM);
			if(ms_measurement!=NULL) {
				divide_sampling_rates(&(((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_time_wait), sec, nsec, check_for_exit_interrupts);
				((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_check_for_exit_interrupts = check_for_exit_interrupts;
			}
			break;
		default:
			std::cout << "!!! 'mgmt' (thread main): Error: cannot set measurement timer. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
	}
}

void ms_init_measurement(MS_SYSTEM *ms_system, MS_LIST *ms_list, int flags) {
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	if (flags & CPU) {
		mgmt->initMeasureThread(CPU, ms_list);
	}
	
	if (flags & GPU) {
		mgmt->initMeasureThread(GPU, ms_list);
	}
	
	if (flags & FPGA) {
		mgmt->initMeasureThread(FPGA, ms_list);
	}
	
	if (flags & SYSTEM) {
		mgmt->initMeasureThread(SYSTEM, ms_list);
	}
	
	if (flags & MIC) {
		mgmt->initMeasureThread(MIC, ms_list);
	}
}

void ms_fini_measurement(MS_SYSTEM *ms_system) {
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	for(int i = 0; i < (log2(ALL+1)); i++) {
		mgmt->finiMeasureThread(1<<i);
	}
}

void ms_start_measurement(MS_SYSTEM *ms_system) {
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	// start threads
	for(int i = 0; i < (log2(ALL+1)); i++) {
		mgmt->lockResourceMutexStart(1<<i);
		mgmt->startMeasureThread(1<<i);
	}
	
	// synchronize threads by mutex locks
	for(int i = 0; i < (log2(ALL+1)); i++) {
		mgmt->lockResourceMutexStart(1<<i);
	}
	
	mgmt->postStartSem(2*log2(ALL+1));
	
#ifdef SIGNALS
	if (kill(getpid(), SIGUSR1)) {
		std::cout << "!!! 'mgmt' (thread main): Warning: Couldn't raise signal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
	}
#endif /* SIGNALS */
	
	// unlock after signal raise
	for(int i = 0; i < (log2(ALL+1)); i++) {
		mgmt->unlockResourceMutexStart(1<<i);
	}
}

void ms_stop_measurement(MS_SYSTEM *ms_system) {
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
#ifdef SIGNALS
	if (kill(getpid(), SIGUSR2)) {
		std::cout << "!!! 'mgmt' (thread main): Warning: Couldn't raise signal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
	}
#endif /* SIGNALS */
	
	for(int i = 0; i < (log2(ALL+1)); i++) {
		mgmt->stopMeasureThread(1<<i);
	}
}

void ms_join_measurement(MS_SYSTEM *ms_system) {
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	for(int i = 0; i < (log2(ALL+1)); i++) {
		mgmt->joinMeasureThread(1<<i);
	}
}

void ms_reg_sighandler_start(MS_SYSTEM *ms_system, void(*signal_handler)(int)) {
#ifdef SIGNALS
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	mgmt->regSighandlerStart(signal_handler);
#else /* NOT(SIGNALS) */
	std::cout << "!!! 'mgmt' (thread main): Warning: You attempt to add signal handlers to a libmeasure compiled without this feature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
#endif /* SIGNALS */
}

void ms_reg_sighandler_stop(MS_SYSTEM *ms_system, void(*signal_handler)(int)) {
#ifdef SIGNALS
	CMgmt *mgmt = (CMgmt *)ms_system->mgmt;
	
	mgmt->regSighandlerStart(signal_handler);
#else /* NOT(SIGNALS) */
	std::cout << "!!! 'mgmt' (thread main): Warning: You attempt to add signal handlers to a libmeasure compiled without this feature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
#endif /* SIGNALS */
}

static void divide_sampling_rates(struct timespec *time_wait, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts) {
	time_wait->tv_sec = sec / check_for_exit_interrupts;
	time_wait->tv_nsec = ((double)sec / check_for_exit_interrupts - time_wait->tv_sec) * 1000000000 + nsec / check_for_exit_interrupts;
	
}