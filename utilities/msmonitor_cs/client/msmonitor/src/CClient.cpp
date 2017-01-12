/*
 * CClient.cpp
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
 * created: 10/25/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

#include "CClient.hpp"

CClient::CClient():
    mVERSION("0.1"),
    mCom(CComC()),
    mProtocol(CProtocolC(mVERSION)),
    mSocket(0),
    mReg(0)
{
	std::cout << "Client Version: " << mVERSION << std::endl;
}

CClient::~CClient() {

}

int CClient::registerToServer(std::vector< int >& values, int port, std::string ipaddr) {
	mIPaddr = ipaddr;
	mPort = port;
	mAmtValues = values.size();
	
	initSocket();
	
	void* rep = malloc(2048);
	if (NULL == rep){
		std::cout << "[FATAL] out of memory!" << std::endl;
		exit(-1);
	}
	int rep_len = 0;
	int reg = -1, tsk;
	
	uint64_t code = mProtocol.createDataCode(values);
	std::string msg = mProtocol.regMsg(code);
	std::vector<double> ignore;
	
	mCom.communicate(msg.c_str() ,rep, rep_len);
	mProtocol.parseMsg(rep, rep_len, reg, tsk, ignore);
	
	free(rep);
	if(reg >= 0){
		this->mReg = reg;
		return 0;
	}
	else 
		return -1;
}

void CClient::requestData() {
	initSocket();
	void* rep= malloc(8192);
	if (NULL == rep){
		std::cout << "[FATAL] out of memory!" << std::endl;
		exit(-1);
	}
	int rep_len = 0;
	int reg = -1, tsk;
	
	std::string msg = mProtocol.requestMsg(mReg);
	mCom.communicate(msg.c_str(), rep, rep_len);
	mProtocol.parseMsg(rep, rep_len, reg, tsk, mValues);

	free(rep);
	
}

void CClient::terminate() {
	initSocket();
	void* rep = malloc(4096);
	if (NULL == rep){
		std::cout << "[FATAL] out of memory!" << std::endl;
		exit(-1);
	}
	int rep_len = 0;

	std::string msg = mProtocol.termMsg(mReg);

	mCom.communicate(msg.c_str(), rep, rep_len);

	free(rep);
}

void CClient::initSocket(){
	mCom.setAddr(mIPaddr);
	mSocket = mCom.initSocket(mPort);
}
