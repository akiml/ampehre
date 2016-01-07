/*
 * list.h
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
 * created: 10/27/14
 * version: 0.1.19 - add a hettime based idle power measurement tool
 *          0.7.0 - modularised measurement struct 
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "hetpowidle.h"

typedef struct __idle_power {
	double idle_power_cpu_0_pkg;
	double idle_power_cpu_0_dram;
	double idle_power_cpu_1_pkg;
	double idle_power_cpu_1_dram;
	double idle_power_cpu;
	double idle_power_gpu;
	double idle_power_fpga;
	double idle_power_mic;
	double idle_power_sys;
	double idle_power_all;
	struct __idle_power *next;
	struct __idle_power *prev;
} IDLE_POWER;

IDLE_POWER *create_element(ARGUMENTS *settings, MS_LIST *m);
void append_list_create_element(IDLE_POWER **ip_list, ARGUMENTS *settings, MS_LIST *m);
void append_list(IDLE_POWER **ip_list, ARGUMENTS *settings, IDLE_POWER *ip);
void sort_list(IDLE_POWER **ip_list, ARGUMENTS *settings);
IDLE_POWER *get_median(IDLE_POWER **ip_list, ARGUMENTS *settings);
void delete_list(IDLE_POWER **ip_list);

#endif /* __LIST_H__ */
