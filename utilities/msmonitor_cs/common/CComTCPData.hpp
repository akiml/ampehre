#ifndef __CCOMTCPDATA_HPP__
#define __CCOMTCPDATA_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>
#include <vector>

class CComTCPData {
	private:
		char *mMsg;
		ssize_t mMsgLength;
		ssize_t mMsgSize;
		
	public:
		int mSocketFildes;
		socklen_t mClientAddrLen;
		struct sockaddr_in mClientAddr;
        int mTaskCode;
        int mRegistry;
        uint64_t mData;
        bool mTermflag;
        void* mpSrv; //CServer

		
	public:
		CComTCPData();
		~CComTCPData();
		
        void setMsg(const char *pMsg);
        void setMsg(const void *pMsg, ssize_t size);
		char *getMsg(ssize_t *pMsgLength);
};

#endif /* __CCOMTCPDATA_HPP__ */
