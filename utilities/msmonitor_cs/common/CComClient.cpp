#include "CComClient.h"

CComClient::CComClient():
    CComAbstract()
{

}

CComClient::~CComClient()
{

}

void CComClient::msm_connect(const int port, const std::string& addr)
{
    struct sockaddr_in dest;
    /*---Initialize server address/port struct---*/
    bzero(&dest, sizeof(dest));
    dest.sin_addr.s_addr = inet_addr(addr.c_str());
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);

    /*---Connect to server---*/
    if ( connect(mSockfd, (struct sockaddr*)&dest, sizeof(dest)) == -1 )
    {
        std::cout << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "connected to server: "<< addr <<" at port: " << port << std::endl;
    }

}

void CComClient::msm_send(const char *msg)
{
    if( send(mSockfd , msg , strlen(msg) , 0) == -1)
    {
        std::cout << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "msg sent!"<< std::endl;
    }
}

void CComClient::msm_send(void* msg, size_t length)
{
    if( send(mSockfd , msg , length , 0) == -1)
    {
        std::cout << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "msg sent!"<< std::endl;
    }
}

void CComClient::msm_recv(void** reply, int& length, int socket)
{
    CComAbstract::msm_recv(reply, length, socket);
}

void CComClient::msm_shutdown()
{
    CComAbstract::msm_shutdown(mSockfd);
}
