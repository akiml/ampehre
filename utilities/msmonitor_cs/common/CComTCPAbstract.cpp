#include "CComTCPAbstract.hpp"

#include <cstdlib>
#include <cstring>

#include <sys/socket.h>

CComTCPAbstract::CComTCPAbstract(int Port, char *pIPAddress) :
	mSockFildes(-1),
	mMaxClients(10)
	{
	
	memset(&mServerAddr, 0, sizeof(struct sockaddr_in));
	mServerAddr.sin_family		= AF_INET;
	mServerAddr.sin_port			= htons(Port);
	if (NULL == pIPAddress) {
		mServerAddr.sin_addr.s_addr	= INADDR_ANY;
	} else {
		mServerAddr.sin_addr.s_addr	= inet_addr(pIPAddress);
	}
}

CComTCPAbstract::~CComTCPAbstract() {
	// nothing todo
}

void CComTCPAbstract::msmSocket() {
	mSockFildes = socket(AF_INET, SOCK_STREAM, 0);
	
	if (-1 == mSockFildes) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

/*
 * Client:  1) shutdown mSocketFildes, delete pComData
 * Server: 	1) shutdown (*pComData)->mSocketFildes, delete pComData
 * 			2) shutdown mSocketFildes
 */
void CComTCPAbstract::msmShutdown(CComTCPData **pComData) {
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

void CComTCPAbstract::msmSetSockOpt() {
	int yes			= 1;
	int ret_value	= setsockopt(mSockFildes, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPAbstract::msmBind() {
	int ret_value = bind(mSockFildes, (struct sockaddr *)&mServerAddr, sizeof(struct sockaddr_in));
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPAbstract::msmListen() {
	int ret_value = listen(mSockFildes, mMaxClients);
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPAbstract::msmAccept(CComTCPData **pComData) {
	*pComData = new CComTCPData();
	
	(*pComData)->mSocketFildes = accept(mSockFildes, (struct sockaddr *)&((*pComData)->mClientAddr), &((*pComData)->mClientAddrLen));
	
	if (-1 = (*pComData)->mSocketFildes) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPAbstract::msmConnect(CComTCPData **pComData) {
	*pComData					= new CComTCPData();
	(*pComData)->mSocketFildes	= mSockFildes;
	
	int ret_value = connect(mSockFildes, (struct sockaddr*)&(mServerAddr), sizeof(struct sockaddr_in));
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPAbstract::msmSend(CComTCPData *pComData) {
	ssize_t ret_value	= -1;
	ssize_t msg_length	= 0;
	
	char *msg	= pComData->getMsg(&msg_length);
	
	ret_value	= send(pComData->mSocketFildes , msg , msg_length , 0);
	
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	} else if (pComData->mMsgLength != ret_value) {
		std::cout << "ERROR: " << "Could not send all data." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CComTCPAbstract::msmRecv(void **reply, int &length, int socket)
{
    std::cout << "inside receive" << std::endl;
    int size = 4096, tmp = 0;
    length = 0;
    *reply = malloc(size);
    void* tmp_rep;
    bool finished = false;
    if (NULL == *reply)
    {
        std::cout << "[FATAL] out of memory!" << std::endl;
        exit(-1);
    }

    while(!finished)
    {
        tmp = recv(socket , *reply , size, 0);
        if(tmp > 0)
        {
            length += tmp;
            if(tmp == size)
            {
                std::cout << "before realloc" << std::endl;
                tmp_rep = realloc(*reply, length+size);
                std::cout << "after realloc" << std::endl;
                if (NULL == tmp_rep)
                {
                    std::cout << "[FATAL] out of memory!" << std::endl;
                    free(*reply);
                    exit(-1);
                }
                *reply = tmp_rep;
            }
            else
            {
                finished = true;
            }
        }
        errno = 0;
    }
    std::cout << "received msg with length: " << length << std::endl;
}
