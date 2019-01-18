#include <chrono>
#include <ctime>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "CCURLUpload.h"

CCURLUpload::CCURLUpload(CWebMeasure& rMeasure, const char* pURL, long intervalSendMs):
	mrMeasure(rMeasure),
	mIntervalMs(intervalSendMs),
	mpURL(pURL)
{

	// clean CURL error buffer
	memset(mpCurlErrorBuffer, 0, CURL_ERROR_SIZE+1);
}

CCURLUpload::~CCURLUpload(){
}

int CCURLUpload::start(){

	CURLcode curlret = CURLE_OK;

	curlret = curl_global_init(CURL_GLOBAL_ALL);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL init failed" << std::endl;
		return -1;
	}

	// get CURL handle
	mpCurlHandle = curl_easy_init();
	if (0 == mpCurlHandle) {
		std::cerr << "CURL handle creation failed" << std::endl;
		return -1;
	}

	// set url
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_URL, mpURL);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting URL failed" << std::endl;
		return -1;
	}

	// set POST method
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_POST, 1);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting POST method failed" << std::endl;
		return -1;
	}

	// set Content-Type header
	mpCurlHeaders = curl_slist_append(mpCurlHeaders,
		"Content-Type: application/x-ndjson");
	mpCurlHeaders = curl_slist_append(mpCurlHeaders,
		"User-Agent: hetwebpush");
	mpCurlHeaders = curl_slist_append(mpCurlHeaders,
		"Expect:");
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_HTTPHEADER, mpCurlHeaders);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting header failed" << std::endl;
		return -1;
	}

	// set Write function
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_WRITEFUNCTION, &CCURLUpload::write);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting write function failed" << std::endl;
		return -1;
	}

	// set Upload function
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_READFUNCTION, &CCURLUpload::upload);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting read function failed" << std::endl;
		return -1;
	}

	// set Upload data
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_READDATA, &mUploadData);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting read data failed" << std::endl;
		return -1;
	}

	// set Xfer function
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_PROGRESSFUNCTION, &CCURLUpload::progress);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting progress function failed" << std::endl;
		return -1;
	}

	// set Xfer data
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_PROGRESSDATA, this);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting progress data failed" << std::endl;
		return -1;
	}

	// activate progress callback calls
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_NOPROGRESS, 0);
	if (CURLE_OK != curlret) {
		std::cerr << "CURL activating progress calls failed" << std::endl;
		return -1;
	}

	// set error buffer
	curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_ERRORBUFFER, mpCurlErrorBuffer); 
	if (CURLE_OK != curlret) {
		std::cerr << "CURL setting error buffer failed" << std::endl;
		return -1;
	}

	// intialize thread
	mThread = std::thread(&CCURLUpload::intervalThread, this);

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		mStart = 1;
		mMutexCond.notify_one();
	}

	return 0;
}

void CCURLUpload::intervalThread(){

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		while (0 == mStart && 0 == mStop) {
			mMutexCond.wait(ul);
		}
	}
	if (1 == mStop) {
		return;
	}

	std::chrono::system_clock::time_point wakeup_time = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point now_time;

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		std::cv_status waitTimeout;
		while(0 == mStop) {

			// compute next wakeup time
			now_time = std::chrono::system_clock::now();
			do {
				wakeup_time += std::chrono::milliseconds(mIntervalMs);
			} while(wakeup_time < now_time);


			// wait for next wakeup
			waitTimeout = mMutexCond.wait_until(ul, wakeup_time);

			// check if thread should be stopped
			if (std::cv_status::no_timeout == waitTimeout || 1 == mStop) {
				break;
			}

			// prepare data

			// send data

			char* json_data = mrMeasure.generateJsonCSVBulk();
			long json_length = strlen(json_data);

			if (json_length <= 1) {
				continue;
			}

			mUploadData.data = json_data;
			mUploadData.size = json_length;
			mUploadData.written = 0;

			// set content length
			CURLcode curlret = CURLE_OK;
			//curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_INFILESIZE, json_length);
			curlret = curl_easy_setopt(mpCurlHandle, CURLOPT_POSTFIELDSIZE, json_length);
			if (CURLE_OK != curlret) {
				std::cerr << "CURL setting content length failed" << std::endl;
				return;
			}

			int transfer_error = 0;
			int http_error = 0;

			CURLcode curlret_transfer = CURLE_OK;
			curlret_transfer = curl_easy_perform(mpCurlHandle);

			if (CURLE_OK != curlret_transfer) {
				transfer_error = 1;
			}

			// get transfer information
			long http_response = 0;
			curlret = CURLE_OK;
			curlret = curl_easy_getinfo(mpCurlHandle, CURLINFO_RESPONSE_CODE, &http_response);
			if (CURLE_OK != curlret) {
				http_response = 0;
			}

			if (200 != http_response && 0 != http_response) {
				http_error = 1;
			}


			// print time
			if (1 == transfer_error || 1 == http_error) {
				time_t wakeup_time_t = std::chrono::system_clock::to_time_t(wakeup_time);
				char timebuffer[25];
				memset(timebuffer, 0, 25);
				struct tm wakeup_time_tm = {};
				localtime_r(&wakeup_time_t, &wakeup_time_tm);
				strftime(timebuffer, 24, "%Y.%m.%d %X", &wakeup_time_tm);
				std::cerr << timebuffer << std::endl;
			}

			// print error
			if (1 == transfer_error) {
				std::cerr << curl_easy_strerror(curlret_transfer) << std::endl;
				std::cerr << mpCurlErrorBuffer << std::endl;
			}

			// print HTTP response code
			if (1 == http_error) {
				std::cerr << "HTTP response: " << http_response << std::endl;
			}
			
			if (1 == mStop) {
				break;
			}
		}
	}

}

void CCURLUpload::stop(){

	if (0 == mStop) {
		mStop = 1;
		{
			std::unique_lock<std::timed_mutex> ul(mMutex);
			mMutexCond.notify_one();
		}
	}   
	if (mThread.joinable()) {
		mThread.join();
	}

	if (0 != mpCurlHeaders) {
		curl_slist_free_all(mpCurlHeaders);
		mpCurlHeaders = 0;
	}

	if (0 != mpCurlHandle) {
		curl_easy_cleanup(mpCurlHandle);
		mpCurlHandle = 0;
	}

	curl_global_cleanup();

}

size_t CCURLUpload::upload(char *bufptr, size_t size, size_t nitems, void *userp) {

	// get user data
	struct CCURLUpload_data* data = (struct CCURLUpload_data*) userp;

	// compute number of bytes to write
	long remaining = data->size-data->written;
	long towrite = size * nitems;
	if (remaining < towrite) {
		towrite = remaining;
	}

	// write to buffer
	char* src = &(data->data[data->written]);
	memcpy(bufptr, src, towrite);
	data->written += towrite;

	return towrite;
}

int CCURLUpload::progress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {

	CCURLUpload* upload = (CCURLUpload*) clientp;
	if (1 == upload->mStop) {
		return -1;
	}
	return 0;

}


size_t CCURLUpload::write(char *ptr, size_t size, size_t nmemb, void *userdata) {

	// drop received data
	return nmemb;

}
