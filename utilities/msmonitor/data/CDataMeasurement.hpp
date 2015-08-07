/*
 * CDataMeasurement.hpp
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
 * created: 1/27/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.0 - add cpu, gpu and mic memory information
 */

#ifndef __CDATAMEASUREMENT_HPP__
#define __CDATAMEASUREMENT_HPP__

#include "CDataSettings.hpp"

namespace NData {
	class CDataMeasurement {
		private:
			CDataSettings& mrSettings;
			uint32_t mNumberOfTicks;
			uint32_t mTick;
			
		public:
			double *mpX;
			
			double *mpYPowerCpu0;
			double *mpYPowerCpu1;
			double *mpYPowerGpu;
			double *mpYPowerFpga;
			double *mpYPowerMic;
			double *mpYPowerSystem;
			
			double *mpYTempCpu0;
			double *mpYTempCpu1;
			double *mpYTempGpu;
			double *mpYTempFpgaM;
			double *mpYTempFpgaI;
			double *mpYTempMicDie;
			double *mpYTempSystem;
			
			double *mpYClockCpu0;
			double *mpYClockCpu1;
			double *mpYClockGpuCore;
			double *mpYClockGpuMem;
			double *mpYClockMicCore;
			double *mpYClockMicMem;
			
			double *mpYUtilCpu;
			double *mpYUtilGpuCore;
			double *mpYUtilGpuMem;
			double *mpYUtilFpga;
			double *mpYUtilMic;
			
			double *mpYMemoryCpu;
			double *mpYSwapCpu;
			double *mpYMemoryGpu;
			double *mpYMemoryMic;
			
		public:
			CDataMeasurement(CDataSettings& rSettings);
			~CDataMeasurement(void);
			
			void reset(void);
			
			uint32_t getTick(void);
			void incTick(void);
			
			double getTime(void);
			
		private:
			void allocArrays(void);
			void freeArrays(void);
	};
}

#endif /* __CDATAMEASUREMENT_HPP__ */
