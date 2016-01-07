/*
 * gaussblur.c
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
 * created: 1/11/15
 * version: 0.2.5 - add gaussblur example application
 *          0.7.0 - modularised measurement struct
 */

#include "gaussblur.h"
#include "measure.h"

#include "gaussblur_gpu.h"

#include "printer.h"

static void init_gaussblur_app(DATA **image_out, DATA **image_in, ARGUMENTS *settings);
static void exec_gaussblur_app(DATA *image_out, DATA *image_in, ARGUMENTS *settings);
static void fini_gaussblur_app(DATA **image_out, DATA **image_in, ARGUMENTS *settings);
static void print(ARGUMENTS *settings, MS_LIST *m);

static void init_gaussblur_app(DATA **image_out, DATA **image_in, ARGUMENTS *settings) {
	size_t size_in_bytes = settings->size*settings->size*(PIXEL_CHANNELS*sizeof(DATA));
	
	*image_in	= (DATA *)malloc(size_in_bytes);
	*image_out	= (DATA *)malloc(size_in_bytes);
	
	srand(0);
	
	int i, j;
	for(i=0; i<settings->size; i++) {
		for(j=0; j<(settings->size*PIXEL_CHANNELS); j++) {
			if ((i%PIXEL_CHANNELS)<(PIXEL_CHANNELS-1)) {
				(*image_in)[i*settings->size+j] = (uint16_t)(rand() % USHRT_MAX);
			} else {
				(*image_in)[i*settings->size+j] = 0;
			}
			(*image_out)[i*settings->size+j] = 0;
		}
	}
}

static void exec_gaussblur_app(DATA *image_out, DATA *image_in, ARGUMENTS *settings) {
	size_t size_in_bytes = settings->size*settings->size*(PIXEL_CHANNELS*sizeof(DATA));
	
	DATA *image_in_gpu	= (DATA *)acc_malloc(size_in_bytes);
	DATA *image_out_gpu	= (DATA *)acc_malloc(size_in_bytes);
	
	uint32_t e;
	uint32_t i;
	
	for (e=0; e<settings->energy_loops; ++e) {
		acc_memcpy_to_device(image_in_gpu , image_in , size_in_bytes);
		acc_memcpy_to_device(image_out_gpu, image_out, size_in_bytes);
		
		for (i=0; i<settings->checkpoints; ++i) {
			gaussblur_calc_gpu_compute((uint16_t *)image_in_gpu , (uint16_t *)image_out_gpu, settings->size);
			gaussblur_calc_gpu_compute((uint16_t *)image_out_gpu, (uint16_t *)image_in_gpu , settings->size);
		}
		
		acc_memcpy_from_device(image_in , image_in_gpu , size_in_bytes);
		acc_memcpy_from_device(image_out, image_out_gpu, size_in_bytes);
	}
	
	acc_free(image_in_gpu);
	acc_free(image_out_gpu);
	
	image_in_gpu	= NULL;
	image_out_gpu	= NULL;
}

static void fini_gaussblur_app(DATA **image_out, DATA **image_in, ARGUMENTS *settings) {
	free(*image_in);
	free(*image_out);
	
	*image_in	= NULL;
	*image_out	= NULL;
}

void run(ARGUMENTS *settings) {
	MS_SYSTEM *ms	= NULL;
	MS_LIST *m		= NULL;
	
	DATA *image_in	= NULL;
	DATA *image_out	= NULL;
	
	init_measuring_system(settings, &ms, &m);
	
	init_gaussblur_app(&image_out, &image_in, settings);
	
	start_measuring_system(ms);
	exec_gaussblur_app(image_out, image_in, settings);
	stop_measuring_system(ms);
	
	fini_gaussblur_app(&image_out, &image_in, settings);
	
	print(settings, m);
	
	fini_measuring_system(&ms, &m);
}

static void print(ARGUMENTS *settings, MS_LIST *m) {
	FILE *file = fopen("/usr/ampehre/share/gaussblur_2048_original.json", "w");
	if (NULL == file) {
		LOG_ERROR("Cannot open json file.");
		exit(EXIT_FAILURE);
	}
	
	print_json(file, settings, m);
	
	fclose(file);
}
