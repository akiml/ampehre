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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 *          0.7.0 - modularised measurement struct
 */

#ifndef __CMEASUREIPMITHREAD_HPP__
#define __CMEASUREIPMITHREAD_HPP__

#include <stdint.h>
#include <iomanip>

#include "CMeasureIPMI.hpp"

#include "../common/CMeasureAbstractThread.hpp"

namespace NLibMeasure {
 	template <int TVariant>
	class CMeasureIPMIThread : public CMeasureAbstractThread {
		public:
			CMeasureIPMIThread(CLogger& rLogger, CSemaphore& rStartSem, void* pMsMeasurement, CMeasureAbstractResource& rMeasureRes);
			~CMeasureIPMIThread();
			
		private:
			void run(void);
	};
}

#include "CMeasureIPMIThread.cpp"

#endif /* __CMEASUREIPMITHREAD_HPP__ */
