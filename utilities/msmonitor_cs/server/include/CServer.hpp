/*
 * CServer.hpp
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

#ifndef CSERVER_HPP
#define CSERVER_HPP

#include "CComServer.h"
#include "CMeasure.hpp"
#include "CProtocolS.hpp"
#include "utils.h"
#include <list>
#include <signal.h>
#include <stdlib.h>
#include <ctime>
#include <pthread.h>


class CServer{

public:
	CServer(int port, int maxClients);
	~CServer();
	void init();
	void acceptLoop();
	void getCurrentTime(double& time);
	std::vector<Application> mApplications;
	std::string mVERSION;
	CComServer mCom;
	CProtocolS mProtocol;
	void answer(int taskCode, int registry, uint64_t datacode);
	
private:
	void registerClient(uint64_t datacode);
	void dataRequest(int registry);
	void terminate(int registry);
	void confirmFreq(int registry);
	void getFrequencies();
	void createDataAnswer(std::string &msg, uint64_t dataCode);
	int createDataAnswer(void** answer, uint64_t dataCode);
	void controlClients();
	static void* clientTask(void* d);


	CMeasure mMeasure;

	std::list<clReg> mRegClients;
	std::list<clReg>::iterator mIterator;
	int mPort;
	int mMaxClients;
	double mCurrentTime;
	std::vector<uint64_t> mFreq;
	std::vector<clock_t> mTimesForClients;
	
	
};

struct ClientData
{
	CServer* srv;
	void* buffer;
	int recv_length;
	int task_code;
	int registry;
	uint64_t data;
	bool termflag;
	int socket;
	std::vector<ClientData>* dataVec;
	std::vector<pthread_t>* threads;
	int pos;
};

#endif
