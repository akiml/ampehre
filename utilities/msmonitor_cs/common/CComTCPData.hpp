#ifndef __CCOMTCPDATA_HPP__
#define __CCOMTCPDATA_HPP__

#include <sys/types.h>

class CComTCPData {
	private:
		char *mMsg;
		ssize_t mMsgLength;
		ssize_t mMsgSize;
		
	public:
		int mSocketFildes;
		socklen_t mClientAddrLen;
		struct sockaddr_in mClientAddr;
		
	public:
		CComTCPData();
		~CComTCPData();
		
		void setMsg(char *pMsg);
		char *getMsg(ssize_t *pMsgLength);
};

#endif /* __CCOMTCPDATA_HPP__ */
