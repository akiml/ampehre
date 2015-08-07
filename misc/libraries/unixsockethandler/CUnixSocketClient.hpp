/*
 * CUnixSocketClient.hpp
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

#ifndef __CUNIXSOCKETCLIENT_HPP__
#define __CUNIXSOCKETCLIENT_HPP__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string>

namespace NUnixSocket {
	class CUnixSocketClient {
		private:
			std::string mSocketName;
			
			int mSocket;
			struct sockaddr_un mAddr;
			
		public:
			CUnixSocketClient(char *pSocketName);
			~CUnixSocketClient(void);
			
		public:
			void clientSocket(void);
			int clientConnect(void);
			ssize_t clientSend(void *pBuffer, size_t sizeData);
			ssize_t clientRecv(void *pBuffer, size_t sizeBuffer);
			void clientShutdown(void);
	};
}

#endif /* __CUNIXSOCKETCLIENT_HPP__ */
