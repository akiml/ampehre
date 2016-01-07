/*
 * measure.h
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

#ifndef __MEASURE_H__
#define __MEASURE_H__

#include "gaussblur.h"

#include "../../../include/ms_measurement.h"
#include "../../../include/ms_version.h"

void init_measuring_system(ARGUMENTS *settings, MS_SYSTEM **ms, MS_LIST **m);
void start_measuring_system(MS_SYSTEM *ms);
void stop_measuring_system(MS_SYSTEM *ms);
void fini_measuring_system(MS_SYSTEM **ms, MS_LIST **m);

#endif /* __MEASURE_H__ */
