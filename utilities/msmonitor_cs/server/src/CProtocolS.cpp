/*
 * CProtocolS.cpp
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

#include "CProtocolS.hpp"

CProtocolS::CProtocolS(std::string version){
	mVersion = version;
}

CProtocolS::~CProtocolS(){}

int CProtocolS::parseMsg(char* msg, const unsigned int length, int& tsk, int& reg, uint64_t& data){
	unsigned int i = 0;
	int k = 0;
	std::string msg_str (msg, length);
	std::string end ("\r\n");
	std::string submsg;
	
	std::size_t found = 1;
	std::size_t next = 0;
	std::size_t before = 0;
	
	while(found != std::string::npos){
		found = msg_str.find(end, next);
		before = next;
		next = found;
		if(found == std::string::npos){
			break;
		}
		else{
			submsg = msg_str.substr(before, next-before);
			switch(i){
			  case 0:
				tsk = checkCmdVersion(submsg, mVersion);
				if(tsk < 0){
					return -1;
				}
				break;
			  case 1:
				if(setReg(submsg, reg) < 0){
					if(checkData(submsg, &data) < 0){
						return -1;
					} 
				}
				break;
			}
		}
		next += end.size();
		i++;
	}
	
	return 0;
}


int CProtocolS::checkData(std::string msg, uint64_t* data) {
	*data = 0x0;
	unsigned int length = msg.size();
	if(length == 8){ //8 bytes should be transmitted
		for (unsigned int i = 0 ; i < length; i++ ){
			*data |= (((int)msg.at(i)-20) << i*8);
		}
	}
	else {
		data = NULL;
		return -1;
	}
	return 0;
}

void CProtocolS::answerRegisterMsg(std::string& msg, int reg) {
	addCmdVersion(msg, CLIENT_REG, mVersion);
	msg.append("REG: ");
	std::ostringstream ss;
	ss << reg;
	msg.append(ss.str());
	msg.append("\r\n");
}

void CProtocolS::termComMsg(std::string& msg, int reg){
	addCmdVersion(msg, TERM_COM, mVersion);
	msg.append("REG: ");
	std::ostringstream ss;
	ss << reg;
	msg.append(ss.str());
	msg.append("\r\n");
}



