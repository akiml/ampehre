/*
 * CDataHandler.hpp
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

#ifndef __CDATAHANDLER_HPP__
#define __CDATAHANDLER_HPP__

#include "CDataSettings.hpp"
#include "CDataMeasurement.hpp"
#include "CDataLibrary.hpp"

namespace NData {
	class CDataHandler {
		private:
			CDataSettings mSettings;
			CDataMeasurement mMeasurement;
			CDataLibrary mLibrary;
			
			bool mCollectData;
			
		public:
			CDataHandler(void);
			~CDataHandler(void);
			
			CDataSettings& getSettings(void);
			CDataMeasurement& getMeasurement(void);
			
			void startCollectData(void);
			void stopCollectData(void);
			void resetCollectData(void);
	};
}

#endif /* __CDATAHANDLER_HPP__ */
