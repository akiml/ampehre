/*
 * CMutex.cpp
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
 * created: 1/26/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 */

#include "CMutex.hpp"

namespace NCommon {
	CMutex::CMutex(void) {
		pthread_mutex_init(&mMutexID, 0);
	}
	
	CMutex::~CMutex(void) {
		pthread_mutex_destroy(&mMutexID);
	}
	
	void CMutex::lock(void) {
		pthread_mutex_lock(&mMutexID);
	}
	
	void CMutex::unlock(void) {
		pthread_mutex_unlock(&mMutexID);
	}
}
