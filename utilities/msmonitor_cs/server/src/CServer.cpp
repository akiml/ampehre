#include "CServer.hpp"

CServer::CServer(){
	mVERSION = "MSMv0.1";
	mMeasure = new CMeasure();
	mCom = new CComS();
	mProtocol = new CProtocolS(mVERSION);
}

CServer::~CServer(){}

void CServer::init(){
	mMeasure->init();
	mSockfd = mCom->initSocket();	
}

void CServer::acceptLoop() {
	int recv_length;
	char buffer[1024];
	
	int* task_code = 0;
	int* registry = 0;
	uint64_t* data = 0;
	
	while(1){
		int cl_Socket;
		mCom->acceptSocket(mSockfd, &recv_length, buffer, cl_Socket);
		
		if(mProtocol->parseMsg(buffer, recv_length, task_code, registry, data) < 0){
			std::cout << "[!]error parsing message" << std::endl;
		}
		else{
			std::cout << "SUCCESS!" << std::endl;
		}
		
		close(cl_Socket);
	}
}
