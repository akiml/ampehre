/*
 * mic.h
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
 * created: 3/18/15
 * version: 0.3.4 - add a tool for first intergration tests of mic
 */

#ifndef __MIC_H__
#define __MIC_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define LOG_ERROR(msg) \
	fprintf(stderr, "Error (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)
#define LOG_WARN(msg) \
	fprintf(stderr, "Warning (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)
#define LOG_INFO(msg) \
	fprintf(stdout, "Info (file: %s, line: %i): %s\n", (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__, msg)

#define CHECK_MIC_STATUS_ERROR(status, msg) \
	{ \
		if (status != E_MIC_SUCCESS) { \
			LOG_WARN(msg); \
			exit(EXIT_FAILURE); \
		} \
	}
#define CHECK_MIC_STATUS_WARN(status, msg) \
	{ \
		if (status != E_MIC_SUCCESS) { \
			LOG_WARN(msg); \
		} \
	}

void mic_call(int32_t num_of_measurements);

#endif /* __MIC_H__ */
