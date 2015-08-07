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
 * created: 10/27/14
 * version: 0.1.19 - add a hettime based idle power measurement tool
 */

#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <stdio.h>

#include "hetpowidle.h"
#include "list.h"

void print_json(FILE *file, ARGUMENTS* settings, IDLE_POWER *ip_list, IDLE_POWER *ip_median);

#endif /* __PRINTER_H__ */
