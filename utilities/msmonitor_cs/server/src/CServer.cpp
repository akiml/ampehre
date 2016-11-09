#include "CServer.hpp"

CServer::CServer(int port, int maxClients):
     mVERSION("MSMv0.1"),
     mMeasure(CMeasure()),
     mCom(CComS()),
     mProtocol(CProtocolS(mVERSION)),
     mPort(port),
     mMaxClients(maxClients),
     mSocket(0)
{
	//nothing to do here
}

CServer::~CServer()
{
	//nothing to do here
}

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
	signal (SIGINT, termHandler);
	
	while(1){
		mCom.acceptSocket(&recv_length, buffer, mSocket);
		
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
			
			answer(task_code, registry, data);
		}
		
		close(mSocket);
	}
}

void CServer::answer(int taskCode, int registry, uint64_t datacode){
	switch(taskCode){
		case DATA_REQ:{
			dataRequest(registry);
			break;
		}
		case CLIENT_REG:{
			registerClient(datacode);
			break;
		}
		case DATA_RES:{
			std::cout <<"illegal taskcode for client" << std::endl;
			break;
		}
		case TERM_COM:{
			terminate(registry);
			break;
		}
	}
}

void CServer::registerClient(uint64_t datacode){	
	int reg = ut::getReg(mRegClients, mMaxClients);		//find available registry
	clReg a = {reg, datacode};
	mRegClients.push_back(a);							//add to register
	std::string answer;
	
	mProtocol.answerRegisterMsg(answer, reg);
	mCom.sendMsg(answer, mSocket);
}

void CServer::dataRequest(int registry){
	
}

void CServer::terminate(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0)
		mRegClients.erase(mIterator);
}

void CServer::createDataAnswer(std::string& msg, uint64_t dataCode) {
	mProtocol.addVersion(msg, mVERSION);
	mProtocol.addCmd(msg, DATA_RES);
	
}

void CServer::termHandler(sig_t s) {
	std::cout << "terminating server..." << std::endl;
	exit(0);
}


