/*
 * CMeasureNVMLThread.hpp
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
 * created: 3/04/14
 * version: 0.1.0 - initial implementation
 *          0.1.4 - add signals for measuring system start and stop
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.1 - modularised libmeasure
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#ifndef __CMEASURENVMLTHREAD_HPP__
#define __CMEASURENVMLTHREAD_HPP__

#include "../common/CMeasureAbstractThread.hpp"

#include "CMeasureNVML.hpp"

#include <nvml.h>

namespace NLibMeasure {
	class CMeasureNVMLThread : public CMeasureAbstractThread {
		public:
			CMeasureNVMLThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes);
			~CMeasureNVMLThread();
			
		private:
			void run(void);
	};
}

#endif /* __CMEASURENVMLTHREAD_HPP__ */
