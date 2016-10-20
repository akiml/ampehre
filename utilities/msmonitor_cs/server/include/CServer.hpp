#ifndef CSERVER_HPP
#define CSERVER_HPP

#include "CComS.hpp"
#include "CMeasure.hpp"
#include "CProtocolS.hpp"
#include <list>


class CServer{

public:
	CServer();
	~CServer();
	void init();
	void acceptLoop();
	
private:
	int mSockfd;
	std::string mVERSION;
	CMeasure* mMeasure;
	CComS* mCom;
	CProtocolS* mProtocol;
	
	std::list<int> mRegClients;
	
};

#endif