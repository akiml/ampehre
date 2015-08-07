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

#include "correlation_gpu.h"

#include "correlation_gpu_kernel.h"

#include "settings.h"
#include "time_watch.h"

#include <stdint.h>

#ifdef ORIGINAL

void exec_correlation_app(unsigned int granularity, DATA *stddev, DATA *mean, DATA *symmat, DATA *data) {
	DATA float_n		= (DATA)N_CORRELATION;
	
	/* GPU pointers */
	DATA *stddev_gpu	= NULL;
	DATA *mean_gpu		= NULL;
	DATA *symmat_gpu	= NULL;
	DATA *data_gpu		= NULL;
	DATA *float_n_gpu	= NULL;
	
	int cuda_device_nr	= 0;
	
	/*
	 * INIT
	 */
	CUDACALL( cudaSetDevice(cuda_device_nr) );
	
	CUDACALL( cudaMalloc((void**)&symmat_gpu, SIZE_MATRIX) );
	CUDACALL( cudaMalloc((void**)&data_gpu, SIZE_MATRIX) );
	
	CUDACALL( cudaMalloc((void**)&mean_gpu, SIZE_VECTOR) );
	CUDACALL( cudaMalloc((void**)&stddev_gpu, SIZE_VECTOR) );
	CUDACALL( cudaMalloc((void**)&float_n_gpu, SIZE_SCALAR) );
	
	#ifdef ENERGY_LOOPS
	uint32_t e;
	for (e=0; e<E_CORRELATION; ++e) {
		CUDACALL( cudaMemcpyAsync(symmat_gpu, symmat, SIZE_MATRIX, cudaMemcpyHostToDevice) );
		CUDACALL( cudaMemcpyAsync(data_gpu, data, SIZE_MATRIX, cudaMemcpyHostToDevice) );
		
		CUDACALL( cudaMemcpyAsync(mean_gpu, mean, SIZE_VECTOR, cudaMemcpyHostToDevice) );
		CUDACALL( cudaMemcpyAsync(stddev_gpu, stddev, SIZE_VECTOR, cudaMemcpyHostToDevice) );
		CUDACALL( cudaMemcpyAsync(float_n_gpu, &float_n, SIZE_SCALAR, cudaMemcpyHostToDevice) );
	}
	#else /* NO ENERGY_LOOPS */
	CUDACALL( cudaMemcpyAsync(symmat_gpu, symmat, SIZE_MATRIX, cudaMemcpyHostToDevice) );
	CUDACALL( cudaMemcpyAsync(data_gpu, data, SIZE_MATRIX, cudaMemcpyHostToDevice) );
	
	CUDACALL( cudaMemcpyAsync(mean_gpu, mean, SIZE_VECTOR, cudaMemcpyHostToDevice) );
	CUDACALL( cudaMemcpyAsync(stddev_gpu, stddev, SIZE_VECTOR, cudaMemcpyHostToDevice) );
	CUDACALL( cudaMemcpyAsync(float_n_gpu, &float_n, SIZE_SCALAR, cudaMemcpyHostToDevice) );
	#endif /* ENERGY_LOOPS */
	
	/*
	 * COMPUTE
	 */
	dim3 block(NTHREADSX, NTHREADSY);
	dim3 grid(NBLOCKSX, NBLOCKSY);
	
	correlation_calc_data_gpuOPT <<<grid, block,0>>> ((DATA(*)[N_CORRELATION])data_gpu, mean_gpu, stddev_gpu, float_n_gpu);
	CHECKCUDAERROR("correlation_calc_data_gpuOPT launch failed");
	
	int cur = 0;
	for (cur=0; cur<N_CORRELATION; cur += granularity) {
	#ifdef ENERGY_LOOPS
		for (e=0; e<E_CORRELATION; ++e) {
			dim3 grid2(NBLOCKSX, NBLOCKSY);
			dim3 block2(NTHREADSX, NTHREADSY, NTHREADSZ);
			
			correlation_calc_gpuOPT1 <<<grid2, block2,0>>> (cur, granularity, (DATA(*)[N_CORRELATION])data_gpu, (DATA(*)[N_CORRELATION])symmat_gpu);
			CHECKCUDAERROR("correlation_calc_gpuOPT1 launch failed");
			
			CUDACALL( cudaThreadSynchronize());
			
			correlation_calc_gpuOPT2 <<<grid2, block2,0>>> (cur, granularity, (DATA(*)[N_CORRELATION])data_gpu, (DATA(*)[N_CORRELATION])symmat_gpu);
			CHECKCUDAERROR("correlation_calc_gpuOPT2 launch failed");
			
			CUDACALL( cudaThreadSynchronize());
			
			correlation_calc_gpuOPT3 <<<grid2, block2,0>>> (cur, granularity, (DATA(*)[N_CORRELATION])data_gpu, (DATA(*)[N_CORRELATION])symmat_gpu);
			CHECKCUDAERROR("correlation_calc_gpuOPT3 launch failed");
			
			CUDACALL( cudaThreadSynchronize());
		}
	#else /* NO ENERGY_LOOPS */
		dim3 grid2(NBLOCKSX, NBLOCKSY);
		dim3 block2(NTHREADSX, NTHREADSY, NTHREADSZ);
		
		correlation_calc_gpuOPT1 <<<grid2, block2,0>>> (cur, granularity, (DATA(*)[N_CORRELATION])data_gpu, (DATA(*)[N_CORRELATION])symmat_gpu);
		CHECKCUDAERROR("correlation_calc_gpuOPT1 launch failed");
		
		CUDACALL( cudaThreadSynchronize());
		
		correlation_calc_gpuOPT2 <<<grid2, block2,0>>> (cur, granularity, (DATA(*)[N_CORRELATION])data_gpu, (DATA(*)[N_CORRELATION])symmat_gpu);
		CHECKCUDAERROR("correlation_calc_gpuOPT2 launch failed");
		
		CUDACALL( cudaThreadSynchronize());
		
		correlation_calc_gpuOPT3 <<<grid2, block2,0>>> (cur, granularity, (DATA(*)[N_CORRELATION])data_gpu, (DATA(*)[N_CORRELATION])symmat_gpu);
		CHECKCUDAERROR("correlation_calc_gpuOPT3 launch failed");
		
		CUDACALL( cudaThreadSynchronize());
	#endif /* ENERGY_LOOPS */
	}
	
	/*
	 * FREE
	 */
	#ifdef ENERGY_LOOPS
	for (e=0; e<E_CORRELATION; ++e) {
		CUDACALL( cudaMemcpyAsync(data, data_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
		CUDACALL( cudaMemcpyAsync(symmat, symmat_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
		
		CUDACALL( cudaThreadSynchronize());
	}
	#else /* NO ENERGY_LOOPS */
	CUDACALL( cudaMemcpyAsync(data, data_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
	CUDACALL( cudaMemcpyAsync(symmat, symmat_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
	
	CUDACALL( cudaThreadSynchronize());
	#endif /* ENERGY_LOOPS */
	
	CUDACALL( cudaFree(mean_gpu) );
	CUDACALL( cudaFree(stddev_gpu) );
	CUDACALL( cudaFree(float_n_gpu) );
	CUDACALL( cudaFree(data_gpu) );
	CUDACALL( cudaFree(symmat_gpu) );
}

#else /* PROGRAMMING PATTERN */

static void exec_correlation_app_init(WORKER *worker);
static int exec_correlation_app_compute(WORKER *worker);
static void exec_correlation_app_free(WORKER *worker);

static void exec_correlation_app_init(WORKER *worker) {
	CUDACALL( cudaSetDevice(worker->cuda_device_nr) );

	CUDACALL( cudaMalloc((void**)&(worker->symmat_gpu), SIZE_MATRIX) );
	CUDACALL( cudaMalloc((void**)&(worker->data_gpu), SIZE_MATRIX) );
	if (worker->cur == 0) {
		CUDACALL( cudaMalloc((void**)&(worker->mean_gpu), SIZE_VECTOR) );
		CUDACALL( cudaMalloc((void**)&(worker->stddev_gpu), SIZE_VECTOR) );
		CUDACALL( cudaMalloc((void**)&(worker->float_n_gpu), SIZE_SCALAR) );
	}
	
	#ifdef ENERGY_LOOPS
		uint32_t e;
		for (e=0; e<E_CORRELATION; ++e) {
			CUDACALL( cudaMemcpyAsync(worker->symmat_gpu, worker->symmat, SIZE_MATRIX, cudaMemcpyHostToDevice) );
			CUDACALL( cudaMemcpyAsync(worker->data_gpu, worker->data, SIZE_MATRIX, cudaMemcpyHostToDevice) );
			if (worker->cur == 0) {
				CUDACALL( cudaMemcpyAsync(worker->mean_gpu, worker->mean, SIZE_VECTOR, cudaMemcpyHostToDevice) );
				CUDACALL( cudaMemcpyAsync(worker->stddev_gpu, worker->stddev, SIZE_VECTOR, cudaMemcpyHostToDevice) );
				CUDACALL( cudaMemcpyAsync(worker->float_n_gpu, &(worker->float_n), SIZE_SCALAR, cudaMemcpyHostToDevice) );
			}
		}
	#else /* NO ENERGY_LOOPS */
		CUDACALL( cudaMemcpyAsync(worker->symmat_gpu, worker->symmat, SIZE_MATRIX, cudaMemcpyHostToDevice) );
		CUDACALL( cudaMemcpyAsync(worker->data_gpu, worker->data, SIZE_MATRIX, cudaMemcpyHostToDevice) );
		if (worker->cur == 0) {
			CUDACALL( cudaMemcpyAsync(worker->mean_gpu, worker->mean, SIZE_VECTOR, cudaMemcpyHostToDevice) );
			CUDACALL( cudaMemcpyAsync(worker->stddev_gpu, worker->stddev, SIZE_VECTOR, cudaMemcpyHostToDevice) );
			CUDACALL( cudaMemcpyAsync(worker->float_n_gpu, &(worker->float_n), SIZE_SCALAR, cudaMemcpyHostToDevice) );
		}
	#endif /* ENERGY_LOOPS */
}

static int exec_correlation_app_compute(WORKER *worker) {
	if (worker->cur == 0) {
		dim3 block(NTHREADSX, NTHREADSY);
		dim3 grid(NBLOCKSX, NBLOCKSY);
		
		correlation_calc_data_gpuOPT <<<grid, block,0>>> ((DATA(*)[N_CORRELATION])(worker->data_gpu), worker->mean_gpu, worker->stddev_gpu, worker->float_n_gpu);
		CHECKCUDAERROR("correlation_calc_data_gpuOPT launch failed");
		
		CUDACALL( cudaFree(worker->mean_gpu) );
		CUDACALL( cudaFree(worker->stddev_gpu) );
		CUDACALL( cudaFree(worker->float_n_gpu) );
	}
	
	#ifdef ENERGY_LOOPS
		uint32_t e;
		for (e=0; e<E_CORRELATION; ++e) {
			dim3 grid2(NBLOCKSX, NBLOCKSY);
			dim3 block2(NTHREADSX, NTHREADSY, NTHREADSZ);
			
			correlation_calc_gpuOPT1 <<<grid2, block2,0>>> (worker->cur, worker->granularity, (DATA(*)[N_CORRELATION])(worker->data_gpu), (DATA(*)[N_CORRELATION])(worker->symmat_gpu));
			CHECKCUDAERROR("correlation_calc_gpuOPT1 launch failed");
			
			CUDACALL( cudaThreadSynchronize());
			
			correlation_calc_gpuOPT2 <<<grid2, block2,0>>> (worker->cur, worker->granularity, (DATA(*)[N_CORRELATION])(worker->data_gpu), (DATA(*)[N_CORRELATION])(worker->symmat_gpu));
			CHECKCUDAERROR("correlation_calc_gpuOPT2 launch failed");
			
			CUDACALL( cudaThreadSynchronize());
			
			correlation_calc_gpuOPT3 <<<grid2, block2,0>>> (worker->cur, worker->granularity, (DATA(*)[N_CORRELATION])(worker->data_gpu), (DATA(*)[N_CORRELATION])(worker->symmat_gpu));
			CHECKCUDAERROR("correlation_calc_gpuOPT3 launch failed");
			
			CUDACALL( cudaThreadSynchronize());
		}
	#else /* NO ENERGY_LOOPS */
		dim3 grid2(NBLOCKSX, NBLOCKSY);
		dim3 block2(NTHREADSX, NTHREADSY, NTHREADSZ);
		
		correlation_calc_gpuOPT1 <<<grid2, block2,0>>> (worker->cur, worker->granularity, (DATA(*)[N_CORRELATION])(worker->data_gpu), (DATA(*)[N_CORRELATION])(worker->symmat_gpu));
		CHECKCUDAERROR("correlation_calc_gpuOPT1 launch failed");
		
		CUDACALL( cudaThreadSynchronize());
		
		correlation_calc_gpuOPT2 <<<grid2, block2,0>>> (worker->cur, worker->granularity, (DATA(*)[N_CORRELATION])(worker->data_gpu), (DATA(*)[N_CORRELATION])(worker->symmat_gpu));
		CHECKCUDAERROR("correlation_calc_gpuOPT2 launch failed");
		
		CUDACALL( cudaThreadSynchronize());
		
		correlation_calc_gpuOPT3 <<<grid2, block2,0>>> (worker->cur, worker->granularity, (DATA(*)[N_CORRELATION])(worker->data_gpu), (DATA(*)[N_CORRELATION])(worker->symmat_gpu));
		CHECKCUDAERROR("correlation_calc_gpuOPT3 launch failed");
		
		CUDACALL( cudaThreadSynchronize());
	#endif /* ENERGY_LOOPS */
	
	worker->cur += worker->granularity;
	
	if((worker->cur)>=N_CORRELATION) {
		return 1;
	}
	
	return 0;
}

static void exec_correlation_app_free(WORKER *worker) {
	#ifdef ENERGY_LOOPS
		uint32_t e;
		for (e=0; e<E_CORRELATION; ++e) {
			CUDACALL( cudaMemcpyAsync(worker->data, worker->data_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
			CUDACALL( cudaMemcpyAsync(worker->symmat, worker->symmat_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
			
			CUDACALL( cudaThreadSynchronize());
		}
	#else /* NO ENERGY_LOOPS */
		CUDACALL( cudaMemcpyAsync(worker->data, worker->data_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
		CUDACALL( cudaMemcpyAsync(worker->symmat, worker->symmat_gpu, SIZE_MATRIX, cudaMemcpyDeviceToHost) );
		
		CUDACALL( cudaThreadSynchronize());
	#endif /* ENERGY_LOOPS */
	
	CUDACALL( cudaFree(worker->data_gpu) );
	CUDACALL( cudaFree(worker->symmat_gpu) );
}

void exec_correlation_app(WORKER *worker) {
	int break_loop = 1;
	
	exec_correlation_app_init(worker);
	
	do {
		break_loop = exec_correlation_app_compute(worker);
	} while (!break_loop);
	
	exec_correlation_app_free(worker);
	
#if 0
	int i, j;
	for (i=0; i<N_CORRELATION; ++i) {
		for (j=0; j<N_CORRELATION; ++j) {
			printf("%.5f|", worker->data[i*N_CORRELATION+j]);
		}
		printf("\n");
	}
	
	for (i=0; i<N_CORRELATION; ++i) {
		for (j=0; j<N_CORRELATION; ++j) {
			printf("%.5f|", worker->symmat[i*N_CORRELATION+j]);
		}
		printf("\n");
	}
#endif
}

#endif /* ORIGINAL */
