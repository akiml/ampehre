#include "CComTCPAbstract.hpp"

#include <cstring>
#include <sys/socket.h>

CComTCPAbstract::CComTCPAbstract(int Port, const char *pIPAddress) :
	mSockFildes(-1),
	mMaxClients(5)
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

void CComTCPAbstract::setPort(int port) {
	mServerAddr.sin_port = htons(port);
}


int CComTCPAbstract::msmSend(CComTCPData *pComData) {
	ssize_t ret_value	= -1;
	ssize_t msg_length	= 0;
	
	char *msg	= pComData->getMsg(&msg_length);
	
	ret_value	= send(pComData->mSocketFildes , msg , msg_length , 0);
	
    //ECONNRESET -> connection reset by peer, EINTR -> signal occured before any data transmitted
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
        return -1;
	} /*else if (pComData->mMsgLength != ret_value) {
		std::cout << "ERROR: " << "Could not send all data." << std::endl;
		exit(EXIT_FAILURE);
	}*/
	std::cout << "msg with length " << msg_length <<" sent:" << std::endl;
	std::cout<< std::string(msg, msg_length) << std::endl;
    return 0;
}

int CComTCPAbstract::msmRecv(CComTCPData *pComData){
	ssize_t size = 4096, tmp = 0, length = 0;
	void* reply = malloc(size);
	void* tmp_rep;
	bool finished = false;
    errno = 0;

	while(!finished)
	{
		tmp = recv(pComData->mSocketFildes , reply , size, 0);
		//ECONNREFUSED, EINTR, ENOTCONN
        if(errno != 0){
            return -1;
        }
		if(tmp > 0){
			length += tmp;
			if(tmp == size){
				tmp_rep = realloc(reply, length+size);
				if (NULL == tmp_rep){
					std::cout << "[FATAL] out of memory!" << std::endl;
					free(reply);
					exit(-1);
				}
				//pComData->setMsg((char*) tmp_rep);
				reply = tmp_rep;
			}
			else {
				finished = true;
				pComData->setMsg(reply, length);
			}
		}
		else if (tmp == 0){
			finished = true;
            pComData->setMsg(reply, length);
			free(reply);
            return 0;
		}
		else{
			free(reply);
            return -1;
		}
        errno = 0;
	}
	tmp_rep = (void*) pComData->getMsg(&size);
	std::cout << "received msg with length " << length << ":"<< std::endl;
	std::cout << std::string((char*)tmp_rep, size) << std::endl;
	free(reply);
    return 0;
}
