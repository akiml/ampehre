#ifndef CCLIENT_HPP
#define CCLIENT_HPP

#include <vector>
#include "CComC.hpp"
#include "CProtocolC.hpp"


class CClient{
public:
	CClient();
	~CClient();

	int registerToServer(std::vector<int>& values);
	void requestData(std::vector<int>& values);
	void terminate();
	
private:
	CComC* mCom;
	CProtocolC* mProtocol;
	int mSocket;
	int mReg;
	std::string mVERSION;
	
};



#endif