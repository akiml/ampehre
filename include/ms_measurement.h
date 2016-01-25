/*
 * ms_measurement.h
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 12/10/15
 * version: 0.7.0 - modularized measurement struct
 *          0.7.1 - move functions to query measurement results into the modules
 */

#ifndef __MS_MEASUREMENT_H__
#define __MS_MEASUREMENT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>

#include "ms_driver.h"
#include "ms_ipmiwrapper.h"
#include "ms_version.h"
#include "ms_list.h"
#include "ms_cpu_intel_xeon_sandy.h"
#include "ms_gpu_nvidia_tesla_kepler.h"
#include "ms_fpga_maxeler_max3a.h"
#include "ms_mic_intel_knc.h"
#include "ms_sys_dell_idrac7.h"

// Value from kernel (2.6.38.8+mpss3.4.1) which may change with a new kernel version
#define S_PER_JIFFY 0.01
		
#ifdef CPU_LIB
	#define CPU_LIB_NAME "libms_cpu_intel_xeon_sandy.so"
#else
	#define CPU_LIB_NAME "libms_stub.so"
#endif

#ifdef GPU_LIB
	#define GPU_LIB_NAME "libms_gpu_nvidia_tesla_kepler.so"
#else
	#define GPU_LIB_NAME "libms_stub.so"
#endif
		
#ifdef FPGA_LIB
	#define FPGA_LIB_NAME "libms_fpga_maxeler_max3a.so"
#else
	#define FPGA_LIB_NAME "libms_stub.so"
#endif
		
#ifdef SYS_LIB
	#define SYS_LIB_NAME "libms_sys_dell_idrac7.so"
#else
	#define SYS_LIB_NAME "libms_stub.so"
#endif
		
#ifdef MIC_LIB
	#define MIC_LIB_NAME "libms_mic_intel_knc.so"
#else
	#define MIC_LIB_NAME "libms_stub.so"
#endif

enum skip_ms_rate {
	SKIP_PERIODIC,
	SKIP_NEVER,
	SKIP_MS_RATES
};

enum lib_variant {
	VARIANT_LIGHT,
	VARIANT_FULL,
	VARIANTS
};

typedef struct __ms_config {
	int32_t cpu_enabled;
	int32_t gpu_enabled;
	int32_t fpga_enabled;
	int32_t mic_enabled;
	int32_t sys_enabled;
} MS_CONFIG;

typedef struct __ms_system{
	void *mgmt;
	MS_CONFIG *config;
} MS_SYSTEM;

// Library management functions
MS_SYSTEM *ms_init(MS_VERSION* version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
				 enum gpu_frequency gpu_freq, enum ipmi_timeout_setting timeout_setting, enum skip_ms_rate skip_ms, enum lib_variant variant);
void ms_init_fpga_force_idle(MS_SYSTEM *ms_system);
void ms_fini(MS_SYSTEM *ms_system);

MS_LIST *ms_alloc_measurement(MS_SYSTEM *ms_system);
void ms_free_measurement(MS_LIST *ms_list);

void ms_set_timer(MS_LIST *ms_list, int flag, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts);

void ms_init_measurement(MS_SYSTEM *ms_system, MS_LIST *ms_list, int flags);
void ms_fini_measurement(MS_SYSTEM *ms_system);
void ms_start_measurement(MS_SYSTEM *ms_system);
void ms_stop_measurement(MS_SYSTEM *ms_system);
void ms_join_measurement(MS_SYSTEM *ms_system);

void ms_reg_sighandler_start(MS_SYSTEM *ms_system, void(*signal_handler)(int));
void ms_reg_sighandler_stop(MS_SYSTEM *ms_system, void(*signal_handler)(int));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_MEASUREMENT_H__ */
