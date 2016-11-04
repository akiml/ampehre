#ifndef CSERVER_HPP
#define CSERVER_HPP

#include "CComS.hpp"
#include "CMeasure.hpp"
#include "CProtocolS.hpp"
#include <list>


class CServer{

public:
	CServer(int port);
	~CServer();
	void init();
	void acceptLoop();
	
private:
	void answer(int taskCode, int registry, uint64_t datacode);
	void registerClient(uint64_t datacode);
	void dataRequest(int registry);
	void terminate(int registry);
	
	std::string mVERSION;
	CMeasure mMeasure;
	CComS mCom;
	CProtocolS mProtocol;
	std::list<int> mRegClients;
	int mPort;
	
};

#endif