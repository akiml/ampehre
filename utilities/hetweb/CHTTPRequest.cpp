#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include "CHTTPRequest.h"

CHTTPRequest::~CHTTPRequest(){
	clear();
}

void CHTTPRequest::clear(){

	mState = RequestState::NOSTATE;
	mMethod = RequestMethod::NOMETHOD;

	mVersion = 0;

	if (0 != mPath) {
		free(mPath); // allocated by sscanf
		mPath = 0;
	}
	mCloseConnection = 0;

//	while (false == mHeaders.empty()) {
//		delete mHeaders.back();
//		mHeaders.pop_back();
//	}

}

int CHTTPRequest::addLine(char* line) {


	switch (mState) {
		case RequestState::NOSTATE:
		{
			// check method line

			char* method = 0;
			char* path = 0;
			char* version = 0;



			int ret;
			int error;
			ret = sscanf(line, "%m[A-Z] %m[^ ] %m[^\r\n]\r\n", &method, &path, &version);
			if (3 != ret) {
				error = errno;
				if (error == 0) {
				}
				if (0 != method) {
					free(method);
				}
				if (0 != path) {
					free(path);
				}
				if (0 != version) {
					free(version);
				}
				mState = RequestState::FAILED;
				return -1;
			}

			// check method
			if (0 == strncmp(method, "GET", 3)) {
				mMethod = RequestMethod::GET;
			} else
			if (0 == strncmp(method, "POST", 4)) {
				mMethod = RequestMethod::POST;
			} else
			if (0 == strncmp(method, "HEAD", 4)) {
				mMethod = RequestMethod::HEAD;
			} else
			if (0 == strncmp(method, "PUT", 3)) {
				mMethod = RequestMethod::PUT;
			} else
			if (0 == strncmp(method, "DELETE", 6)) {
				mMethod = RequestMethod::DELETE;
			} else
			if (0 == strncmp(method, "CONNECT", 7)) {
				mMethod = RequestMethod::CONNECT;
			} else
			if (0 == strncmp(method, "OPTIONS", 7)) {
				mMethod = RequestMethod::OPTIONS;
			} else
			if (0 == strncmp(method, "TRACE", 5)) {
				mMethod = RequestMethod::TRACE;
			} else
			if (0 == strncmp(method, "PATCH", 5)) {
				mMethod = RequestMethod::PATCH;
			} else {
				mMethod = RequestMethod::UNKNOWN;
			}
			free(method);

			mPath = path;

			if (0 == strncmp(version, "HTTP/1.0", 8)) {
				mVersion = 10;
			} else
			if (0 == strncmp(version, "HTTP/1.1", 8)) {
				mVersion = 11;
			} else {
				mVersion = -1;
			}
			free(version);
			mState = RequestState::HEADER;
			return 0;
		}
		break;

		case RequestState::HEADER:

			if (0 == strcmp(line, "Connection: close")) {
				mCloseConnection = 1;
			} else
			if (0 == strcmp(line, "\r\n")) {
				mState = DONE;
				return 1;
			}
			// ignore header at this point
			return 0;
		break;

		case RequestState::FAILED:
			return -1;
		break;

		case RequestState::DONE:
		break;
	}

	return -1;
}
