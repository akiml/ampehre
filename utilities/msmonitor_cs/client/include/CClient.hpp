#ifndef CCLIENT_HPP
#define CCLIENT_HPP

#include <vector>
#include "CComC.hpp"
#include "CProtocolC.hpp"


class CClient{
public:
	CClient();
	~CClient();

	int registerToServer(std::vector<int>& values, int port, std::string ipaddr);
	void requestData();
	void terminate();
	
private:
	void initSocket();
	std::string mVERSION;
	CComC mCom;
	CProtocolC mProtocol;
	int mSocket;
	int mReg;
	int mPort;
	int mAmtValues;
	std::vector<double> mValues;
	std::string mIPaddr;
	
};



#endif