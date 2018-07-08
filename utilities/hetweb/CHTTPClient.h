#ifndef __CHTTPCLIENT_H__
#define __CHTTPCLIENT_H__
#include <thread>
#include "CHTTPRequest.h"

class CHTTPClient {

	public:
		int mDone;

	private:
		int mSocket;
		long mBufferSize = 0;
		char* mBuffer = 0;
		int mStop;
		long mBufferPos;
		std::thread mThread;
		int mPid = 0;
		CHTTPRequest mRequest;


	private:
		void readThread();
		void advanceBuffer(long shift);
		void abortClient();
		void processRequest();


	public:
		CHTTPClient(int socket);
		~CHTTPClient();

};

#endif
