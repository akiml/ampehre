#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include <string>

	enum CMD { CLIENT_REG ,DATA_REQ, DATA_RES, TERM_COM };
	
	enum DATA { X, YPowerCpu0, YPowerCpu1, YPowerGpu, YPowerFpga, YPowerMic, YPowerSystem, YTempCpu0, YTempCpu1, YTempGpu, YTempFpgaM, YTempFpgaI, YTempMicDie, YTempSystem, YClockCpu0, YClockCpu1, YClockGpuCore, YClockGpuMem, YClockMicCore, YClockMicMem, YUtilCpu, YUtilGpuCore, YUtilGpuMem, YUtilFpga, YUtilMic, YMemoryCpu, YSwapCpu, YMemoryGpu, YMemoryMic };

class CProtocol{

public:
	CProtocol(std::string version);
	CProtocol();
	~CProtocol();

	
	uint64_t createDataCode(std::vector<int>& values);
	int checkVersion(char* vrs, unsigned int length, std::string version);
	int checkCmd(char* cmd, unsigned int length);
	int setReg(char* txt, unsigned int length, int* registry);

protected:
	std::string mVersion;
};


#endif