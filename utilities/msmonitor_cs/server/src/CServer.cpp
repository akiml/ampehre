#include "CServer.hpp"

CServer::CServer(){}

CServer::~CServer(){}

int CServer::init(){
	//mMeasure->init();
	mSockfd = mCom->initSocket();
	mCom->acceptSocket(mSockfd);
	
	return 0;
}