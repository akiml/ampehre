#ifndef CPROTOCOLC_HPP
#define CPROTOCOLC_HPP

#include "CProtocol.hpp"
#include <string>
#include <iostream>

class CProtocolC : public CProtocol{
public:
	CProtocolC(std::string version);
	~CProtocolC();
	
	std::string regMsg(uint64_t code);
	std::string requestMsg(char reg);
	std::string termMsg(char reg);
	
	int parseMsg(char* msg, unsigned int length, bool rflag, int* reg, int* tsk, std::vector< double >& values);
	int getData(char* msg, int length, std::vector<double>& values);
private:
	
	
};

#endif