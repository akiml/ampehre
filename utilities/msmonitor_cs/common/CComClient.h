#ifndef CCOMCLIENT_H
#define CCOMCLIENT_H

#include "CComAbstract.h"

class CComClient : public CComAbstract
{
public:
    CComClient();
    ~CComClient();

    void msm_connect(const int port, const std::string& addr);
    void msm_send(const char* msg);
    void msm_send(void* msg, size_t length);
    void msm_recv(void** reply, int& length, int socket = mSockfd);
    void msm_shutdown();

};

#endif // CCOMCLIENT_H
