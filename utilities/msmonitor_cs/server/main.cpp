/*
 * main.cpp
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

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string>
#include <signal.h>
#include "CServer.hpp"

static void termHandler(int s, siginfo_t* info, void* context);
CServer srv = CServer(2901, 5);

int main(int argc, char **argv) {

	if(argc == 2){
	    std::string s = argv[1];
	    if(s != "-d" && s != "--debug")
	    {
	        std::cout<<"-d (--debug)\t\tdebug information"<< std::endl;
	        return 1;
	    }
	}else if (argc == 1){
	    std::cout.setstate(std::ios_base::failbit);
	}
	else
	    return 1;


	std::cout << "initiating server..." << std::endl; 
	
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	act.sa_sigaction = &termHandler;
	act.sa_flags = SA_SIGINFO;
	if(sigaction(SIGINT, &act, NULL) < 0)
	{
		std::cout << "cannot catch SIGINT!" << std::endl;
	}
	if(sigaction(SIGUSR1, &act, NULL) < 0)
	{
		std::cout << "cannot catch SIGUSR1!" << std::endl;
	}
	if(sigaction(SIGUSR2, &act, NULL) < 0)
	{
		std::cout << "cannot catch SIGUSR2!" << std::endl;
	}	
	srv.init();
	srv.acceptLoop();

	return 0;
}

void termHandler(int s, siginfo_t* info, void* context) 
{
	if(s == SIGINT)	//closes client threads and joins them 
	{
		std::cout << "interrupt triggered!" << std::endl;
		for(unsigned int i = 0; i < srv.mThreads.size(); i++){
			int ret = pthread_cancel(srv.mThreads[i]);
			if(ret == 0){
				ret = pthread_join(srv.mThreads[i], NULL);
				if( ret != 0){
					std::cout << "error joining thread! Error: " << strerror(ret) << std::endl;
				}
				else{
					std::cout << "client thread terminated!" << std::endl;
				}
			}
		}
		if(srv.mThreads.size() > 0){
			srv.mThreads.clear();
			srv.mDataVec.clear();
		}
		std::cout << "terminating server..." << std::endl;
		exit(0);
	}
	else if(s == SIGUSR1)
	{
		std::cout << "Application started!" << std::endl;
		Application a;
		pid_t sender = info->si_pid;
		a.mPid = sender;
		srv.getCurrentTime(a.mTime);
		a.start = true;
		srv.mApplications.push_back(a);
	}
	else if(s == SIGUSR2)
	{
		std::cout << "Application finished!" << std::endl;
		Application a;
		pid_t sender = info->si_pid;
		a.mPid = sender;
		srv.getCurrentTime(a.mTime);
		a.start = false;
		srv.mApplications.push_back(a);
	}
}


