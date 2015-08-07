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

#include "CLogger.hpp"

#include <cstdio>

namespace NLibMeasure {
	const int CLogger::scOut = 1;
	const int CLogger::scErr = 2;
	
	CLogger::CLogger() :
		mMutex()
		{
		// nothing todo
	}
	
	CLogger::~CLogger() {
		// nothing todo
	}
	
	void CLogger::lock(void) {
		mMutex.lock();
	}
	
	void CLogger::unlock(void) {
		mMutex.unlock();
	}
	
	void CLogger::flush(void) {
		std::cout.flush();
		std::cerr.flush();
	}
	
	std::ostream& CLogger::operator()(int stream_fildes) {
		std::ostream& stream = (stream_fildes == 1) ? std::cout : std::cerr;
		
		return stream;
	}
}
