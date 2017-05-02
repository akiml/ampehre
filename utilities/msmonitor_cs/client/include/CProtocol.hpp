/*
 * CProtocol.hpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "utils.h"

    enum CMD { CLIENT_REG, DATA_REQ , DATA_RES, TERM_COM, SET_FREQ };
	
	enum DATA { X, YPowerCpu0, YPowerCpu1, YPowerGpu, YPowerFpga, YPowerMic, YPowerSystem, YTempCpu0, YTempCpu1, YTempGpu, YTempFpgaM, YTempFpgaI, YTempMicDie, YTempSystem, YClockCpu0, YClockCpu1, YClockGpuCore, YClockGpuMem, YClockMicCore, YClockMicMem, YUtilCpu, YUtilGpuCore, YUtilGpuMem, YUtilFpga, YUtilMic, YMemoryCpu, YSwapCpu, YMemoryGpu, YMemoryMic, VALUESIZE };
	
class CProtocol{

public:
	CProtocol(std::string version);
	CProtocol();
	~CProtocol();

	
	uint64_t createDataCode(std::vector<int>& values);
    int checkCmdVersion(std::string msg);
	int setReg(std::string msg, int& registry);
	void addData(std::string &msg, double value);
	void extractData(std::vector<int>& sol, uint64_t dataCode);
	void addCmdVersion(std::string& msg, int cmd, std::string version);

protected:
	std::string mVersion;
};


#endif
