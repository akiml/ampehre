/*
 * ms_ipmiwrapper.h
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
 *          0.2.4 - add version check functionality to library, wrappers, and tools
 *          0.5.12 - add ioctl call to driver to configure the ipmi timeout
 */

#ifndef __MS_IPMIWRAPPER_H__
#define __MS_IPMIWRAPPER_H__
#include <stdint.h>
#include "ms_version.h"

void init_ipmi_wrapper(MS_VERSION *version);
void close_ipmi_wrapper();

/*Resets the energy counter and returns 0 on succes.*/
int dellResetEnergyCounter();
/*Returns 0 on success, the results are stored in time and result_energy.*/
int dellCumulativeEnergy(uint32_t* time, uint32_t* result_energy);
/*Returns 0 on success and the result in current_power.*/
int dellCurrentPower(uint16_t* current_power);
/*Returns the temperature from the SDR with the given record id.*/
int getTemperature(int record_id);
/*Returns the power from the SDR with the given record id.*/
double getPower(int record_id);
/*Returns the timeout for ipmi requests*/
int getIPMITimeout();
/*Set the timeout for ipmi requests. Returns 0 if successful*/
int setIPMITimeout(uint32_t ipmi_timeout);
/*Set the timeout for ipmi requests and lock the value*/
int setAndLockIPMITimeout(uint32_t ipmi_timeout);

#endif /* __MS_IPMIWRAPPER_H__*/
