/*
 * odroid.h
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 2/24/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#ifndef __ODROID_H__
#define __ODROID_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define INA231_IOCGREG		_IOR('i', 1, ODROID_INA231_IOCREG *)
#define INA231_IOCSSTATUS	_IOW('i', 2, ODROID_INA231_IOCREG *)
#define INA231_IOCGSTATUS	_IOR('i', 3, ODROID_INA231_IOCREG *)
#define INA231_IOCREG_NAME_LENGTH 19

#define DEVFS_SENSOR_ARM	"/dev/sensor_arm"
#define DEVFS_SENSOR_MEM	"/dev/sensor_mem"
#define DEVFS_SENSOR_KFC	"/dev/sensor_kfc"
#define DEVFS_SENSOR_G3D	"/dev/sensor_g3d"

#define SYSFS_SENSOR_TEMP	"/sys/devices/10060000.tmu/temp"

#define SYSFS_SENSOR_FREQ_A7	"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq"
#define SYSFS_SENSOR_FREQ_A15	"/sys/devices/system/cpu/cpu4/cpufreq/cpuinfo_cur_freq"
#define SYSFS_SENSOR_FREQ_MALI	"/sys/devices/11800000.mali/clock"

typedef struct __odroid_ina231_iocreg {
	char name[INA231_IOCREG_NAME_LENGTH+1];
	unsigned int enable;
	unsigned int cur_uV;
	unsigned int cur_uA;
	unsigned int cur_uW;
} ODROID_INA231_IOCREG;

typedef struct __odroid_sensor {
	int fd;
	ODROID_INA231_IOCREG data;
} ODROID_SENSOR;

#endif /* __ODROID_H__ */
