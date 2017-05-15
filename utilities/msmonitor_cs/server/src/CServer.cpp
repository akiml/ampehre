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
     mSocket(0),
     mCurrentTime(0)
{
	for(unsigned int k = 0; k < 5; k++)
	{
		mFreq.push_back(0);
	}
	getFrequencies();
	
	for(unsigned int i = 0; i < maxClients; i++)
	{
		mTimesForClients.push_back(-1);
	}
	
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

void CServer::getCurrentTime(double& time) 
{
	mMeasure.getCurrentTime(time);
}


void CServer::controlClients() 
{
	double dur;
	for(unsigned int i = 0; i < mTimesForClients.size(); i++)
	{
		if(mTimesForClients[i] >= 0)
		{
			dur = (std::clock() - mTimesForClients[i]) / (double) CLOCKS_PER_SEC;
			if(dur > 1)
			{
				std::cout << "exceeded maximal interval!" << std::endl;
				terminate(i);
			}
		}
	}
}



void CServer::acceptLoop() {
	int recv_length;
	char buffer[4096] = {0};
	int task_code = 0;
	int registry = 0;
	uint64_t data = 0;


	while(1){
		for(unsigned int n = 0; n < mApplications.size(); n++)
		{
			if(mApplications[n].mTime == 0)
			{
				mApplications[n].mTime = mCurrentTime;
			}
		}
		controlClients();
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


void CServer::getFrequencies()
{

	NData::CDataSettings settings = mMeasure.getSettings();
	for(unsigned int k = 0; k < 5; k++){
		switch(k){
			case(0):
				mFreq[k] = settings.mCPUSamplingRate;
				break;
			case(1):
				mFreq[k] = settings.mGPUSamplingRate;
				break;
			case(2):
				mFreq[k] = settings.mFPGASamplingRate;
				break;
			case(3):
				mFreq[k] = settings.mMICSamplingRate;
				break;
			case(4):
				mFreq[k] = settings.mSystemSamplingRate;
				break;		
		}
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
		case SET_FREQ:{
			confirmFreq(registry);
			break;
		}
	}
}

void CServer::registerClient(uint64_t datacode){	
	int reg = ut::getReg(mRegClients, mMaxClients);		//find available registry
	clReg a = {reg, datacode};
	mRegClients.push_back(a);							//add to register
	std::string answer;
	mTimesForClients[reg] = std::clock();
	mProtocol.answerRegisterMsg(answer, reg);
	mCom.sendMsg(answer, mSocket);
}

void CServer::confirmFreq(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0){
		std::string m;
		mProtocol.freqMsg(m, mFreq);
		mCom.sendMsg(m,  mSocket);

	}else{
		std::cout<<"client not registered yet!" << std::endl;
	}
}

void CServer::dataRequest(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0){
		void* m;
		int s = createDataAnswer(&m, mIterator->dataCode);
		mCom.sendMsg(m, s, mSocket);
		mTimesForClients[registry] = std::clock();
		free(m);

	}else{
		std::cout<<"client not registered yet!" << std::endl;
	}
}

void CServer::terminate(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0){
		mRegClients.erase(mIterator);
		std::string msg;
		mTimesForClients[registry] = -1;
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
		std::cout << "value: " << values[i] << std::endl;
	}

}

int CServer::createDataAnswer(void** answer, uint64_t dataCode) {
	std::string msg;
	mProtocol.addCmdVersion(msg, DATA_RES, mVERSION);
	std::vector<int> d;
	std::vector<double> values;
	std::vector<std::string> values_pid;
	std::vector<std::string> values_app;
	char *rn = "\r\n";

	mProtocol.extractData(d, dataCode);		//extract wanted data from 64Bit dataCode
	mMeasure.getValues(values, d);			//read needed values into double vector
	mMeasure.getProcesses(values_pid);

	std::size_t size_str = 0;
	for(unsigned int i = 0; i < values_pid.size(); i++)
	{
		std::cout<<"process: " << values_pid[i] << std::endl;
		size_str += values_pid[i].size();
	}
	for(unsigned int i = 0; i < mApplications.size(); i++)	//write apps into values_app and count string size
	{
		std::string tmp = "";
		if(mApplications[i].start)
		{
			tmp += "y ";
		}
		else
		{
			tmp += "z ";
		}
		std::stringstream ss;
		ss << mApplications[i].mPid;
		std::string pid_str = ss.str();
		tmp += pid_str; 
		tmp += " ";
		ss.clear();
		ss << (unsigned long)mApplications[i].mTime;
		std::string time_str = ss.str();
		tmp += time_str;
		tmp += "\r\n";
		size_str += tmp.size();
		values_app.push_back(tmp);
	}
	std::size_t size = msg.size()+values.size()*(sizeof(double)+2*sizeof(char)) + size_str;
	
	*answer = malloc(size);
	if (NULL == *answer){
		std::cout << "[FATAL] out of memory!" << std::endl;
		exit(-1);
	}
	const char* str_c = msg.c_str();
	memcpy(*answer, str_c, strlen(str_c));

	void* b = *answer+msg.size();	
	const char* c;

	for(unsigned int i = 0; i < values.size(); i++)
	{
//		std::cout << "value: " << values[i] << std::endl;
		//mProtocol.addData(msg, values[i]);		//write all data values 
		memcpy(b, &values[i], sizeof(double));
		b+=sizeof(double);
		memcpy(b, rn, strlen(rn));
		b += strlen(rn);
	}
	for(unsigned int i = 0; i < values_pid.size(); i++)	
	{
		if(values_pid[i].size() > 2)
		{
			c = values_pid[i].c_str();
			std::cout << std::string(c, values_pid[i].size()) << std::endl;
			memcpy(b, c, values_pid[i].size());
			b+=values_pid[i].size();
		}
	}
	for(unsigned int i = 0; i < values_app.size(); i++)	
	{
		if(values_app[i].size() > 2)
		{
			c = values_app[i].c_str();
			std::cout << values_app[i] << std::endl;
			memcpy(b, c, values_app[i].size());
			b+=values_app[i].size();
		}
	}
	

	return size;
}

