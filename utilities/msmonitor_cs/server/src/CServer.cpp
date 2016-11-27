#include "CServer.hpp"

CServer::CServer(int port, int maxClients):
     mVERSION("0.1"),
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
	mMeasure.start();
	if(mCom.initSocket(mPort) < 0)
		exit(-1);
}


void CServer::acceptLoop() {
	int recv_length;
	char buffer[4096];
	int task_code = 0;
	int registry = 0;
	uint64_t data = 0;

	struct sigaction act;
	act.sa_handler = termHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, 0);

	while(1){
		mCom.acceptSocket(mSocket);

		recv_length = recv(mSocket, buffer, 4096, 0);
		
		std::cout<<"************************" << std::endl;
		std::cout<<"received: "<<std::endl;
		
		int i = 0;
		
		while(i < recv_length){
			std::cout << buffer[i];
			i++;
		}
		
		std::cout<<"************************" << std::endl;
		
		if(mProtocol.parseMsg(buffer, recv_length, task_code, registry, data) < 0){
			std::cout << "[!]error parsing message" << std::endl;
		}
		else{
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
	if(ut::find(mRegClients, registry, mIterator) == 0){
		std::string msg;
		createDataAnswer(msg, mIterator->dataCode);
		mCom.sendMsg(msg, mSocket);
	}
	else{
		std::cout<<"client not registered yet!" << std::endl;
	}
}

void CServer::terminate(int registry){
	if(ut::find(mRegClients, registry, mIterator) == 0){
		mRegClients.erase(mIterator);
		std::string msg;
		mProtocol.termComMsg(msg, registry);
		mCom.sendMsg(msg, mSocket);
	}
}

void CServer::createDataAnswer(std::string& msg, uint64_t dataCode) {
	mProtocol.addCmdVersion(msg, DATA_RES, mVERSION);
	std::vector<int> d;
	std::vector<double> values;

	mProtocol.extractData(d, dataCode);		//extract wanted data from 64Bit dataCode
	mMeasure.getValues(values, d);			//read needed values into double vector

	for(unsigned int i = 0; i < values.size(); i++){
		mProtocol.addData(msg, values[i]);		//write all data values 
	}
}

void CServer::termHandler(int s) {
	std::cout << "terminating server..." << std::endl;
	exit(0);
}
