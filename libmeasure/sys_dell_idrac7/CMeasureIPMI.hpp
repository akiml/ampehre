/*
 * CMeasureIPMI.hpp
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
 * created: 3/08/14
 * version: 0.1.0 - initial implementation
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

#ifndef __CMEASUREIPMI_HPP__
#define __CMEASUREIPMI_HPP__

#include "../common/CMeasureAbstractResource.hpp"
#include "../../include/ms_ipmiwrapper.h"

namespace NLibMeasure {
	class CMeasureIPMI : public CMeasureAbstractResource{
		private: 
			uint64_t mTimeoutSetting;
		public:
			CMeasureIPMI(CLogger& rLogger, uint64_t ipmi_timeout_setting);
			~CMeasureIPMI();
			
		private:
			void init(void);
			void destroy(void);
			void measureRecordIDs(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void measureRawMsgs(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void measureRawMsgDellResetEnergy(int32_t& rThreadNum);
			void measureRawMsgDellCumulativeEnergy(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void measureRawMsgDellCurrentPower(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			
		public:
			void measure(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void resetEnergyCounter(int32_t& rThreadNum);
			void setIPMITimeout(uint32_t timeout, int32_t& rThreadNum);
	};
}

#endif /* __CMEASUREIPMI_HPP__ */
