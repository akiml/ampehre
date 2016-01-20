/*
 * correlation.c
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

#include "correlation.h"
#include "measure.h"
#include "settings.h"
#include "time_watch.h"

#include "correlation_gpu.h"

#include "printer.h"

#include <math.h>

static void init_array(DATA (*data)[N_CORRELATION]);
static void reset_array(DATA (*symmat)[N_CORRELATION]);
static void correlation_preprocessing(DATA (*data)[N_CORRELATION], DATA* mean, DATA* stddev, DATA float_n);

static void init_correlation_app(DATA **stddev, DATA **mean, DATA **symmat, DATA **data);
static void fini_correlation_app(DATA **stddev, DATA **mean, DATA **symmat, DATA **data);

static void print(ARGUMENTS *settings, MS_LIST *m);

static void init_array(DATA (*data)[N_CORRELATION]) {
	int i, j;
	
	for (i=0; i<N_CORRELATION; i++) {
		for (j=0; j<N_CORRELATION; j++) {
			data[i][j] = (DATA)(i*j)/N_CORRELATION;
		}
	}
}

static void reset_array(DATA (*symmat)[N_CORRELATION]) {
	int i, j;
	
	for (i=0; i<N_CORRELATION; i++) {
		for (j=0; j<N_CORRELATION; j++) {
			symmat[i][j] = 0.0;
		}
	}
}

static void correlation_preprocessing(DATA (*data)[N_CORRELATION], DATA* mean, DATA* stddev, DATA float_n) {
#if 0
	struct timespec two;
	twatch_start_cpu_time(&two);
#endif
	
	int i, j;
	
	DATA eps4der = 0.005;
	
	for (j = 0; j < N_CORRELATION; j++) {
		mean[j] = 0.0;
		
		for (i = 0; i < N_CORRELATION; i++) {
			mean[j] += data[i][j];
		}
		
		mean[j] /= float_n;
	}
	
#if 0
	twatch_stop_print_cpu_time(&two, "correlation_preprocessing_time");
#endif
	
	for (j = 0; j < N_CORRELATION; j++) {
		stddev[j] = 0.0;
		
		for (i = 0; i < N_CORRELATION; i++) {
			stddev[j] += (data[i][j] - mean[j]) * (data[i][j] - mean[j]);
		}
		
		stddev[j] /= float_n;
		stddev[j] = sqrt(stddev[j]);
		
		stddev[j] = stddev[j] <= eps4der ? 1.0 : stddev[j];
	}
}

static void init_correlation_app(DATA **stddev, DATA **mean, DATA **symmat, DATA **data) {
	DATA float_n = (DATA)N_CORRELATION;
	
	*stddev	= (DATA *)malloc(SIZE_VECTOR);
	*mean	= (DATA *)malloc(SIZE_VECTOR);
	*symmat	= (DATA *)malloc(SIZE_MATRIX);
	*data	= (DATA *)malloc(SIZE_MATRIX);
	
#if 0
	struct timespec tmp;
	twatch_start(&tmp);
#endif
	
	init_array((DATA(*)[N_CORRELATION])(*data));
	
#if 0
	twatch_stop_print(&tmp, "init_array_time");
	twatch_start(&tmp);
#endif
	
	correlation_preprocessing((DATA(*)[N_CORRELATION])(*data), *mean, *stddev, float_n);
	
#if 0
	twatch_stop_print(&tmp, "correlation_preprocessing_time");
#endif
}

static void fini_correlation_app(DATA **stddev, DATA **mean, DATA **symmat, DATA **data) {
	(*symmat)[(N_CORRELATION-1)*(N_CORRELATION)+N_CORRELATION-1] = 1.0;
	
	free(*stddev);
	free(*mean);
	free(*symmat);
	free(*data);
	
	*stddev	= NULL;
	*mean	= NULL;
	*symmat	= NULL;
	*data	= NULL;
}

void run(ARGUMENTS *settings) {
#if 0
	struct timespec one;
	twatch_start(&one);
#endif
	
	//MSYSTEM *ms		= NULL;
	//MS_LIST *m	= NULL;
	
	DATA *stddev	= NULL;
	DATA *mean		= NULL;
	DATA *symmat	= NULL;
	DATA *data		= NULL;
	
	//init_measuring_system(settings, &ms, &m);
	
	init_correlation_app(&stddev, &mean, &symmat, &data);

#if 0
	twatch_stop_print(&one, "app_initialization_time");
#endif

#ifndef ORIGINAL
	WORKER worker;
	
	worker.stddev			= stddev;
	worker.mean				= mean;
	worker.symmat			= symmat;
	worker.data				= data;
	worker.float_n			= (DATA)N_CORRELATION;
	
	worker.stddev_gpu		= NULL;
	worker.mean_gpu			= NULL;
	worker.symmat_gpu		= NULL;
	worker.data_gpu			= NULL;
	worker.float_n_gpu		= NULL;
	
	worker.cuda_device_nr	= 0;
	
	worker.cur				= 0;
	
	worker.granularity		= settings->granularity;
#endif /* ORIGINAL */
	
	//start_measuring_system(ms, m);
	
#if 0
	twatch_stop_print(&one, "app_initialization_time");
#endif
	
#if 0
	struct timespec two;
	twatch_start(&two);
#endif
	
#ifdef ORIGINAL
	exec_correlation_app(settings->granularity, stddev, mean, symmat, data);
#else /* PROGRAMMING PATTERN */
	exec_correlation_app(&worker);
#endif /* ORIGINAL */
	
	//stop_measuring_system(ms, m);
	
#if 0
	twatch_stop_print(&two, "total_time");
#endif
	
#if 0
	struct timespec three;
	twatch_start(&three);
#endif
	
	fini_correlation_app(&stddev, &mean, &symmat, &data);
	
#if 0
	twatch_stop_print(&three, "cleanup_time");
#endif
	
	//print(settings, m);
	
	//fini_measuring_system(&ms, &m);
}

static void print(ARGUMENTS *settings, MS_LIST *m) {
	size_t size = 2047;
	char filename[size+1];
	memset(filename, 0, size+1);
	
	int energy_loops	= 0;
	int original		= 0;
	
#ifdef ENERGY_LOOPS
	energy_loops = 1;
#endif /* ENERGY_LOOPS */
	
#ifdef ORIGINAL
	original = 1;
#endif /* ORIGINAL */
	
	snprintf(filename, size, "/usr/ampehre/share/correlation_ORIG%i_N%i_EE%i_ELS%i_GRAN%i.json",
		original, N_CORRELATION, energy_loops, E_CORRELATION, settings->granularity
	);
	
	FILE *file = fopen(filename, "w");
	if (NULL == file) {
		LOG_ERROR("Cannot open json file.");
		exit(EXIT_FAILURE);
	}
	
	print_json(file, settings, m);
	
	fclose(file);
	
	fprintf(stdout, "INFO: File \"%s\" has been written to HDD.\n", filename);
}
