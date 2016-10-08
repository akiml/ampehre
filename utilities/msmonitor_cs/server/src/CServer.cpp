#include <CServer.hpp>

CServer::CServer(){}

CServer::~CServer(){}

int CServer::init(){
	mMeasure->init();
	mCom->initSocket();
}