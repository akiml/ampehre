/*
 * CProtocolC.hpp
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
 * created: 10/24/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

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

	static void addAll(std::vector<int>& values);
	static void addPower(std::vector<int>& values);
	static void addTemp(std::vector<int>& values);
	static void addClock(std::vector<int>& values);
	static void addUtil(std::vector<int>& values);
	static void addMemory(std::vector<int>& values);
	
	int parseMsg(void* msg, unsigned int length, int& reg, int& tsk, std::vector< double >& values);
	int getData(void* msg, int size, std::vector<double>& values);
	
	
};

#endif
