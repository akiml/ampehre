#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>
#include <algorithm>

#define PORT 1050

class CComS{
	
public:
	CComS();
	~CComS();
	int initSocket();
	void acceptSocket(int sockfd);
	void registerClient(int socket);
	void unregisterClient(int socket);
	void fatal(char *message);
	void dump(char *data_buffer, const unsigned int length);
	
private:
	std::list<int> mRegClients;

};