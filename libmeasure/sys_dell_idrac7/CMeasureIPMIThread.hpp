/*
 * CMeasureIPMIThread.hpp
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
 * created: 3/20/14
 * version: 0.1.0 - initial implementation
 *          0.1.4 - add signals for measuring system start and stop
 *          0.2.2 - add semaphore to synchronize the start of the measurements
 *          0.5.2 - delete different ThreadTimer classes in libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 */

#ifndef __CMEASUREIPMITHREAD_HPP__
#define __CMEASUREIPMITHREAD_HPP__

#include <stdint.h>

#include "CMeasureIPMI.hpp"

#include "../common/CMeasureAbstractThread.hpp"

namespace NLibMeasure {
	class CMeasureIPMIThread : public CMeasureAbstractThread {
		public:
			CMeasureIPMIThread(CLogger& rLogger, CSemaphore& rStartSem, MEASUREMENT* pMeasurement, CMeasureAbstractResource& rMeasureRes);
			~CMeasureIPMIThread();
			
		private:
			void run(void);
	};
}

#endif /* __CMEASUREIPMITHREAD_HPP__ */
