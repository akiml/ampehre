/*
 * CProtocolS.hpp
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
 * created: 10/5/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

#ifndef CPROTOCOLS_HPP
#define CPROTOCOLS_HPP

#include <stdio.h>
#include <string>
#include <iostream>
#include <inttypes.h>
#include <vector>
#include <sstream>
#include "CProtocol.hpp"

class CProtocolS : public CProtocol{
	
public:

	CProtocolS(std::string version);
	~CProtocolS();
	int parseMsg(char *msg, const unsigned int length, int& tsk, int& reg, uint64_t& data);
	int checkData(std::string msg, uint64_t* data);
	void answerRegisterMsg(std::string &msg, int reg);
	void termComMsg(std::string &msg, int reg);
	void freqMsg(std::string &msg, std::vector<uint64_t>& vals);
	
private:
	
};



#endif
