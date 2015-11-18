/*
 * CMeasureNVML.hpp
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
 *          0.1.13 - make GPU frequency settable
 *          0.2.3 - add gpu_management tool and use the tool in the libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.1 - modularised libmeasure
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library.
 */

#ifndef __CMEASURENVML_HPP__
#define __CMEASURENVML_HPP__

#include <unistd.h>
#include <sys/wait.h>

#include "../common/CMeasureAbstractResource.hpp"
#include <nvml.h>

namespace NLibMeasure {
	template <int SkipMs, int Variant>
	class CMeasureNVML : public CMeasureAbstractResource {
		private:
			nvmlDevice_t mDevice;
			
			gpu_frequency mGpuFrequency;
			
		public:
			CMeasureNVML(CLogger& rLogger, gpu_frequency gpuFrequency);
			~CMeasureNVML();
			
		private:
			void init(void);
			void destroy(void);
			int exec_gpu_mgmt(char* args[]);
			
		public:
			void measure(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			void read_memory_total(MEASUREMENT *pMeasurement, int32_t& rThreadNum);
			int getVariant();
	};
}

#include "CMeasureNVML.cpp"

#endif /* __CMEASURENVML_HPP__ */
