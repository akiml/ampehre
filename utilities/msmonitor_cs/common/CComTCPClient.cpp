#include "CComTCPClient.hpp"

CComTCPClient::CComTCPClient(int port, char *pIPAddress):
    CComTCPAbstract(port, pIPAddress)
{

}

CComTCPClient::~CComTCPClient(){

}

void CComTCPClient::msmConnect(CComTCPData **pComData){
    *pComData					= new CComTCPData();
    (*pComData)->mSocketFildes	= mSockFildes;

    int ret_value = connect(mSockFildes, (struct sockaddr*)&(mServerAddr), sizeof(struct sockaddr_in));

    if (-1 == ret_value) {
        std::cout << "ERROR: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void CComTCPClient::msmShutdown(CComTCPData **pComData){
    int ret_value = -1;
    ret_value = shutdown(mSockFildes, 2);

    if (-1 == ret_value) {
        std::cout << "ERROR: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (NULL != *pComData) {
        delete *pComData;
        *pComData = NULL;
    }
}

