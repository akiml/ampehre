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
     mCom( new CComTCPServer(port)),
     mProtocol(CProtocolS(mVERSION)),
     mMeasure(CMeasure()),
     mPort(port),
     mMaxClients(maxClients),
     mCurrentTime(0)
{
	for(unsigned int k = 0; k < 5; k++)
	{
		mFreq.push_back(0);
	}
	getFrequencies();
	
	for(int i = 0; i < maxClients; i++)
	{
		mTimesForClients.push_back(-1);
	}
	
}

CServer::~CServer()
{
	mMeasure.stop();
}

void CServer::setPort(int port)
{
	mPort = port;
	mCom->setPort(port);
}

void CServer::init()
{
	mMeasure.start();
	mCom->msmSocket();
	mCom->msmSetSockOpt();
	mCom->msmBind();
	mCom->msmListen();
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
				eraseClient(mDataVec[i]);
				mDataVec.erase(mDataVec.begin() + i);
				mThreads.erase(mThreads.begin() + i);
			}
		}
	}
}

void* CServer::clientTask(void* d) 
{
//	signal(SIGUSR1, SIG_IGN);
//	signal(SIGUSR2, SIG_IGN);		//lieber forken??
	int s = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	CComTCPData* pData = (CComTCPData*) d;
	CServer* pSrv = (CServer*) pData->mpSrv;
	
	while(!pData->mTermflag)
	{
		pSrv->mCom->msmRecv(pData);
		
		ssize_t recv_length;
		char* buffer = pData->getMsg(&recv_length);
		if(recv_length <= 0){
			break;
		}
		if(pSrv->mProtocol.parseMsg(buffer, recv_length, pData->mTaskCode, pData->mRegistry, pData->mData) < 0){
			std::cout << "[!]error parsing message" << std::endl;
		}
		else{
			pSrv->answer(pData);
		}	
		if(pData->mTaskCode == TERM_COM)
		{
			pData->mTermflag = true;
		}
	}
	pSrv->mCom->msmShutdown(&pData);
	
	pthread_exit(NULL);
}


