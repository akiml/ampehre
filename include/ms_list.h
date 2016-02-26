/*
 * ms_list.h
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 01/11/16
 * version: 0.7.1 - move functions to query measurement results into the modules
 *          0.7.4 - add support for Odroid XU4 systems
 */

#ifndef __MS_LIST_H__
#define __MS_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#define CPU		0x01
#define GPU		0x02
#define FPGA	0x04
#define SYSTEM	0x08
#define MIC 	0x10
#define ODROID	0x20
#define ALL		(CPU | GPU | FPGA | SYSTEM | MIC | ODROID)
	
#define NULL_CHECK(val)  if(NULL == val) return 0;

typedef struct __ms_list {
	int32_t resource_id;
	void *ms_measurement;
	struct __ms_list *next;
} MS_LIST;

//Functions for MS_LIST
void appendList(MS_LIST **ms_list, int id);
void *getMeasurement(MS_LIST **ms_list, int id);
void deleteList(MS_LIST **ms_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_LIST_H__ */