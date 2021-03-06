/*
 * CProtocol.cpp
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
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */
#include "CProtocol.hpp"

CProtocol::CProtocol(std::string version) {
	mVersion = version;
}

CProtocol::~CProtocol() {
	// nothing todo
}

uint64_t CProtocol::createDataCode(std::vector<int>& values){
	uint64_t sol = 0;
	
	for(unsigned int i = 0; i < values.size(); i++){
		sol |= (0x1 << values[i]);
	}
	
	return sol;
}

int CProtocol::checkCmdVersion(std::string msg){
	int cmd = -1;
	std::string ver_str = "MSMP/" + mVersion;
	std::size_t found = msg.find(" / ");
	
	if(found != std::string::npos){
		std::string subs = msg.substr(0, found);
		if (subs == "DATA_REQ"){
			cmd = DATA_REQ;
		} else if (subs== "CLIENT_REG") {
			cmd = CLIENT_REG;
		} else if (subs == "DATA_RES") {
			cmd = DATA_RES;
		} else if (subs == "TERM_COM") {
			cmd = TERM_COM;
		} else if (subs == "SET_FREQ") {
			cmd = SET_FREQ;
		} else {
			return -1;
		}
		
		std::string sub2 = msg.substr(found+3);
		if(sub2 == ver_str){
			return cmd;
		}else{
			std::cout << "wrong version, abort..." << std::endl;
			return -1;
		}
		
	}
	else {
		return -1;
	}
}

int CProtocol::setReg(std::string msg, int& registry) {
	std::size_t found = msg.find("REG: ");
	if(found != std::string::npos && found == 0){
		std::istringstream ss(msg.substr(5));
		ss >> registry;
		return 0;
	}
	else {
        //registry = NULL;
		return -1;
	}
}


void CProtocol::addData(std::string& msg, double value) {
	std::ostringstream ss;
	ss.str(""); //clear the stream
	
	ss << value;
	msg.append(ss.str());
	
	msg.append("\r\n");
}

void CProtocol::extractData(std::vector< int >& sol, uint64_t dataCode) {
	sol.clear();
	int tmp = 0;
	uint64_t v = 1;
	for(unsigned int i = 0; i < 64; i++){
		tmp = dataCode & v;
		if(tmp > 0){
			sol.push_back(i);
		}
		v <<= 1;
	}
}

void CProtocol::addCmdVersion(std::string& msg, int cmd, std::string version){
	msg = "";
	switch(cmd){
		case CLIENT_REG:{
			msg.append("CLIENT_REG");
			break;
		}
		case DATA_REQ:{
			msg.append("DATA_REQ");
			break;
		}
		case DATA_RES:{
			msg.append("DATA_RES");
			break;
		}
		case TERM_COM:{
			msg.append("TERM_COM");
			break;
		}
		case SET_FREQ:{
			msg.append("SET_FREQ");
			break;
		}
	}
	msg.append(" / MSMP/");
	msg.append(version);
	msg.append("\r\n");
}
