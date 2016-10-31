#include "CServer.hpp"

CServer::CServer(int port):
     mVERSION("MSMv0.1"),
     mMeasure(CMeasure()),
     mCom(CComS()),
     mProtocol(CProtocolS(mVERSION)),
     mPort(port){

}

CServer::~CServer(){}

void CServer::init(){
	mMeasure.init();
	if(mCom.initSocket(mPort) < 0)
		exit(-1);
}

void CServer::acceptLoop() {
	int recv_length;
	char buffer[1024];
	
	int task_code = 0;
	int registry = 0;
	uint64_t data = 0;
	
	while(1){
		int cl_Socket;
		mCom.acceptSocket(&recv_length, buffer, cl_Socket);
		
		std::cout<<"************************" << std::endl;
		std::cout<<"received: "<<std::endl;
		
		int i = 0;
		
		while(i < recv_length){
			std::cout << buffer[i];
			i++;
		}
		
		std::cout<<"************************" << std::endl;
		
		if(mProtocol.parseMsg(buffer, recv_length, &task_code, &registry, &data) < 0){
			std::cout << "[!]error parsing message" << std::endl;
		}
		else{
			std::cout << "task code: " << task_code << std::endl;
			std::cout << "registry: " << registry << std::endl;
			std::cout << "data: " << data << std::endl;
		}
		
		close(cl_Socket);
	}
}
