/*
 * CUnixSocketServer.hpp
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
 * created: 3/02/15
 * version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
 */

#include "CUnixSocketServer.hpp"

#include <cstdlib>
#include "common.hpp"

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif /* UNIX_PATH_MAX */

namespace NUnixSocket {
	CUnixSocketServer::CUnixSocketServer(char *pSocketName, int maxConnections) :
		mSocketName(pSocketName),
		mMaxConnections(maxConnections),
		mSocketLocal(-1),
		mSocketRemote(-1)
		{
		
		memset(&mAddrLocal, 0, sizeof(struct sockaddr_un));
		memset(&mAddrRemote, 0, sizeof(struct sockaddr_un));
	}
	
	CUnixSocketServer::~CUnixSocketServer(void) {
		if (mSocketRemote != -1) {
			serverShutdownRemote();
		}
		if (mSocketLocal != -1) {
			serverShutdownLocal();
		}
	}
	
	void CUnixSocketServer::serverSocket(void) {
		mSocketLocal = socket(AF_UNIX, SOCK_STREAM, 0);
		if (mSocketLocal == -1) {
			LOG_ERROR("Cannot create socket.");
			exit(EXIT_FAILURE);
		}
	}
	
	void CUnixSocketServer::serverBind(void) {
		int res = -1;
		
		memset(&mAddrLocal, 0, sizeof(struct sockaddr_un));
		mAddrLocal.sun_family	= AF_UNIX;
		
		strncpy(mAddrLocal.sun_path, mSocketName.c_str(), UNIX_PATH_MAX-1);
		unlink(mAddrLocal.sun_path);
		
		res = bind(mSocketLocal, (struct sockaddr *)&mAddrLocal, sizeof(struct sockaddr_un));
		if (res == -1) {
			LOG_ERROR("Cannot bind socket path name to socket.");
			exit(EXIT_FAILURE);
		}
	}
	
	void CUnixSocketServer::serverListen(void) {
		int res = -1;
		
		res = listen(mSocketLocal, mMaxConnections);
		if (res == -1) {
			LOG_ERROR("Cannot listen for connections on socket.");
			exit(EXIT_FAILURE);
		}
	}
	
	int CUnixSocketServer::serverAccept(void) {
		int accepted	= 0;
		socklen_t len	= sizeof(struct sockaddr_un);
		
		mSocketRemote	= accept(mSocketLocal, (struct sockaddr *)&mAddrRemote, &len);
		if (mSocketRemote == -1) {
			LOG_WARN("Cannot accept connection on socket.");
			accepted	= 0;
		} else {
			accepted	= 1;
		}
		
		return accepted;
	}
	
	ssize_t CUnixSocketServer::serverRecv(void *pBuffer, size_t sizeBuffer) {
		ssize_t bytesReceived = recv(mSocketRemote, pBuffer, sizeBuffer, 0);
		if (bytesReceived == -1) {
			LOG_ERROR("Cannot receive data from client.");
			exit(EXIT_FAILURE);
		}
		
		return bytesReceived;
	}
	
	ssize_t CUnixSocketServer::serverSend(void *pBuffer, size_t sizeData) {
		ssize_t bytesSent = send(mSocketRemote, pBuffer, sizeData, 0);
		if (bytesSent == -1) {
			LOG_ERROR("Cannot send data to client.");
			exit(EXIT_FAILURE);
		}
		
		return bytesSent;
	}
	
	void CUnixSocketServer::serverShutdownLocal(void) {
		int res = -1;
		
		res = shutdown(mSocketLocal, SHUT_RDWR);
		if (res == -1) {
			LOG_ERROR("Cannot close socket.");
			exit(EXIT_FAILURE);
		}
		
		mSocketLocal = -1;
	}
	
	void CUnixSocketServer::serverShutdownRemote(void) {
		int res = -1;
		
		res = shutdown(mSocketRemote, SHUT_RDWR);
		if (res == -1) {
			LOG_ERROR("Cannot close socket.");
			exit(EXIT_FAILURE);
		}
		
		mSocketRemote = -1;
	}
}
