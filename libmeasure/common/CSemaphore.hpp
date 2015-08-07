/*
 * CSemaphore.hpp
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 11/11/14
 * version: 0.2.2 - add semaphore to synchronize the start of the measurements
 */

#ifndef __CSEMAPHORE_HPP__
#define __CSEMAPHORE_HPP__

#include <semaphore.h>

namespace NLibMeasure {
	class CSemaphore {
		private:
			sem_t mSemID;
			
		public:
			CSemaphore();
			~CSemaphore();
			
			void post(int count);
			void wait(void);
	};
}

#endif /* __CSEMAPHORE_HPP__ */
