/*
 * common.c
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
 * created: 8/03/14
 * version: 0.1.10 - add child execution time measurement
 */

#include "hettime.h"

#include <sys/mman.h>
#include <string.h>

void alloc_exec_time(EXEC_TIME **time) {
	*time = mmap(NULL, sizeof(EXEC_TIME), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (*time == MAP_FAILED) {
		LOG_ERROR("Cannot allocate shared memory.");
		exit(EXIT_FAILURE);
	}
	
	memset(*time, 0, sizeof(EXEC_TIME));
}

void free_exec_time(EXEC_TIME **time) {
	munmap(*time, sizeof(EXEC_TIME));
	
	*time = NULL;
}

void get_current_time(struct timespec *time_cur) {
	clock_gettime(CLOCK_REALTIME, time_cur);
}

void calc_exec_time_diff(EXEC_TIME *time) {
	if ((time->time_stop.tv_nsec - time->time_start.tv_nsec) < 0) {
		time->time_diff.tv_sec	= 				time->time_stop.tv_sec	- time->time_start.tv_sec	- 1;
		time->time_diff.tv_nsec	= 1000000000 + 	time->time_stop.tv_nsec	- time->time_start.tv_nsec;
	} else {
		time->time_diff.tv_sec	= time->time_stop.tv_sec	- time->time_start.tv_sec;
		time->time_diff.tv_nsec	= time->time_stop.tv_nsec	- time->time_start.tv_nsec;
	}
	time->exec_time_diff	= (double)(time->time_diff.tv_sec) + ((double)(time->time_diff.tv_nsec) / 1000000000.0);
}
