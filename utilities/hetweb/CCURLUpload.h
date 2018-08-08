#ifndef __CCURLUPLOAD_H__
#define __CCURLUPLOAD_H__
#include <thread>
#include <mutex>
#include <condition_variable>
#include <curl/curl.h>
#include "CWebMeasure.h"

struct CCURLUpload_data {

	char* data;
	long size;
	long written;

};

class CCURLUpload {


	private:
		CWebMeasure& mrMeasure;
		long mIntervalMs = 0;
		const char* mpURL = 0;
		CURL* mpCurlHandle = 0;
		struct curl_slist* mpCurlHeaders = 0;
		struct CCURLUpload_data mUploadData;
		char mpCurlErrorBuffer[CURL_ERROR_SIZE+1];
		std::thread mThread;
		std::timed_mutex mMutex;
		std::condition_variable_any mMutexCond;
		int mStop = 0;
		int mStart = 0;


	private:
		static size_t upload(char *bufptr, size_t size, size_t nitems, void *userp);
		static int progress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
		static size_t write(char *ptr, size_t size, size_t nmemb, void *userdata);

		void intervalThread();

	public:
		int start();
		void stop();
		CCURLUpload(CWebMeasure& rMeasure, const char* pURL, long intervalSendMs);
		~CCURLUpload();

};

#endif
