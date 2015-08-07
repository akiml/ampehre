/*
 * CThread.hpp
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
 * created: 3/03/14
 * version: 0.1.0 - initial implementation
 * 
 */

#ifndef __CTHREAD_HPP__
#define __CTHREAD_HPP__

#include <pthread.h>
#include "CMutex.hpp"

namespace NLibMeasure {
	class CThread {
		public:
			static int sNumOfThreads;
			
		public:
			CThread();
			~CThread();
			
		protected:
			virtual void run(void) = 0;
			virtual void exit(void) = 0;
			
		public:
			virtual void start(CMutex *pMutexStart = 0) = 0;
			virtual void stop(void) = 0;
			virtual void join(void) = 0;
	};
}

#endif /* __CTHREAD_HPP__ */
