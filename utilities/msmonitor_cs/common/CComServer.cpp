#include "CComServer.h"

CComServer::CComServer():
    CComAbstract(),
    mNewSockfd(-1)
{
}

CComServer::~CComServer()
{

}

void CComServer::msm_bind(const int port)
{
    struct sockaddr_in host_addr;	// my address information
    int yes=1;
    if (setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }

    host_addr.sin_family = AF_INET;		 // host byte order
    host_addr.sin_port = htons(port);	 // short, network byte order
    host_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(host_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(mSockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr_in)) == -1)
    {
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }
}


void CComServer::msm_listen(const int maxClients)
{
    if (listen(mSockfd, maxClients) == -1)
    {
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }
    std::cout << "listening on socket " << mSockfd << std::endl;
}

int CComServer::msm_accept()
{
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    mNewSockfd = -1;

    while(mNewSockfd == -1)
    {
        mNewSockfd = accept(mSockfd, (struct sockaddr *)& client_addr, &sin_size);
    }
    return mNewSockfd;
}


void CComServer::msm_shutdown(int socket) 
{
	CComAbstract::msm_shutdown(socket);
}



void CComServer::msm_send(const char *msg, int socket)
{
    if( send(socket , msg , strlen(msg) , 0) == -1)
    {
        std::cout << strerror(errno) << std::endl;
    }
    else
	{
		std::cout << "msg sent!" << std::endl << std::string(msg) << std::endl;
	}
}

void CComServer::msm_send(void* msg, size_t length, int socket)
{
    if( send(socket , msg , length , 0) == -1)
    {
        std::cout << strerror(errno) << std::endl;
    }
        else
	{
		std::cout << "msg sent!" << std::endl << std::string((char*)msg, length) << std::endl;
	}
}


void CComServer::msm_recv(void** reply, int& length, int socket)
{
	CComAbstract::msm_recv(reply, length, socket);
}
