/*
 * ms_ipmiwrapper_internal.h
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 10/28/14
 * version: 0.2.1 - add support for IPMI to the measure driver
 */

#ifndef __MS_IPMIWRAPPER_INTERNAL_H__
#define __MS_IPMINODEWRAPPER_INTERNAL_H__

#include "../../../include/ms_driver.h"
#include "../../../include/ms_ipmiwrapper.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define SDR_SENSOR_ID 10
#define SDR_M_LS_BITS 27
#define SDR_M_MS_BITS 28
#define SDR_B_LS_BITS 29
#define SDR_B_MS_BITS 30
#define SDR_EXPONENTS 32

int getSDR(int record_id, unsigned char* datarc, int size);
int getSensorReading(int sensor_id, unsigned char* datarc, int size);

#endif /* __MS_IPMIWRAPPER_INTERNAL_H__*/
