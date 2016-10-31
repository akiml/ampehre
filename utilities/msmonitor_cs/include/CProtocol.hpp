#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include <string>

	enum CMD { DATA_REQ, CLIENT_REG , DATA_RES, TERM_COM };
	
	enum DATA { X, YPowerCpu0, YPowerCpu1, YPowerGpu, YPowerFpga, YPowerMic, YPowerSystem, YTempCpu0, YTempCpu1, YTempGpu, YTempFpgaM, YTempFpgaI, YTempMicDie, YTempSystem, YClockCpu0, YClockCpu1, YClockGpuCore, YClockGpuMem, YClockMicCore, YClockMicMem, YUtilCpu, YUtilGpuCore, YUtilGpuMem, YUtilFpga, YUtilMic, YMemoryCpu, YSwapCpu, YMemoryGpu, YMemoryMic };

class CProtocol{

public:
	CProtocol(std::string version);
	CProtocol();
	~CProtocol();

	
	uint64_t createDataCode(std::vector<int>& values);
	int checkVersion(std::string msg, std::string version);
	int checkCmd(std::string msg);
	int setReg(std::string msg, int* registry);

protected:
	std::string mVersion;
};


#endif