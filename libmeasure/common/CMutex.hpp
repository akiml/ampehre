/*
 * CMutex.hpp
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
 */

#ifndef __CMUTEX_HPP__
#define __CMUTEX_HPP__

#include <pthread.h>

namespace NLibMeasure {
	class CMutex {
		private:
			pthread_mutex_t mMutexID;
			
		public:
			CMutex();
			~CMutex();
			
			void lock(void);
			void unlock(void);
	};
}

#endif /* __CMUTEX_HPP__ */
