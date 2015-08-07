/*
 * time_watch.h
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

#ifndef __TIME_WATCH_H__
#define __TIME_WATCH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void twatch_start_cpu_time(struct timespec *start);
void twatch_stop_set_cpu_time(struct timespec *start, struct timespec *tgt);
void twatch_stop_set(struct timespec *start, struct timespec *tgt);
void twatch_stop_add(struct timespec *start, struct timespec *tgt);
void twatch_print(struct timespec *tspec, const char *name);
int64_t twatch_stop_print_cpu_time(struct timespec *start, const char *name);
void twatch_start(struct timespec *start);
int64_t twatch_stop_print(struct timespec *start, const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIME_WATCH_H__ */
