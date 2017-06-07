#ifndef CCOMSERVER_H
#define CCOMSERVER_H

#include "CComAbstract.h"

class CComServer : public CComAbstract
{
public:
    CComServer();
    ~CComServer();

    void msm_bind(const int port);
    void msm_listen(const int maxClients);
    int msm_accept();
    void msm_send(const char *msg, int socket = mNewSockfd);
    void msm_send(void* msg, size_t length, int socket = mNewSockfd);
	void msm_recv(void** reply, int& length, int socket = mNewSockfd);
	void msm_shutdown(int socket = mNewSockfd);

private:
    int mNewSockfd;
};

#endif // CCOMSERVER_H
