/*
 * CServer.cpp
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

#include "CServer.hpp"

CServer::CServer(int port, int maxClients):
     mVERSION("0.1"),
     mMeasure(CMeasure()),
     mCom(CComS()),
     mProtocol(CProtocolS(mVERSION)),
     mPort(port),
     mMaxClients(maxClients),
     mSocket(0)
{
	//nothing to do here
}

CServer::~CServer()
{
	mMeasure.stop();
}

void CServer::init(){
	mMeasure.start();
	if(mCom.initSocket(mPort) < 0)
		exit(-1);
}


void CServer::acceptLoop() {
	int recv_length;
	char buffer[4096] = {0};
	int task_code = 0;
	int registry = 0;
	uint64_t data = 0;

	struct sigaction act;
	act.sa_handler = termHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, 0);

	while(1){
		mCom.acceptSocket(mSocket);
		recv_length = recv(mSocket, buffer, 4096, 0);
		
		std::cout<<"************************" << std::endl;
		std::cout<<"received: "<<std::endl;
		std::string str(buffer, recv_length);
		std::cout << str;
			
		
		std::cout<<"************************" << std::endl;
		
		if(mProtocol.parseMsg(buffer, recv_length, task_code, registry, data) < 0){
			std::cout << "[!]error parsing message" << std::endl;
		}
		else{
			answer(task_code, registry, data);
		}	

		close(mSocket);
	}
}

void CServer::answer(int taskCode, int registry, uint64_t datacode){
	switch(taskCode){
		case DATA_REQ:{
			dataRequest(registry);
			break;
		}
		case CLIENT_REG:{
			registerClient(datacode);
			break;
		}
		case DATA_RES:{
			std::cout <<"illegal taskcode for client" << std::endl;
			break;
		}
		case TERM_COM:{
			terminate(registry);
			break;
		}
	}
}

void CServer::registerClient(uint64_t datacode){	
	int reg = ut::getReg(mRegClients, mMaxClients);		//find available registry
	clReg a = {reg, datacode};
	mRegClients.push_back(a);							//add to register
	std::string answer;

	mProtocol.answerRegisterMsg(answer, reg);
	mCom.sendMsg(answer, mSocket);
}

void CServer::dataRequest(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0){
		void* m;
		int s = createDataAnswer(&m, mIterator->dataCode);
		mCom.sendMsg(m, s, mSocket);
		free(m);

	}else{
		std::cout<<"client not registered yet!" << std::endl;
	}
}

void CServer::terminate(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0){
		mRegClients.erase(mIterator);
		std::string msg;
		mProtocol.termComMsg(msg, registry);
		mCom.sendMsg(msg, mSocket);
	}
}

void CServer::createDataAnswer(std::string& msg, uint64_t dataCode) {
	mProtocol.addCmdVersion(msg, DATA_RES, mVERSION);
	std::vector<int> d;
	std::vector<double> values;

	mProtocol.extractData(d, dataCode);		//extract wanted data from 64Bit dataCode
	mMeasure.getValues(values, d);			//read needed values into double vector


	for(unsigned int i = 0; i < values.size(); i++){
		mProtocol.addData(msg, values[i]);		//write all data values 
	}
}

int CServer::createDataAnswer(void** answer, uint64_t dataCode) {
	std::string msg;
	mProtocol.addCmdVersion(msg, DATA_RES, mVERSION);
	std::vector<int> d;
	std::vector<double> values;
	char *rn = "\r\n";

	mProtocol.extractData(d, dataCode);		//extract wanted data from 64Bit dataCode
	mMeasure.getValues(values, d);			//read needed values into double vector

	std::size_t size = msg.size()+values.size()*(sizeof(double)+2*sizeof(char));
	
	*answer = malloc(size);
	if (NULL == *answer){
		std::cout << "[FATAL] out of memory!" << std::endl;
		exit(-1);
	}
	const char* str_c = msg.c_str();
	memcpy(*answer, str_c, strlen(str_c));

	void* b = *answer+msg.size();	

	for(unsigned int i = 0; i < values.size(); i++){
		//mProtocol.addData(msg, values[i]);		//write all data values 
		memcpy(b, &values[i], sizeof(double));
		b+=sizeof(double);
		memcpy(b, rn, strlen(rn));
		b += strlen(rn);
	}

	return size;
}



void CServer::termHandler(int s) {
	std::cout << "terminating server..." << std::endl;
	exit(0);
}
