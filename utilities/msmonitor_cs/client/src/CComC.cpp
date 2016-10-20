#include "CComC.hpp"

CComC::CComC() {

}

CComC::~CComC() {

}

int CComC::initSocket() {

	int sockfd;
    struct sockaddr_in dest;

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        return -1;
    }

    /*---Initialize server address/port struct---*/
    bzero(&dest, sizeof(dest));
    dest.sin_addr.s_addr = inet_addr("131.234.58.31");
	dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
	

    /*---Connect to server---*/
    if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) < 0 ){
        return -1;
    }
    
    std::cout << "connected to server!" << std::endl;

    return sockfd;
	
}

int CComC::communicate(char* msg, char* reply, int sockfd) {

	//Send some data
	if( send(sockfd , msg , strlen(msg) , 0) < 0)
	{
		return -1;
	}
		
	//Receive a reply from the server
	if( recv(sockfd , reply , 1024 , 0) < 0)
	{
		return -1;
	}
	
	return 0;
	
}