void CServer::acceptLoop() {
	int count = 0;
	
	while(1)
	{
		controlClients();
		
		count = mThreads.size();
		if(count < mMaxClients){
			pthread_t t;
			CComTCPData* pData;
			
			std::cout << "waiting for incoming clients..." << std::endl;
			
			mCom->msmAccept(&pData);
			pData->mpSrv = (void *) this;
			
			mThreads.push_back(t);
			mDataVec.push_back(pData);
			
			int ret = pthread_create(&mThreads[count], NULL, clientTask, (void*)mDataVec[count]);
			if(ret)
			{
				std::cout << "error creating p_thread, return code: " << ret << std::endl;
			}
			std::cout << "thread created!" << std::endl;
		}
		else{
			for(unsigned int i = 0; i < mThreads.size(); i++){
				int ret = pthread_join(mThreads[i], NULL);
				if( ret != 0){
					std::cout << "error joining thread! Error number: " << ret << std::endl;
				}
				else{
					std::cout << "client thread terminated!" << std::endl;
				}
			}
			mThreads.clear();
			mDataVec.clear();
		}
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

void CServer::answer(CComTCPData* pData) {
		switch(pData->mTaskCode){
		case DATA_REQ:{
			dataRequest(pData);
			break;
		}
		case CLIENT_REG:{
			registerClient(pData);
			break;
		}
		case DATA_RES:{
			std::cout <<"illegal taskcode for client" << std::endl;
			break;
		}
		case TERM_COM:{
			terminate(pData);
			break;
		}
		case SET_FREQ:{
			confirmFreq(pData);
			break;
		}
	}
}


// void CServer::registerClient(uint64_t datacode){	
// 	int reg = ut::getReg(mRegClients, mMaxClients);		//find available registry
// 	clReg a = {reg, datacode};
// 	mRegClients.push_back(a);							//add to register
// 	std::string answer;
// 	mTimesForClients[reg] = std::clock();
// 	mProtocol.answerRegisterMsg(answer, reg);
// 	mCom.msm_send(answer.c_str());
// 	mCom->msmSend(m);
// }

void CServer::registerClient(CComTCPData* pData){	
	int reg = ut::getReg(mRegClients, mMaxClients);		//find available registry
	clReg a = {reg, pData->mData};
	mRegClients.push_back(a);							//add to register
	std::string answer;
	mTimesForClients[reg] = std::clock();
	mProtocol.answerRegisterMsg(answer, reg);
	
	pData->setMsg(answer.c_str());
	mCom->msmSend(pData);
}

// void CServer::confirmFreq(int registry){
// 	if(ut::find(mRegClients, registry, mIterator) == 0){
// 		std::string m;
// 		mProtocol.freqMsg(m, mFreq);
// 		mCom.msm_send(m.c_str());
// 
// 	}else{
// 		std::cout<<"client not registered yet!" << std::endl;
// 	}
// }

void CServer::confirmFreq(CComTCPData* pData){
	if(ut::find(mRegClients, pData->mRegistry, mIterator) == 0){
		std::string m;
		mProtocol.freqMsg(m, mFreq);
		pData->setMsg(m.c_str());
		mCom->msmSend(pData);
	}else{
		std::cout<<"client not registered yet!" << std::endl;
	}
}

//void CServer::dataRequest(int registry){
// 	if(ut::find(mRegClients, registry, mIterator) == 0){
// 		void* m;
// 		int s = createDataAnswer(&m, mIterator->dataCode);
// 		mCom.msm_send(m, s);
// 		mTimesForClients[registry] = std::clock();
// 		free(m);
// 
// 	}else{
// 		std::cout<<"client not registered yet!" << std::endl;
// 	}
// }

void CServer::dataRequest(CComTCPData* pData){
	if(ut::find(mRegClients, pData->mRegistry, mIterator) == 0){
		void* m;
		int s = createDataAnswer(&m, mIterator->dataCode);
		std::string str ((char*) m , s);
		pData->setMsg(m, s);	//getmsg zu wenig inhalt nach setmsg
// 		std::cout << "?!?!??!?!" << std::endl << str << std::endl<< std::endl;
// 		char* tstmsg;
// 		ssize_t tstsize;
// 		tstmsg =  pData->getMsg(&tstsize);
// 		std::string tststring (tstmsg, tstsize);
// 		std::cout << "!!!!!!!!!!" << std::endl << tststring << std::endl;
		mCom->msmSend(pData);
		mTimesForClients[pData->mRegistry] = std::clock();
		free(m);

	}else{
		std::cout<<"client not registered yet!" << std::endl;
	}
}

// void CServer::terminate(int registry){
// 	if(ut::find(mRegClients, registry, mIterator) == 0){
// 		mRegClients.erase(mIterator);
// 		std::string msg;
// 		mTimesForClients[registry] = -1;
// 		mProtocol.termComMsg(msg, registry);
// 		mCom.msm_send(msg.c_str());
// 	}
// }

void CServer::terminate(CComTCPData* pData){
	if(ut::find(mRegClients, pData->mRegistry, mIterator) == 0){
		mRegClients.erase(mIterator);
		std::string msg;
		mTimesForClients[pData->mRegistry] = -1;
		mProtocol.termComMsg(msg, pData->mRegistry);
		pData->setMsg(msg.c_str());
		mCom->msmSend(pData);
	}
}

void CServer::eraseClient(CComTCPData* pData){
		if(ut::find(mRegClients, pData->mRegistry, mIterator) == 0){
		mRegClients.erase(mIterator);
		mTimesForClients[pData->mRegistry] = -1;
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
		std::string str = ss.str();
		tmp += str; 
		tmp += " ";
		
		std::stringstream st;
		st << (unsigned long)mApplications[i].mTime;
		str = st.str();
		tmp += str;
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

