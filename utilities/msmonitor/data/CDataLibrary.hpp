/*
 * CDataLibrary.hpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 1/27/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularised measurement struct
 */

#ifndef __CDATALIBRARY_HPP__
#define __CDATALIBRARY_HPP__

#include <ctime>
#include <fstream>

#include "../common/CThread.hpp"
#include "../common/CMutex.hpp"

#include "CDataTimer.hpp"
#include "CDataSettings.hpp"
#include "CDataMeasurement.hpp"

#include "../../../include/ms_measurement.h"
#include "../../../include/ms_version.h"

namespace NData {
	class CDataLibrary : public NCommon::CThread {
		private:
			static uint32_t sNumberOfCSVs;
			
			pthread_t mThreadID;
			bool mThreadStateRun;
			bool mThreadStateStop;
			
			CDataMeasurement& mrMeasurement;
			CDataSettings& mrSettings;
			
			NCommon::CMutex mMutexTimer;
			CDataTimer mTimer;
			
			MS_VERSION *mpMSVersion;
			MS_SYSTEM *mpMSSystem;
			MS_LIST *mpMSMeasurement;
			
			struct timespec mTimeStart;
			struct timespec mTimeCurrent;
			struct timespec mTimeDiff;
			
		public:
			CDataLibrary(CDataSettings& rSettings, CDataMeasurement& rMeasurement);
			~CDataLibrary(void);
			
		private:
			void initMSSystem(void);
			void startMSSystem(void);
			void stopMSSystem(void);
			void freeMSSystem(void);
			void printHeader(std::ofstream& rCsvFile);
			void printValues(std::ofstream& rCsvFile);
			
		protected:
			void run(void);
			void exit(void);
			
		public:
			void start(void);
			void stop(void);
			void join(void);
			
		private:
			static void* startThread(void* pThreadObject);
			double calcTimeSinceStart(void);
	};
}

#endif /* __CDATALIBRARY_HPP__ */
