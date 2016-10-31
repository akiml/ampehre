#include "CComC.hpp"

CComC::CComC()
{

}

CComC::~CComC() {

}

void CComC::setAddr(std::string addr)
{
	this->mAddress = addr;
}


int CComC::initSocket(int port) {
	struct sockaddr_in dest;

	std::cout << "creating socket..." << std::endl;
	if ( (mSockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		std::cout << "error creating socket" << std::endl;
		return -1;
	}

	/*---Initialize server address/port struct---*/
	bzero(&dest, sizeof(dest));
	dest.sin_addr.s_addr = inet_addr(mAddress.c_str());
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	    
	std::cout << "connecting to server..." << std::endl;
	/*---Connect to server---*/
	if ( connect(mSockfd, (struct sockaddr*)&dest, sizeof(dest)) < 0 ){
		std::cout << "error connecting to server" << std::endl;
		return -1;
	}
	
	std::cout << "connected to server!" << std::endl;

	return 0;
	
}

int CComC::communicate(const char* msg, char* reply, unsigned int length) {

	//Send some data
	if( send(mSockfd , msg , strlen(msg) , 0) < 0){
		std::cout << "error sending msg from client to server" << std::endl;
		return -1;
	}
	else{
		std::cout<< "msg: " << std::endl;
		unsigned int i = 0;
		while(i < strlen(msg)){
			std::cout<< msg[i];
			i++;
		}
	}
	
	length = recv(mSockfd , reply , 1024 , 0);
	//Receive a reply from the server
	if( length < 0){
		std::cout << "error receiving reply from server" << std::endl;
		return -1;
	}
	else{
		std::cout<< "reply: " << std::endl;
		unsigned int i = 0;
		while(i < length){
			std::cout<< reply[i];
			i++;
		}
	}
	
	return 0;
	
}

