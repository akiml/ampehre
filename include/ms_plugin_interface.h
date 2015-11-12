/*
 * ms_plugin_interface.h
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
 * created: 5/27/15
 * version: 0.5.4 - add dynamic loading of resource specific libraries
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#ifndef __MS_PLUGIN_INTERFACE_H__
#define __MS_PLUGIN_INTERFACE_H__

#include "measurement.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void* init_resource(void* pLogger, void* pParams);
void  fini_resource(void* pMeasureRes);

void* init_resource_thread(void* pLogger, void* pStartSem, MEASUREMENT* pMeasurement, void* pMeasureRes);
void  fini_resource_thread(void* pMeasureResThread);

void  trigger_resource_custom(void* pMeasureRes, void* pParams);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MS_PLUGIN_INTERFACE_H__ */
