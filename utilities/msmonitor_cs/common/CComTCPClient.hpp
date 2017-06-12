#ifndef __CCOMTCPCLIENT_H__
#define __CCOMTCPCLIENT_H__

#include "CComTCPAbstract.hpp"
#include <string.h>

class CComTCPClient : public CComTCPAbstract
{
public:
    CComTCPClient(int port, const char *pIPAddress);
    ~CComTCPClient();

    void msmConnect(CComTCPData **pComData);
    void msmShutdown(CComTCPData **pComData);


//    void msm_connect(const int port, const std::string& addr);
//    void msm_send(const char* msg);
//    void msm_send(void* msg, size_t length);
//    void msm_recv(void** reply, int& length, int socket = mSockfd);

};

#endif // __CCOMTCPCLIENT_H__
