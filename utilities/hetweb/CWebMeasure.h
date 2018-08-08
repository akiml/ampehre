#ifndef __CWEBMEASURE_H__
#define __CWEBMEASURE_H__
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ms_measurement.h"

class CWebMeasure;

extern CWebMeasure* measure;

class CWebMeasure {

	public: struct measure_data {
		struct timespec time;
		MS_MEASUREMENT_CPU cpu;
		MS_MEASUREMENT_GPU gpu;
		MS_MEASUREMENT_FPGA fpga;
		MS_MEASUREMENT_MIC mic;
		MS_MEASUREMENT_SYS sys;
	};

	std::thread mThread;
	                
	MS_VERSION *mpVersion;
	MS_SYSTEM *mpMSystem;
	MS_LIST *mpMList;
	struct measure_data* mBuffer;
	int mBufferSize = 0;
	int mBufferPos = 0;
	long mCount = 0;
	char* mCSV = 0;
	int mStop = 0;
	int mStart = 0;
	std::timed_mutex mMutex;
	std::condition_variable_any mMutexCond;
	long mIntervalNs = 0;

	private:
		void intervalThread();

	public:
		CWebMeasure(long intervalNs, int bufferSize);
		~CWebMeasure();
		void startMeasurement();
		void stopMeasurement();
		char* generateCSV();
		char* generateJsonCSVBulk();

};

#endif
