/*
 * correlation_gpu_kernel.cu
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

#include "correlation_gpu_kernel.h"

__global__ void correlation_calc_gpuOPT1(unsigned int cur, unsigned int gran, float (*data_d)[N_CORRELATION], float (*symmat_d)[N_CORRELATION]) {
	int BLOCKY = blockIdx.y;
	int BLOCKX = blockIdx.x;
	int THREADY = threadIdx.y;
	int THREADX = threadIdx.x;
	
	int t2, t3, t5,t6;
	
	if ((cur <= N_CORRELATION-2) && (gran >= 1)) {
		for (t2=ceild(cur,16)+BLOCKY;t2<=min(floord(N_CORRELATION-2,16),floord(cur+gran-1,16));t2+=16) {
			for (t3=(max(ceild(cur,16),t2))+BLOCKX;t3<=floord(N_CORRELATION-1,16);t3+=16) {
				for (t5=(max(cur,16*t2))+THREADY;t5<=min(min(min(N_CORRELATION-2,16*t2+15),16*t3+14),cur+gran-1);t5+=16) {
					for (t6=(max(16*t3,t5+1))+THREADX;t6<=min(N_CORRELATION-1,16*t3+15);t6+=16) {
						symmat_d[t5][t6]=0.0;;
					}
				}
			}
		}
		for (t2=ceild(cur,16)+BLOCKY;t2<=min(floord(N_CORRELATION-2,16),floord(cur+gran-1,16));t2+=16) {
			for (t5=max(cur,16*t2);t5<=min(min(N_CORRELATION-2,16*t2+15),cur+gran-1);t5++) {
				symmat_d[t5][t5]=1.0;;
			}
		}
	}
}

__global__ void correlation_calc_gpuOPT2(unsigned int cur, unsigned int gran, float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION]) {
	int BLOCKY = blockIdx.y;
	int BLOCKX = blockIdx.x;
	int THREADY = threadIdx.y;
	int THREADX = threadIdx.x;
	
	int t1, t2, t3, t4, t5, t6;
	
	if ((N_CORRELATION >= 1) && (cur <= N_CORRELATION-1)) {
		for (t1=ceild(cur,16)+BLOCKY;t1<=min(floord(N_CORRELATION-2,16),floord(cur+gran-1,16));t1+=16) {
			for (t2=(max(ceild(cur,16),t1))+BLOCKX;t2<=floord(N_CORRELATION-1,16);t2+=16) {
				for (t3=0;t3<=floord(N_CORRELATION-1,16);t3++) {
					for (t4=(max(cur,16*t1))+THREADY;t4<=min(min(min(N_CORRELATION-2,cur+gran-1),16*t1+15),16*t2+14);t4+=16) {
						for (t5=(max(16*t2,t4+1))+THREADX;t5<=min(N_CORRELATION-1,16*t2+15);t5+=16) {
							for (t6=max(1,16*t3);t6<=min(N_CORRELATION-1,16*t3+15);t6++) {
								symmat[t4][t5]+=(data[t6][t4]*data[t6][t5]);;
							}
						}
					}
				}
			}
		}
	}
}

__global__ void correlation_calc_gpuOPT3(unsigned int cur, unsigned int gran, float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION]) {
	int BLOCKY = blockIdx.y;
	int BLOCKX = blockIdx.x;
	int THREADY = threadIdx.y;
	int THREADX = threadIdx.x;
	
	int t2, t3,t5,t6;
	
	if ((cur < N_CORRELATION)) {
		for (t2=ceild(cur,16)+BLOCKY;t2<=min(floord(N_CORRELATION-2,16),floord(cur+gran-1,16));t2+=16) {
			for (t3=(max(ceild(cur,16),t2))+BLOCKX;t3<=floord(N_CORRELATION-1,16);t3+=16) {
				for (t5=(max(cur,16*t2))+THREADY;t5<=min(min(min(N_CORRELATION-2,16*t2+15),16*t3+14),cur+gran-1);t5+=16) {
					for (t6=(max(16*t3,t5+1))+THREADX;t6<=min(N_CORRELATION-1,16*t3+15);t6+=16) {
						symmat[t6][t5]=symmat[t5][t6];;
					}
				}
			}
		}
	}
}

extern "C" void init_correlation_gpuOPT(float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION], float** data_d, float** symmat_d, int device_num) {
	CUDACALL( cudaSetDevice(device_num) );
	
	CUDACALL( cudaMalloc((void**)data_d, (N_CORRELATION)*(N_CORRELATION)*sizeof(float)) );
	CUDACALL( cudaMalloc((void**)symmat_d, (N_CORRELATION)*(N_CORRELATION)*sizeof(float)) );
	
	CUDACALL( cudaMemcpyAsync(*data_d, data, (N_CORRELATION)*(N_CORRELATION)*sizeof(float), cudaMemcpyHostToDevice) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(*symmat_d, symmat, (N_CORRELATION)*(N_CORRELATION)*sizeof(float), cudaMemcpyHostToDevice) ); //NONBLOCKING
	
	CUDACALL( cudaThreadSynchronize());
}

extern "C" void free_correlation_gpuOPT(float (*data)[N_CORRELATION], float (*symmat)[N_CORRELATION], float* data_d, float* symmat_d) {
	CUDACALL( cudaMemcpyAsync(data, data_d, (N_CORRELATION)*(N_CORRELATION)*sizeof(float), cudaMemcpyDeviceToHost) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(symmat, symmat_d, (N_CORRELATION)*(N_CORRELATION)*sizeof(float), cudaMemcpyDeviceToHost) ); //NONBLOCKING
	
	CUDACALL( cudaThreadSynchronize());
	
	CUDACALL( cudaFree(data_d) );
	CUDACALL( cudaFree(symmat_d) );
}

extern "C" void launch_correlation_gpuOPT(unsigned int cur, unsigned int gran, float* data_d, float* symmat_d) {
	dim3 grid(NBLOCKSX, NBLOCKSY);
	dim3 block(NTHREADSX, NTHREADSY, NTHREADSZ);
	
	correlation_calc_gpuOPT1 <<<grid,block,0>>> (cur, gran, (float(*)[N_CORRELATION])data_d, (float(*)[N_CORRELATION])symmat_d);
	CHECKCUDAERROR("correlation_calc_gpuOPT1 launch failed");
	
	correlation_calc_gpuOPT2 <<<grid,block,0>>> (cur, gran, (float(*)[N_CORRELATION])data_d, (float(*)[N_CORRELATION])symmat_d);
	CHECKCUDAERROR("correlation_calc_gpuOPT2 launch failed");
	
	correlation_calc_gpuOPT3 <<<grid,block,0>>> (cur, gran, (float(*)[N_CORRELATION])data_d, (float(*)[N_CORRELATION])symmat_d);
	CHECKCUDAERROR("correlation_calc_gpuOPT3 launch failed");
}

__global__ void correlation_calc_data_gpuOPT(float (*data_d)[N_CORRELATION], float *mean_d, float *stddev_d, float* float_n_d) {
	int BLOCKY = blockIdx.y;
	int BLOCKX = blockIdx.x;
	int THREADY = threadIdx.y;
	int THREADX = threadIdx.x;
	
	int t2, t3, t4, t5;
	
	if (N_CORRELATION >= 1) {
		for (t2=0+BLOCKY;t2<=floord(N_CORRELATION-1,16);t2+=16) {
			for (t3=0+BLOCKX;t3<=floord(N_CORRELATION-1,16);t3+=16) {
				for (t4=16*t2+THREADY;t4<=min(N_CORRELATION-1,16*t2+15);t4+=16) {
					for (t5=16*t3+THREADX;t5<=min(N_CORRELATION-1,16*t3+15);t5+=16) {
						data_d[t4][t5]-=mean_d[t5];;
						data_d[t4][t5]/=sqrt(*float_n_d)*stddev_d[t5];;
					}
				}
			}
		}
	}
}

extern "C" void launch_correlation_data_gpuOPT(float (*data)[N_CORRELATION], float *mean, float *stddev, float float_n, int device_num) {
	CUDACALL( cudaSetDevice(device_num) );
	
	float* data_d;
	float* mean_d;
	float* stddev_d;
	float* float_n_d;
	
	CUDACALL( cudaMalloc((void**)&data_d, (N_CORRELATION)*(N_CORRELATION)*sizeof(float)) );
	CUDACALL( cudaMalloc((void**)&mean_d, (N_CORRELATION)*sizeof(float)) );
	CUDACALL( cudaMalloc((void**)&stddev_d, (N_CORRELATION)*sizeof(float)) );
	CUDACALL( cudaMalloc((void**)&float_n_d, sizeof(float)) );
	
	CUDACALL( cudaMemcpyAsync(data_d, data, (N_CORRELATION)*(N_CORRELATION)*sizeof(float), cudaMemcpyHostToDevice) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(mean_d, mean, (N_CORRELATION)*sizeof(float), cudaMemcpyHostToDevice) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(stddev_d, stddev, (N_CORRELATION)*sizeof(float), cudaMemcpyHostToDevice) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(float_n_d, &float_n, sizeof(float), cudaMemcpyHostToDevice) ); //NONBLOCKING
	
	dim3 block(NTHREADSX, NTHREADSY);
	dim3 grid(NBLOCKSX, NBLOCKSY);
	
	correlation_calc_data_gpuOPT <<<grid, block,0>>> ((float(*)[N_CORRELATION])data_d, mean_d, stddev_d, float_n_d);
	CHECKCUDAERROR("heat_calc_data_gpuOPT launch failed");
	
	CUDACALL( cudaMemcpyAsync(data, data_d, (N_CORRELATION)*(N_CORRELATION)*sizeof(float), cudaMemcpyDeviceToHost) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(mean, mean_d, (N_CORRELATION)*sizeof(float), cudaMemcpyDeviceToHost) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(stddev, stddev_d, (N_CORRELATION)*sizeof(float), cudaMemcpyDeviceToHost) ); //NONBLOCKING
	CUDACALL( cudaMemcpyAsync(&float_n, float_n_d, sizeof(float), cudaMemcpyDeviceToHost) ); //NONBLOCKING
	
	CUDACALL( cudaFree(data_d) );
	CUDACALL( cudaFree(mean_d) );
	CUDACALL( cudaFree(stddev_d) );
	CUDACALL( cudaFree(float_n_d) );
}
