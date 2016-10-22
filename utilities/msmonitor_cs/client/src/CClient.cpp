#include "CClient.hpp"

CClient::CClient() {
	mVERSION = "MSMv0.1";
	mCom = new CComC();
 	mProtocol = new CProtocolC(mVERSION);
	mSocket = 0;

}

CClient::~CClient() {

}

int CClient::registerToServer(std::vector<int>& values) {
	mSocket = mCom->initSocket();
	
	char rep[2048];
	unsigned int rep_len = 0;
	bool bReg = false;
	int reg, tsk;
	
	uint64_t code = mProtocol->createDataCode(values);
	std::string msg = mProtocol->regMsg(code);
	const char *cpMsg = msg.c_str();
	std::vector<double> ignore;
	
	mCom->communicate(cpMsg ,rep, rep_len, mSocket);
	mProtocol->parseMsg(rep, rep_len, bReg, &reg, &tsk, ignore);
	
	if(bReg == true){
		this->mReg = reg;
		return 0;
	}
	else 
		return -1;
}

void CClient::requestData(std::vector< int >& values) {
	//request data using registry
}

void CClient::terminate() {
	//send TERM signal
}

