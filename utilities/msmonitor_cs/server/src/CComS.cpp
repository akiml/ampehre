#include "CComS.hpp"

CComS::CComS():
	mSockfd(0),
	mPort(2900),
	mSin_size(sizeof(struct sockaddr_in))
{}

CComS::~CComS(){}

int CComS::initSocket(int port){
	this->mPort = port;
	struct sockaddr_in host_addr;	// my address information
	int yes=1;

	if ((mSockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		std::cout <<"error while creating socket!"<<std::endl;
		return -1;
	}

	if (setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		std::cout << "error setting socket options" << std::endl;
		return -1;
	}
	
	host_addr.sin_family = AF_INET;		 // host byte order
	host_addr.sin_port = htons(port);	 // short, network byte order
	host_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(host_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(mSockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr_in)) == -1){
		std::cout << "error while binding socket" << std::endl;
		return -1;
	}

	if (listen(mSockfd, 5) == -1){
		std::cout << "error listening to socket" << std::endl;
		return -1;
	}
	
	std::cout << "listening on socket " << mSockfd << std::endl;
	return 0;
}

void CComS::acceptSocket(int& new_socket) {
	new_socket = accept(mSockfd, (struct sockaddr *)& mClient_addr, &mSin_size);		

	if(new_socket == -1){
		std::cout << "error while accepting connection" << std::endl;
		exit(-1);
	}
 	//std::cout << "server: got connection from " << inet_ntoa(client_addr.sin_addr)<< " port " << ntohs(client_addr.sin_port)<< std::endl;
}

int CComS::sendMsg(std::string msg, int socket) {
	const char* str = msg.c_str();
	if(send(socket , str , strlen(str) , 0) < 0){
		std::cout<< "error communicating with client" << std::endl;
		return -1;
	}
	return 0;
}