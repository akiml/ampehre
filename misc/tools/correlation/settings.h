/*
 * settings.h
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
 * created: 2/11/15
 * version: 0.3.1 - add correlation example application
 */

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// Include input data size.
#include "settings_dynamic.h"

// Use the original application
#if 1
#define ORIGINAL
#endif

// Enable energy loops
#if 0
#define ENERGY_LOOPS
#endif

// Number of energy loops
#define E_CORRELATION	200

typedef float DATA;
#define SIZE_SCALAR	(sizeof(DATA))
#define SIZE_VECTOR	((N_CORRELATION)*sizeof(DATA))
#define SIZE_MATRIX	((N_CORRELATION)*(N_CORRELATION)*sizeof(DATA))

#endif /* __SETTINGS_H__ */
