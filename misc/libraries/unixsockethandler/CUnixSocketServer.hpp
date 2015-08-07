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

#ifndef __CUNIXSOCKETSERVER_HPP__
#define __CUNIXSOCKETSERVER_HPP__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string>

namespace NUnixSocket {
	class CUnixSocketServer {
		private:
			std::string mSocketName;
			int mMaxConnections;
			
			int mSocketLocal;
			int mSocketRemote;
			struct sockaddr_un mAddrLocal;
			struct sockaddr_un mAddrRemote;
			
		public:
			CUnixSocketServer(char *pSocketName, int maxConnections);
			~CUnixSocketServer(void);
			
		public:
			void serverSocket(void);
			void serverBind(void);
			void serverListen(void);
			int serverAccept(void);
			ssize_t serverRecv(void *pBuffer, size_t sizeBuffer);
			ssize_t serverSend(void *pBuffer, size_t sizeData);
			void serverShutdownLocal(void);
			void serverShutdownRemote(void);
	};
}

#endif /* __CUNIXSOCKETSERVER_HPP__ */
