/*
 * CLogger.hpp
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

#ifndef __CLOGGER_HPP__
#define __CLOGGER_HPP__

#include "CMutex.hpp"
#include <iostream>

namespace NLibMeasure {
	class CLogger {
		private:
			CMutex mMutex;
			
		public:
			const static int scOut;
			const static int scErr;
			
		public:
			CLogger();
			~CLogger();
			
			void lock(void);
			void unlock(void);
			void flush(void);
			std::ostream& operator()(int stream_fildes = scOut);
	};
}

#endif /* __CLOGGER_HPP__ */
