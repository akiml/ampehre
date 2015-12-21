/*
 * printer.h
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
 * created: 2/26/14
 * version: 0.3.1 - add correlation example application
 *          0.5.6 - extended hettime csv printer
 *          0.6.1 - add json printer to hettime
 */

#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "hettime.h"

#define MAX_HEADER_LENGTH 4096
#define MAX_UNITS_LENGTH 1024
#define MAX_VALUES_LENGTH 4096

void print_ostream(FILE *file, ARGUMENTS* settings, MEASUREMENT *m, EXEC_TIME *exec_time);
void print_csv(FILE *csv, ARGUMENTS* settings, MEASUREMENT *m, EXEC_TIME *exec_time);
void print_json(FILE *json, ARGUMENTS *settings, MEASUREMENT *m);

#endif /* __PRINTER_H__ */
