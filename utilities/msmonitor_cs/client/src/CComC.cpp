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

	if ( (mSockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		std::cout << "error creating socket" << std::endl;
		return -1;
	}

	/*---Initialize server address/port struct---*/
	bzero(&dest, sizeof(dest));
	dest.sin_addr.s_addr = inet_addr(mAddress.c_str());
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	    
	/*---Connect to server---*/
	if ( connect(mSockfd, (struct sockaddr*)&dest, sizeof(dest)) < 0 ){
		std::cout << "error connecting to server" << std::endl;
		return -1;
	}
	

	return 0;
	
}

int CComC::communicate(const char* msg, void* reply, int& length) {

	//Send some data
	if( send(mSockfd , msg , strlen(msg) , 0) < 0){
		std::cout << "error sending msg from client to server" << std::endl;
		return -1;
	}
	length = recv(mSockfd , reply , 4096 , 0);
	//Receive a reply from the server
	if( length < 0){
		std::cout << "error receiving reply from server" << std::endl;
		return -1;
	}
	else{
		std::cout << "****************************" << std::endl;
		std::cout<< "received: " << std::endl;
		std::string str((char*)reply, length);
		std::size_t found = 2; 
		found = str.find("DATA_RES /", 0);
		if(found != 0)
			std::cout << str;
	}

	return 0;
	
}
