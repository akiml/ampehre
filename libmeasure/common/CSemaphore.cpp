/*
 * CSemaphore.cpp
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

#include "CSemaphore.hpp"
#include <iostream>

namespace NLibMeasure {
	CSemaphore::CSemaphore() {
		sem_init(&mSemID, 0, 0);
	}
	
	CSemaphore::~CSemaphore() {
		sem_destroy(&mSemID);
	}
	
	void CSemaphore::post(int count) {
		for(int i = 0; i < count; i++){
			sem_post(&mSemID);
		}
	}
	
	void CSemaphore::wait(void) {
		sem_wait(&mSemID);
	}
}
