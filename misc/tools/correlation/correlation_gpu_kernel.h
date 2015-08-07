/*
 * correlation_gpu_kernel.h
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

#ifndef __CORRELATION_GPU_KERNEL_H__
#define __CORRELATION_GPU_KERNEL_H__

#include <stdio.h>

#include "settings.h"

#include <omp.h>
#include <cuda.h>
#include <cuda_runtime_api.h>

#define CUDACALL(call)                                                          \
{                                                                               \
    cudaError_t error = call;                                                   \
    if (error!=cudaSuccess)                                                     \
    {                                                                           \
        fprintf(stderr, "CUDA error: %s (%d) in %s on line %d\n",               \
                cudaGetErrorString(error), error, __FILE__, __LINE__);          \
    }                                                                           \
}


#define CHECKCUDAERROR(msg)                                                     \
{                                                                               \
    cudaError_t error = cudaGetLastError();                                     \
    if (error!=cudaSuccess)                                                     \
    {                                                                           \
        fprintf(stderr, "CUDA error: %s (%d) in %s on line %d\n\tMessage: %s",\
                cudaGetErrorString(error), error, __FILE__, __LINE__,msg);      \
        exit(-1);                                                               \
    }                                                                           \
}

#define NBLOCKSY  16
#define NBLOCKSX  16
#define NTHREADSY 16
#define NTHREADSX 16
#define NTHREADSZ 1

#define ceild(n,d)  ceil(((DATA)(n))/((DATA)(d)))
#define floord(n,d) floor(((DATA)(n))/((DATA)(d)))
#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

__global__ void correlation_calc_gpuOPT1(unsigned int cur, unsigned int gran, float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION]);
__global__ void correlation_calc_gpuOPT2(unsigned int cur, unsigned int gran, float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION]);
__global__ void correlation_calc_gpuOPT3(unsigned int cur, unsigned int gran, float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION]);
#ifdef __cplusplus
extern "C" {
#endif
void init_correlation_gpuOPT(float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION], float** data_d, float** symmat_d, int device_num);
void free_correlation_gpuOPT(float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION], float* data_d, float* symmat_d);
void launch_correlation_gpuOPT(unsigned int cur, unsigned int gran, float* data_d, float* symmat_d);
#ifdef __cplusplus
}
#endif

__global__ void correlation_calc_data_gpuOPT(float (*data_d)[N_CORRELATION], float *mean_d, float *stddev_d, float* float_n_d);
#ifdef __cplusplus
extern "C" {
#endif
void launch_correlation_data_gpuOPT(float (*data)[N_CORRELATION], float *mean, float *stddev, float float_n, int device_num);
#ifdef __cplusplus
}
#endif

#endif /* __CORRELATION_GPU_KERNEL_H__ */
