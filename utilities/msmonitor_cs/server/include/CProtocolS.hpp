#ifndef CPROTOCOLS_HPP
#define CPROTOCOLS_HPP

#include <stdio.h>
#include <string>
#include <iostream>
#include <inttypes.h>
#include "CProtocol.hpp"

class CProtocolS : public CProtocol{
	
public:

	CProtocolS(std::string version);
	~CProtocolS();
	int parseMsg(char *msg, const unsigned int length, int* tsk, int* reg, uint64_t* data);
	int checkData(std::string msg, uint64_t* data);
	void answerRegisterMsg(std::string &msg, int reg);
	void termComMsg(std::string &msg, int reg);
	
private:
	
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