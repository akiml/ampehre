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
	void acceptSocket(int* recv_length, char* buffer,  int& new_socket);
	int sendMsg(std::string msg, int socket);
	
private:
	
	
	int mSockfd;
	int mPort;
	

};

#endif