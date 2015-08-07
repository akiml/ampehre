/*
 * ms_hetschedwrapper.h
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
 *          0.1.4 - add signals for measuring system start and stop
 *          0.1.13 - make GPU frequency settable
 *          0.1.15 - make CPU frequency settable
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 */

#ifndef __MS_HETSCHEDWRAPPER_H__
#define __MS_HETSCHEDWRAPPER_H__

#include "ms_taskwrapper.h"
#include "ms_hetschedwrapper_client.h"

/*
 * Initialize and start measuring system.
 * In order to register measuring system start and stop
 * signal handlers you need to set the two function pointers.
 * Use NULL to utilize the empty signal handler of the library.
 */
void mshetsched_init(MS_VERSION *version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max, enum gpu_frequency gpu_freq,
					 void(*signal_handler_start)(int), void(*signal_handler_stop)(int));
// Stop measuring system and free unneeded memory
void mshetsched_destroy(void);

// Register a hetsched task/delegate thread
void mshetsched_register(MTASK **mtask);
// Unregister a hetsched tasl/delegate thread
void mshetsched_unregister(MTASK **mtask);

// Hetsched related measurements

// task currently running on a CPU
void mshetsched_init_cpu_init(MTASK *mtask);
void mshetsched_init_cpu_compute(MTASK *mtask);
void mshetsched_init_cpu_free(MTASK *mtask);
void mshetsched_fini_cpu_init(MTASK *mtask);
void mshetsched_fini_cpu_compute(MTASK *mtask);
void mshetsched_fini_cpu_free(MTASK *mtask);

// task currently running on a GPU
void mshetsched_init_gpu_init(MTASK *mtask);
void mshetsched_init_gpu_compute(MTASK *mtask);
void mshetsched_init_gpu_free(MTASK *mtask);
void mshetsched_fini_gpu_init(MTASK *mtask);
void mshetsched_fini_gpu_compute(MTASK *mtask);
void mshetsched_fini_gpu_free(MTASK *mtask);

// task currently running on an FPGA
void mshetsched_init_fpga_init(MTASK *mtask);
void mshetsched_init_fpga_compute(MTASK *mtask);
void mshetsched_init_fpga_free(MTASK *mtask);
void mshetsched_fini_fpga_init(MTASK *mtask);
void mshetsched_fini_fpga_compute(MTASK *mtask);
void mshetsched_fini_fpga_free(MTASK *mtask);

// Obtain results
MREL_MEASUREMENT *mshetsched_get_cpu_init(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_cpu_compute(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_cpu_free(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_gpu_init(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_gpu_compute(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_gpu_free(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_fpga_init(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_fpga_compute(MTASK *mtask);
MREL_MEASUREMENT *mshetsched_get_fpga_free(MTASK *mtask);

// Obtain transport struct for scheduler to client data transmission.
void mshetsched_get_all(MTASK *mtask, MTRANS *mtrans, uint32_t *size);

#endif /* __MS_HETSCHEDWRAPPER_H__ */
