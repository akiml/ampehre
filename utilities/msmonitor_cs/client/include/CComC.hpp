#ifndef CCOMC_HPP
#define CCOMC_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#define PORT 2900

class CComC{
public:
	CComC();
	~CComC();
	void setAddr(std::string addr);
	int initSocket(int port);
	int communicate(const char* msg, char* reply, int length);
	
private:
	int mSockfd;
	std::string mAddress;
};

#endif