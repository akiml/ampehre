#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <csignal>
#include <cstring>
#include "CHTTPClient.h"
#include "CHTTPServer.h"
#include "CWebMeasure.h"

CHTTPClient::CHTTPClient(int socket):
	mDone(0),
	mSocket(socket),
	mStop(0),
	mBufferPos(0)
{

	mBufferSize = 2048;
	mBuffer = new char[mBufferSize]();

	// start new thread after (!) buffer is ready
	mThread = std::thread(&CHTTPClient::readThread, this);

}

CHTTPClient::~CHTTPClient(){

	if (-1 != mSocket) {
		close(mSocket);
		mSocket = -1;
	}
	mStop = 1;

	if (0 == mDone) {
		int ret = kill(mPid, SIGINT);
		if (0 != ret) {
			std::cerr << strerror(errno) << std::endl;
		}
	}

	if (true == mThread.joinable()) {
		mThread.join();
	}

	if (0 != mBuffer) {
		delete[] mBuffer;
		mBuffer = 0;
	}
}

void CHTTPClient::advanceBuffer(long shift){
	// buffer content completly discarded
	if (mBufferPos == shift) {
		mBufferPos = 0;
		return;
	}
	long tocopy = mBufferPos - shift;
	// rest does not overlap
	if (tocopy <= shift) {
		memcpy(mBuffer, &(mBuffer[shift]), tocopy);
		mBufferPos = tocopy;
		return;
	}
	// enough space in buffer after pos
	if (mBufferSize-mBufferPos >= tocopy) {
		memcpy(&(mBuffer[mBufferPos]), &(mBuffer[shift]), tocopy);
		memcpy(mBuffer, &(mBuffer[mBufferPos]), tocopy);
		mBufferPos = tocopy;
		return;
	} else {
		// not enough space, need new memory
		char* temp = new char[mBufferSize]();
		memcpy(temp, &(mBuffer[shift]), tocopy);
		delete[] mBuffer;
		mBuffer = temp;
		mBufferPos = tocopy;
		return;
	}
}

void CHTTPClient::readThread(){

	mPid = syscall(SYS_gettid);

	int ret = 0;
	int error = 0;
	ssize_t read;
	long end = -1;
	long pos = -1;
	int found = 0;

	while(0 == mStop) {

		// try to process lines in buffer
		do {
			// find next line
			end = -1;
			for(pos = 0; pos < mBufferPos; pos++) {
				if (mBuffer[pos] == '\n') {
					end = pos;
					break;
				}
			}
			// line found
			if (end != -1) {
				{
					char temp[end+2];
					memcpy(&(temp[0]), mBuffer, end+1);
					temp[end+1] = 0;
					ret = mRequest.addLine(&(temp[0]));
				}
				if (ret < 0) {
					// HTTP invalid
					abortClient();
					break;
				}
				if (ret == 1) {
					// request done
					processRequest();
					mRequest.clear();
				}
				advanceBuffer(end+1);
				found = 1;
			} else {
				found = 0;
			}
		} while(1 == found);

		if (1 == mStop) {
			break;
		}

		if (mBufferPos == mBufferSize) {
			// buffer full and no lines found
			abortClient();
			break;
		}

		// get new data
		read = recv(mSocket, &(mBuffer[mBufferPos]), mBufferSize-mBufferPos, 0);
		if (0 == read) {
			// socket disconnected from the other side
			if (-1 != mSocket) {
				close(mSocket);
				mSocket = -1;
			}
			mStop = 1;
			break;
		}

		if (0 > read) {
			// error
			error = errno;
			std::cerr << strerror(error) << std::endl;
			if (error == EAGAIN || error == ETIMEDOUT || error == ENOBUFS || error == ENOMEM) {
				// try again
			} else {
				// bad, stop
				if (-1 != mSocket) {
					close(mSocket);
					mSocket = -1;
				}
				mStop = 1;
			}
			break;
		}

		mBufferPos += read;
	}

	mDone = 1;

}

const char* notfound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
const char* ok = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n";
const char* badrequest = "HTTP/1.1 400 Bad Request\r\nConnection: Close\r\n\r\n";

void CHTTPClient::abortClient(){

	int ret = 0;
	if (-1 != mSocket) {
		ret = send(mSocket, badrequest, strlen(badrequest), 0);
		if (-1 == ret) {
			std::cerr << strerror(errno) << std::endl;
		}
		ret = close(mSocket);
		if (-1 == ret) {
			std::cerr << strerror(errno) << std::endl;
		}
		mSocket = -1;
	}
	mStop = 1;

}

void CHTTPClient::processRequest(){


	if (0 == mRequest.mPath) {
		return;
	}
	if (0 == strcmp("/measure", mRequest.mPath)) {
//		char* resp = measure
		char* resp = measure->generateCSV();
		int length = strlen(resp);
		char header[100] = {};
		int ret = 0;
		snprintf(header, 99, "Content-length: %d\r\n\r\n", length);
		ret = send(mSocket, ok, strlen(ok), 0);
		if (-1 == ret) {
			std::cerr << strerror(errno) << std::endl;
		}
		ret = send(mSocket, header, strlen(header), 0);
		if (-1 == ret) {
			std::cerr << strerror(errno) << std::endl;
		}
		ret = send(mSocket, resp, length, 0);
		if (-1 == ret) {
			std::cerr << strerror(errno) << std::endl;
		}

	} else {
		int ret = send(mSocket, notfound, strlen(notfound), 0);
		if (-1 == ret) {
			std::cerr << strerror(errno) << std::endl;
		}
	}

	if (1 == mRequest.mCloseConnection || 10 == mRequest.mVersion) {
		if (-1 != mSocket) {
			close(mSocket);
			mSocket = -1;
		}
		mStop = 1;
	}

}
