#include "CComAbstract.h"

CComAbstract::CComAbstract()
{
}

CComAbstract::~CComAbstract()
{

}

void CComAbstract::msm_socket()
{
    if ( (mSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        std::cout << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "created socket" << std::endl;
    }
}

void CComAbstract::msm_shutdown(int socket)
{
    if(shutdown(socket, 2) == -1)  //0 stop receiving, 1 stop transmit, 2 stop both
    {
        std::cout << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "connection " << socket << " is closed" << std::endl;
    }
}

void CComAbstract::msm_recv(void **reply, int &length, int socket)
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
