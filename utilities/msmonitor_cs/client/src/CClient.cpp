#include "CClient.hpp"

CClient::CClient():
    mVERSION("0.1"),
    mCom(CComC()),
    mProtocol(CProtocolC(mVERSION)),
    mSocket(0),
    mReg(0)
{
	std::cout << "Client Version: " << mVERSION << std::endl;
}

CClient::~CClient() {

}

int CClient::registerToServer(std::vector< int >& values, int port, std::string ipaddr) {
	mIPaddr = ipaddr;
	mPort = port;
	mAmtValues = values.size();
	
	initSocket();
	
	char rep[2048];
	unsigned int rep_len = 0;
	int reg = -1, tsk;
	
	uint64_t code = mProtocol.createDataCode(values);
	std::string msg = mProtocol.regMsg(code);
	std::vector<double> ignore;
	
	mCom.communicate(msg.c_str() ,rep, rep_len);
	mProtocol.parseMsg(rep, rep_len, reg, tsk, ignore);
	
	if(reg >= 0){
		this->mReg = reg;
		return 0;
	}
	else 
		return -1;
}

void CClient::requestData() {
	initSocket();
	char rep[8192] = {0};
	unsigned int rep_len = 0;
	int reg = -1, tsk;
	
	std::string msg = mProtocol.requestMsg(mReg);
	mCom.communicate(msg.c_str(), rep, rep_len);
	mProtocol.parseMsg(rep, rep_len, reg, tsk, mValues);
	
}

void CClient::terminate() {
	initSocket();
	char rep[4096];
	unsigned int rep_len = 0;

	std::string msg = mProtocol.termMsg(mReg);

	mCom.communicate(msg.c_str(), rep, rep_len);
}

void CClient::initSocket(){
	mCom.setAddr(mIPaddr);
	mSocket = mCom.initSocket(mPort);
}
