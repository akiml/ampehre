#ifndef CSERVER_HPP
#define CSERVER_HPP

#include "CComS.hpp"
#include "CMeasure.hpp"
#include "CProtocolS.hpp"
#include "utils.h"
#include <list>
#include <signal.h>
#include <stdlib.h>

class CServer{

public:
	CServer(int port, int maxClients);
	~CServer();
	void init();
	void acceptLoop();
	
private:
	void answer(int taskCode, int registry, uint64_t datacode);
	void registerClient(uint64_t datacode);
	void dataRequest(int registry);
	void terminate(int registry);
	void createDataAnswer(std::string &msg, uint64_t dataCode);
	static void termHandler(int s);
	
	std::string mVERSION;
	CMeasure mMeasure;
	CComS mCom;
	CProtocolS mProtocol;
	std::list<clReg> mRegClients;
	std::list<clReg>::iterator mIterator;
	int mPort;
	int mMaxClients;
	int mSocket;
	
};

#endif
