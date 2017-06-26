#include "CComTCPData.hpp"

#include <iostream>
#include <cstring>
#include <cstdlib>

CComTCPData::CComTCPData() :
	mMsg(NULL),
	mMsgLength(0),
    mMsgSize(4095),
	mSocketFildes(-1),
	mClientAddrLen(0),
    mTaskCode(0),
    mRegistry(0),
    mData(0),
    mTermflag(false),
    mpSrv(NULL)
{
	
	memset(&mClientAddr, 0, sizeof(struct sockaddr_in));
	mClientAddrLen	= sizeof(struct sockaddr_in);
	
	mMsg = (char *)malloc(mMsgSize+1);
	if (NULL == mMsg) {
		std::cout << "ERROR: " << "Could not allocate memory." << std::endl;
		exit(EXIT_FAILURE);
	}
}

CComTCPData::~CComTCPData() {
	free(mMsg);
}

//funktioniert nicht richtig
void CComTCPData::setMsg(const char *pMsg) {
	memset(mMsg, 0, mMsgSize+1);
	
	mMsgLength = strlen(pMsg)+1;
	
	if (mMsgLength > mMsgSize) {
		mMsg		= (char *)realloc((void *)mMsg, mMsgLength+1);
		
		if (NULL == mMsg) {
            std::cout << "ERROR: Could not allocate memory." << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mMsgSize	= mMsgLength;
	}

    memcpy(mMsg, pMsg, mMsgLength);
}

void CComTCPData::setMsg(const void *pMsg, ssize_t size){
	memset(mMsg, 0, mMsgSize+1);
	
    mMsgLength = size;

    if (mMsgLength > mMsgSize) {
        mMsg		= (char *)realloc((void *)mMsg, mMsgLength+1);

        if (NULL == mMsg) {
            std::cout << "ERROR: Could not allocate memory." << std::endl;
            exit(EXIT_FAILURE);
        }

        mMsgSize	= mMsgLength;
    }

    memcpy(mMsg, pMsg, mMsgLength);
}

char* CComTCPData::getMsg(ssize_t *pMsgLength) {
    *pMsgLength = mMsgLength;
	return mMsg;
}

