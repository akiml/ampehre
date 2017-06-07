#ifndef CCOMABSTRACT_H
#define CCOMABSTRACT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

class CComAbstract
{
public:
    CComAbstract();
    ~CComAbstract();

    void msm_socket();
    void msm_shutdown(int socket);
    void msm_recv(void** reply, int& length, int socket);
    virtual void msm_send(const char* msg, int socket) = 0;
    virtual void msm_send(void* msg, size_t length, int socket) = 0;

protected:
    int mSockfd;

};

#endif // CCOMABSTRACT_H
