/*
 * CMeasureMICThread.hpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 4/7/15
 * version: 0.4.0 - MIC integration into libmeasure
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#ifndef __CMEASUREMICTHREAD_HPP__
#define __CMEASUREMICTHREAD_HPP__

#include "CMeasureMIC.hpp"

#include "../common/CMeasureAbstractThread.hpp"

#include <miclib.h>

namespace NLibMeasure {
	class CMeasureMICThread : public CMeasureAbstractThread {
		public:
			CMeasureMICThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes);
			~CMeasureMICThread();
			
		private:
			void run(void);
	};
}

#endif /* __CMEASUREMICTHREAD_HPP__ */
