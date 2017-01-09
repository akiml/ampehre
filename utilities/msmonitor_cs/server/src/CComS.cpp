/*
 * CComS.cpp
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

#include "CComS.hpp"

CComS::CComS():
	mSockfd(0),
	mPort(2900)
{}

CComS::~CComS(){}

int CComS::initSocket(int port){
	this->mPort = port;
	struct sockaddr_in host_addr;	// my address information
	int yes=1;

	if ((mSockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		std::cout <<"error while creating socket!"<<std::endl;
		return -1;
	}

	if (setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		std::cout << "error setting socket options" << std::endl;
		return -1;
	}
	
	host_addr.sin_family = AF_INET;		 // host byte order
	host_addr.sin_port = htons(port);	 // short, network byte order
	host_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(host_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(mSockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr_in)) == -1){
		std::cout << "error while binding socket" << std::endl;
		return -1;
	}

	if (listen(mSockfd, 5) == -1){
		std::cout << "error listening to socket" << std::endl;
		return -1;
	}
	
	std::cout << "listening on socket " << mSockfd << std::endl;
	return 0;
}

void CComS::acceptSocket(int& new_socket) {
	struct sockaddr_in client_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);

	new_socket = accept(mSockfd, (struct sockaddr *)& client_addr, &sin_size);		
	if(new_socket == -1){
		std::cout << "error while accepting connection" << std::endl;
		exit(-1);
	}
}

int CComS::sendMsg(std::string msg, int socket) {
	const char* str = msg.c_str();
	if(send(socket , str , strlen(str) , 0) < 0){
		std::cout<< "error communicating with client" << std::endl;
		return -1;
	}
	return 0;
}

int CComS::sendMsg(void* msg, size_t len, int socket){
	if(send(socket , msg , len , 0) < 0){
		std::cout<< "error communicating with client" << std::endl;
		return -1;
	}
	return 0;
}
