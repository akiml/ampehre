/*
 * CAbstractTimer.hpp
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
 * created: 1/28/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 */

#ifndef __CABSTRACTTIMER_HPP__
#define __CABSTRACTTIMER_HPP__

#include <ctime>

#include "CThread.hpp"
#include "CMutex.hpp"

// TODO: This is a non-C++ hack!
#include <stdint.h>

namespace NCommon {
	class CAbstractTimer : public CThread {
		protected:
			uint32_t mSamplingRate;
			
			pthread_t mThreadID;
			bool mThreadStateRun;
			bool mThreadStateStop;
			
			struct timespec mTimer;
			
		public:
			CAbstractTimer(uint32_t samplingRate);
			~CAbstractTimer(void);
			
		protected:
			void run(void) = 0;
			void exit(void);
			
		public:
			void start(void) = 0;
			void stop(void);
			void join(void);
	};
}

#endif /* __CABSTRACTTIMER_HPP__ */
