/*
 * ms_version.h
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
 * created: 11/25/14
 * version: 0.2.4 - add version check functionality to library, wrappers, and tools
 */

#ifndef __MS_VERSION_H__
#define __MS_VERSION_H__

/*
 * The version number defined here is valid for all tools and libraries.
 */
#define MS_MAJOR_VERSION	0
#define MS_MINOR_VERSION	7
#define MS_REVISION_VERSION	4
#define MS_VERSION_STRING	"0.7.4"

#ifndef BUILD_DRIVER
#include <stdint.h>

typedef struct __ms_version {
	uint32_t major;
	uint32_t minor;
	uint32_t revision;
} MS_VERSION;
#endif /* BUILD_DRIVER */

#endif /* __MS_VERSION_H__ */
