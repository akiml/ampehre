#include "CComTCPServer.hpp"


CComTCPServer::CComTCPServer(int port):
    CComTCPAbstract(port)
{
}

CComTCPServer::~CComTCPServer(){

}

void CComTCPServer::msmAccept(CComTCPData** pComData) {
	*pComData = new CComTCPData();
	
	(*pComData)->mSocketFildes = accept(mSockFildes, (struct sockaddr *)&((*pComData)->mClientAddr), &((*pComData)->mClientAddrLen));
	
	if (-1 == (*pComData)->mSocketFildes) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPServer::msmBind() {
	int ret_value = bind(mSockFildes, (struct sockaddr *)&mServerAddr, sizeof(struct sockaddr_in));
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPServer::msmListen() {
	int ret_value = listen(mSockFildes, mMaxClients);
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPServer::msmSetSockOpt() {
	int yes			= 1;
	int ret_value	= setsockopt(mSockFildes, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPServer::msmShutdown(CComTCPData** pComData) {
	int ret_value	= -1;
	
	if (NULL == *pComData) {
		ret_value = shutdown(mSockFildes, 2);
	} else {
		ret_value = shutdown((*pComData)->mSocketFildes, 2);
	}
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (NULL != *pComData) {
		delete *pComData;
		*pComData = NULL;
	}
}


