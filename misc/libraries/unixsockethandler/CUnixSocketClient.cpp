/*
 * CUnixSocketClient.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 3/03/15
 * version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
 */

#include "CUnixSocketClient.hpp"

#include <cstdlib>
#include "common.hpp"

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif /* UNIX_PATH_MAX */

namespace NUnixSocket {
	CUnixSocketClient::CUnixSocketClient(char *pSocketName) :
		mSocketName(pSocketName),
		mSocket(-1)
		{
		
		memset(&mAddr, 0, sizeof(struct sockaddr_un));
	}
	
	CUnixSocketClient::~CUnixSocketClient(void) {
		if (mSocket != -1) {
			clientShutdown();
		}
	}
	
	void CUnixSocketClient::clientSocket(void) {
		mSocket = socket(AF_UNIX, SOCK_STREAM, 0);
		if (mSocket == -1) {
			LOG_ERROR("Cannot create socket.");
			exit(EXIT_FAILURE);
		}
	}
	
	int CUnixSocketClient::clientConnect(void) {
		int connected	= 0;
		int res			= -1;
		
		memset(&mAddr, 0, sizeof(struct sockaddr_un));
		mAddr.sun_family	= AF_UNIX;
		
		strncpy(mAddr.sun_path, mSocketName.c_str(), UNIX_PATH_MAX-1);
		
		res = connect(mSocket, (struct sockaddr *)&mAddr, sizeof(struct sockaddr_un));
		if (res == -1) {
			LOG_WARN("Cannot connect to server.");
			connected = 0;
		} else {
			connected = 1;
		}
		
		return connected;
	}
	
	ssize_t CUnixSocketClient::clientSend(void *pBuffer, size_t sizeData) {
		ssize_t bytesSent = send(mSocket, pBuffer, sizeData, 0);
		if (bytesSent == -1) {
			LOG_ERROR("Cannot send data to server.");
			exit(EXIT_FAILURE);
		}
		
		return bytesSent;
	}
	
	ssize_t CUnixSocketClient::clientRecv(void *pBuffer, size_t sizeBuffer) {
		ssize_t bytesReceived = recv(mSocket, pBuffer, sizeBuffer, 0);
		if (bytesReceived == -1) {
			LOG_ERROR("Cannot receive data from server.");
			exit(EXIT_FAILURE);
		}
		
		return bytesReceived;
	}
	
	void CUnixSocketClient::clientShutdown(void) {
		int res = -1;
		
		res = shutdown(mSocket, SHUT_RDWR);
		if (res == -1) {
			LOG_ERROR("Cannot close socket.");
			exit(EXIT_FAILURE);
		}
		
		mSocket = -1;
	}
}
