#ifndef __CHTTPREQUEST_H__
#define __CHTTPREQUEST_H__
#include <vector>

class CHTTPRequest {

	public: enum RequestState {
		NOSTATE,
		HEADER,
		DONE,
		FAILED
	};

	public: enum RequestMethod {
		NOMETHOD,
		GET,
		POST,
		HEAD,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE,
		PATCH,
		UNKNOWN
	};
	

	RequestState mState = RequestState::NOSTATE;
	RequestMethod mMethod = RequestMethod::NOMETHOD;
	//std::vector<char*> mHeaders;

	public:
		char* mPath = 0;
		int mVersion = 0;
		int mCloseConnection = 0;

	public:
		~CHTTPRequest();
		int addLine(char* line);
		void clear();

};

#endif
