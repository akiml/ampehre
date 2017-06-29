#include "CComTCPClient.hpp"

CComTCPClient::CComTCPClient(int port, const char *pIPAddress):
    CComTCPAbstract(port, pIPAddress)
{

}

CComTCPClient::~CComTCPClient(){

}

void CComTCPClient::msmConnect(CComTCPData **pComData){
	*pComData					= new CComTCPData();
	(*pComData)->mSocketFildes	= mSockFildes;
    errno = ECONNREFUSED;

    while(errno == ECONNREFUSED || errno == ETIMEDOUT){
        errno = 0;
        int ret_value = connect(mSockFildes, (struct sockaddr*)&(mServerAddr), sizeof(struct sockaddr_in));

        //ECONNREFUSED -> not listening, EINTR -> interrupt, EISCONN -> already connected, ENETUNREACH -> no network connection
        //ETIMEOUT -> server too busy?
        if (-1 == ret_value) {
            if(errno != ECONNREFUSED && errno != ETIMEDOUT){
                std::cout << "ERROR: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

void CComTCPClient::msmShutdown(CComTCPData **pComData){
	int ret_value = -1;
	ret_value = shutdown(mSockFildes, 2);

	//EABADF -> mSockFildes is not a valid descriptor, ENOTCONN -> socket not connected, ENOTSOCK -> mSockFildes is not a socket
	if (-1 == ret_value) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		if(errno != ENOTCONN){
			exit(EXIT_FAILURE);
		}
	}
	if (NULL != *pComData) {
		delete *pComData;
		*pComData = NULL;
	}
}

