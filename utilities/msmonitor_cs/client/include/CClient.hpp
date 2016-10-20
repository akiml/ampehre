#ifndef CCLIENT_HPP
#define CCLIENT_HPP

#include "CComC.hpp"
#include "CProtocolC.hpp"

class CClient{
public:
	CClient();
	~CClient();
	
private:
	CComC* mCom;
	CProtocolC* mProtocol;
	
};



#endif