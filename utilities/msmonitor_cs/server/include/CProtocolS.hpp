#ifndef CPROTOCOLS_HPP
#define CPROTOCOLS_HPP

#include <stdio.h>
#include <string>
#include <iostream>
#include <inttypes.h>

class CProtocolS{
	
public:
	enum CMD { CLIENT_REG ,DATA_REQ, DATA_RES, TERM_COM };
	
	enum DATA { X, YPowerCpu0, YPowerCpu1, YPowerGpu, YPowerFpga, YPowerMic, YPowerSystem, YTempCpu0, YTempCpu1, YTempGpu, YTempFpgaM, YTempFpgaI, YTempMicDie, YTempSystem, YClockCpu0, YClockCpu1, YClockGpuCore, YClockGpuMem, YClockMicCore, YClockMicMem, YUtilCpu, YUtilGpuCore, YUtilGpuMem, YUtilFpga, YUtilMic, YMemoryCpu, YSwapCpu, YMemoryGpu, YMemoryMic };
	
	CProtocolS(std::string version);
	~CProtocolS();
	int parseMsg(char *msg, const unsigned int length, int* tsk, int* reg, uint64_t* data);
	int checkVersion(char* vrs, unsigned int length);
	int checkCmd(char* cmd, unsigned int length);
	int checkReg(char* txt, unsigned int length, int* registry);
	int checkData(char* txt, unsigned int length, uint64_t* data);
	
private:
	std::string mVersion;
	int mCmd;
};

/**
 Msg from Client:
 
 Version
 Task_Code
 list of needed values
 
 Version 
 Task_Code
 Registry
 
 */

#endif