#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "utils.h"

	enum CMD { CLIENT_REG, DATA_REQ , DATA_RES, TERM_COM };
	
	enum DATA { X, YPowerCpu0, YPowerCpu1, YPowerGpu, YPowerFpga, YPowerMic, YPowerSystem, YTempCpu0, YTempCpu1, YTempGpu, YTempFpgaM, YTempFpgaI, YTempMicDie, YTempSystem, YClockCpu0, YClockCpu1, YClockGpuCore, YClockGpuMem, YClockMicCore, YClockMicMem, YUtilCpu, YUtilGpuCore, YUtilGpuMem, YUtilFpga, YUtilMic, YMemoryCpu, YSwapCpu, YMemoryGpu, YMemoryMic };
	
class CProtocol{

public:
	CProtocol(std::string version);
	CProtocol();
	~CProtocol();

	
	uint64_t createDataCode(std::vector<int>& values);
	int checkCmdVersion(std::string msg, std::string version);
	int setReg(std::string msg, int& registry);
	void addData(std::string &msg, double value);
	void extractData(std::vector<int>& sol, uint64_t dataCode);
	void addCmdVersion(std::string& msg, int cmd, std::string version);

protected:
	std::string mVersion;
};


#endif