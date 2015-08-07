/*
 * correlation_gpu.h
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
 * created: 2/04/15
 * version: 0.3.1 - add correlation example application
 */

#ifndef __CORRELATION_GPU_H__
#define __CORRELATION_GPU_H__

#include "settings.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef ORIGINAL
typedef struct __worker {
	DATA *stddev;
	DATA *mean;
	DATA *symmat;
	DATA *data;
	DATA float_n;
	
	DATA *stddev_gpu;
	DATA *mean_gpu;
	DATA *symmat_gpu;
	DATA *data_gpu;
	DATA *float_n_gpu;
	
	int cur;
	
	int cuda_device_nr;
	
	unsigned int granularity;
} WORKER;
#endif /* ORIGINAL */

#ifdef ORIGINAL
void exec_correlation_app(unsigned int granularity, DATA *stddev, DATA *mean, DATA *symmat, DATA *data);
#else /* PROGRAMMING PATTERN */
void exec_correlation_app(WORKER *worker);
#endif /* ORIGINAL */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CORRELATION_GPU_H__ */
