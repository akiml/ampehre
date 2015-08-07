/*
 * time_watch.c
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
 * created: 2/23/15
 * version: 0.3.1 - add correlation example application
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>

void twatch_start_cpu_time(struct timespec *start) {
	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, start) != 0) {
		printf("gettime failed\n");
	}
}

void twatch_stop_set_cpu_time(struct timespec *start, struct timespec *tgt) {
	struct timespec tmp;
	
	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp) != 0) {
		printf("gettime failed\n");
		return;
	}
	
	tgt->tv_sec = tmp.tv_sec - start->tv_sec;
	tgt->tv_nsec = tmp.tv_nsec - start->tv_nsec;
	if (tgt->tv_nsec < 0) {
		tgt->tv_sec -= 1;
		tgt->tv_nsec += 1000000000;
	}
}

void twatch_stop_set(struct timespec *start, struct timespec *tgt) {
	struct timespec tmp;
	
	if (clock_gettime(CLOCK_MONOTONIC, &tmp) != 0) {
		printf("gettime failed\n");
		return;
	}

	tgt->tv_sec = tmp.tv_sec - start->tv_sec;
	tgt->tv_nsec = tmp.tv_nsec - start->tv_nsec;
	if (tgt->tv_nsec < 0) {
		tgt->tv_sec -= 1;
		tgt->tv_nsec += 1000000000;
	}
}

void twatch_stop_add(struct timespec *start, struct timespec *tgt) {
	struct timespec tmp;
	if (clock_gettime(CLOCK_MONOTONIC, &tmp) != 0) {
		printf("gettime failed\n");
		return;
	}
	tgt->tv_sec += tmp.tv_sec - start->tv_sec;
	tgt->tv_nsec += tmp.tv_nsec - start->tv_nsec;
	if (tgt->tv_nsec < 0) {
		tgt->tv_sec -= 1;
		tgt->tv_nsec += 1000000000;
	}
}

void twatch_print(struct timespec *tspec, const char *name) {
	printf("%s: %lu.%09ld\n", name, tspec->tv_sec, tspec->tv_nsec);
}

int64_t twatch_stop_print_cpu_time(struct timespec *start, const char *name) {
	struct timespec tmp;
	tmp.tv_sec	= 0;
	tmp.tv_nsec	= 0;
	
	twatch_stop_set_cpu_time(start, &tmp);
	twatch_print(&tmp, name);
	return (int64_t)(tmp.tv_sec) * 1000 * 1000 * 1000 + (int64_t)(tmp.tv_nsec);
}

void twatch_start(struct timespec *start) {
	if (clock_gettime(CLOCK_MONOTONIC, start) != 0) {
		printf("gettime failed\n");
	}
}

int64_t twatch_stop_print(struct timespec *start, const char *name) {
	struct timespec tmp;
	tmp.tv_sec	= 0;
	tmp.tv_nsec	= 0;
	
	twatch_stop_set(start, &tmp);
	twatch_print(&tmp, name);
	return (int64_t)(tmp.tv_sec) * 1000 * 1000 * 1000 + (int64_t)(tmp.tv_nsec);
}
