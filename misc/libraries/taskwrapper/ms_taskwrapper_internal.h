/*
 * ms_taskwrapper_internal.h
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
 * created: 8/05/14
 * version: 0.1.0 - initial implementation
 */

#ifndef __MS_TASKWRAPPER_INTERNAL_H__
#define __MS_TASKWRAPPER_INTERNAL_H__

#include "../../../include/ms_taskwrapper.h"

#define LOG_ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", __FILE__, __LINE__, msg)
#define LOG_WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", __FILE__, __LINE__, msg)

#define NUM_OF_RESOURCES 4

typedef struct __minternal {
	MSYSTEM *global_ms;
	MEASUREMENT *global_m;
	int	resources;
	uint32_t sample_rate_cpu;
	uint32_t sample_rate_gpu;
	uint32_t sample_rate_fpga;
	uint32_t sample_rate_sys;
} MINTERNAL;

void mstw_init_rel_measurement(MREL_MEASUREMENT *m);
void mstw_reset_rel_measurement(MREL_MEASUREMENT *m);
MREL_MEASUREMENT *mstw_get_rel_measurement(MTASK *mtask, int resource, int phase);

void mstw_capture_start_measure(MINTERNAL *minternal, MREL_MEASUREMENT *m);
void mstw_capture_stop_measure(MINTERNAL *minternal, MREL_MEASUREMENT *m);

#endif /* __MS_TASKWRAPPER_INTERNAL_H__ */
