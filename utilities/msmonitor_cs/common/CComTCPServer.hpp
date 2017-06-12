#ifndef __CCOMTCPSERVER_H__
#define __CCOMTCPSERVER_H__

#include "CComTCPAbstract.hpp"
#include <string.h>

class CComTCPServer : public CComTCPAbstract
{
public:
    CComTCPServer(int port);
    ~CComTCPServer();
	
	void msmSetSockOpt();
	void msmBind();
	void msmListen();
	void msmAccept(CComTCPData **pComData);

	void msmShutdown(CComTCPData **pComData);

};

#endif // __CCOMTCPSERVER_H__
