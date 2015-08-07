/*
 * CThread.cpp
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

#include "CThread.hpp"

#include <iostream>

namespace NLibMeasure {
	int CThread::sNumOfThreads = 0;
	
	CThread::CThread() {
		// nothing todo
	}
	
	CThread::~CThread() {
		// nothing todo
	}
}
