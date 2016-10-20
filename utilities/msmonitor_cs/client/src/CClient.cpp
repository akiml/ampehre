#include "CClient.hpp"

CClient::CClient() {
	mCom = new CComC();
 	mProtocol = new CProtocolC();

}

CClient::~CClient() {

}
