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
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#ifndef __CDATAMEASUREMENT_HPP__
#define __CDATAMEASUREMENT_HPP__

#include "CDataSettings.hpp"
#include "../common/CRingBuffer.hpp"

namespace NData {
	class CDataMeasurement {
		private:
			CDataSettings& mrSettings;
			uint32_t mNumberOfTicks;
			
		public:
			NCommon::CRingBuffer<double> *mpX;
			
			NCommon::CRingBuffer<double> *mpYPowerCpu0;
			NCommon::CRingBuffer<double> *mpYPowerCpu1;
			NCommon::CRingBuffer<double> *mpYPowerGpu;
			NCommon::CRingBuffer<double> *mpYPowerFpga;
			NCommon::CRingBuffer<double> *mpYPowerMic;
			NCommon::CRingBuffer<double> *mpYPowerSystem;
			
			NCommon::CRingBuffer<double> *mpYTempCpu0;
			NCommon::CRingBuffer<double> *mpYTempCpu1;
			NCommon::CRingBuffer<double> *mpYTempGpu;
			NCommon::CRingBuffer<double> *mpYTempFpgaM;
			NCommon::CRingBuffer<double> *mpYTempFpgaI;
			NCommon::CRingBuffer<double> *mpYTempMicDie;
			NCommon::CRingBuffer<double> *mpYTempSystem;
			
			NCommon::CRingBuffer<double> *mpYClockCpu0;
			NCommon::CRingBuffer<double> *mpYClockCpu1;
			NCommon::CRingBuffer<double> *mpYClockGpuCore;
			NCommon::CRingBuffer<double> *mpYClockGpuMem;
			NCommon::CRingBuffer<double> *mpYClockMicCore;
			NCommon::CRingBuffer<double> *mpYClockMicMem;
			
			NCommon::CRingBuffer<double> *mpYUtilCpu;
			NCommon::CRingBuffer<double> *mpYUtilGpuCore;
			NCommon::CRingBuffer<double> *mpYUtilGpuMem;
			NCommon::CRingBuffer<double> *mpYUtilFpga;
			NCommon::CRingBuffer<double> *mpYUtilMic;
			
			NCommon::CRingBuffer<double> *mpYMemoryCpu;
			NCommon::CRingBuffer<double> *mpYSwapCpu;
			NCommon::CRingBuffer<double> *mpYMemoryGpu;
			NCommon::CRingBuffer<double> *mpYMemoryMic;
			
		public:
			CDataMeasurement(CDataSettings& rSettings);
			~CDataMeasurement(void);
			
			void reset(void);
			
			double getTime(void);
			
		private:
			void allocRingBuffers(void);
			void freeRingBuffers(void);
	};
}

#endif /* __CDATAMEASUREMENT_HPP__ */
