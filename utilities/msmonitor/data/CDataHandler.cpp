/*
 * CDataHandler.cpp
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

#include "CDataHandler.hpp"

namespace NData {
	CDataHandler::CDataHandler(void) :
		mSettings(),
		mMeasurement(mSettings),
		mLibrary(mSettings, mMeasurement),
		mCollectData(false)
		{
		
		// nothing todo
	}

	CDataHandler::~CDataHandler(void) {
		// nothing todo
	}
	
	CDataSettings &CDataHandler::getSettings(void) {
		return mSettings;
	}
	
	CDataMeasurement &CDataHandler::getMeasurement(void) {
		return mMeasurement;
	}
	
	void CDataHandler::startCollectData(void) {
		mLibrary.start();
		mCollectData = true;
	}
	
	void CDataHandler::stopCollectData(void) {
		if (mCollectData) {
			mCollectData = false;
			mLibrary.stop();
			mLibrary.join();
		}
	}
	
	void CDataHandler::resetCollectData(void) {
		mMeasurement.reset();
	}
}
