#ifndef CCOMS_HPP
#define CCOMS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>


class CComS{
	
public:
	CComS();
	~CComS();
	int initSocket(int port = 2900);
	void acceptSocket(int& new_socket);
	int sendMsg(std::string msg, int socket);
	int sendMsg(void* msg, size_t len, int socket);
	
private:
	int mSockfd;
	int mPort;
	

};

#endif