#include "CComTCPData.hpp"

#include <iostream>
#include <cstring>
#include <cstdlib>

CComTCPData::CComTCPData() :
	mSocketFildes(-1),
	mClientAddrLen(0),
	mMsg(NULL),
	mMsgLength(0),
	mMsgSize(4096)
	{
	
	memset(&mClientAddr, 0, sizeof(struct sockaddr_in));
	mClientAddrLen	= sizeof(struct sockaddr_in);
	
	mMsg = (char *)malloc(mMsgSize);
	if (NULL == mMsg) {
		std::cout << "ERROR: " << "Could not allocate memory." << std::endl;
		exit(EXIT_FAILURE);
	}
}

CComTCPData::~CComTCPData() {
	free(mMsg);
}

void CComTCPData::setMsg(char *pMsg) {
	mMsgLength = strlen(pMsg)+1;
	
	if (mMsgLength > mMsgSize) {
		mMsg		= (char *)realloc((void *)mMsg, mMsgLength);
		
		if (NULL == mMsg) {
			std::cout << "ERROR: " << "Could not allocate memory." << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mMsgSize	= mMsgLength;
	}
}

char* CComTCPData::getMsg(ssize_t *pMsgLength) {
	return mMsg;
}
