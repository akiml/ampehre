/*
 * gaussblur_gpu.h
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
 */

#ifndef __GAUSSBLUR_GPU_H__
#define __GAUSSBLUR_GPU_H__

#include <stdint.h>

#include "openacc.h"

typedef struct __PixelPacket {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	uint16_t opacity;
} PixelPacket;

// gaussblur_gpu_calc.c
void gaussblur_calc_gpu_compute(uint16_t *image_in, uint16_t *image_out, uint32_t N_GAUSSBLUR_var);

#endif /* __GAUSSBLUR_GPU_H__ */
