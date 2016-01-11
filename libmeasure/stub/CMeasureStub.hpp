/*
 * CMeasureStub.hpp
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
 * created: 6/01/15
 * version: 0.5.4 - add dynamic loading of resource specific libraries
 *          0.7.0 - modularized measurement struct
 */

#ifndef __CMEASURESTUB_HPP__
#define __CMEASURESTUB_HPP__

#include "../common/CMeasureAbstractResource.hpp"

namespace NLibMeasure {
	class CMeasureStub : public CMeasureAbstractResource {
		public:
			CMeasureStub(CLogger& rLogger);
			~CMeasureStub();
			
		private:
			void init(void);
			void destroy(void);
			
		public:
			void measure(void *pMsMeasurement, int32_t& rThreadNum);
			int getVariant();
	};
}

#endif /* __CMEASURESTUB_HPP__ */
