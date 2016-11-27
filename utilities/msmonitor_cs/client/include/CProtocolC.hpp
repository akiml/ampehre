#ifndef CPROTOCOLC_HPP
#define CPROTOCOLC_HPP

#include "CProtocol.hpp"
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>


class CProtocolC : public CProtocol{
public:
	CProtocolC(std::string version);
	~CProtocolC();
	
	std::string regMsg(uint64_t code);
	std::string requestMsg(int reg);
	std::string termMsg(int reg);
	
	int parseMsg(char* msg, unsigned int length, int& reg, int& tsk, std::vector< double >& values);
	int getData(std::string &msg, std::vector<double>& values);
private:
	
	
};

#endif