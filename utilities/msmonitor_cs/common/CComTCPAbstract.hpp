#ifndef __CCOMTCPABSTRACT_HPP__
#define __CCOMTCPABSTRACT_HPP__

#include <stdio.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

#include <sys/types.h>

#include "CComTCPData.hpp"

class CComTCPAbstract {
	protected:
		int mSockFildes;
		int mMaxClients;
		struct sockaddr_in mServerAddr;

	public:
		CComTCPAbstract(int Port, char *pIPAddress = NULL);
		~CComTCPAbstract();
		
		void msmSocket();
        void msmShutdown(CComTCPData **pComData) = 0;
		
        void msmSend(CComTCPData *pComData);
        void msmRecv(CComTCPData *pComData);
};

#endif /* __CCOMTCPABSTRACT_HPP__ */
