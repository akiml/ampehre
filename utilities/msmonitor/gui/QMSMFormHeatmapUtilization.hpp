/*
 * QMSMFormHeatmapUtilization.hpp
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
 * created: 01/27/16
 * version: 0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#ifndef __QMSMFORMHEATMAPUTILIZATION_HPP__
#define __QMSMFORMHEATMAPUTILIZATION_HPP__

#include "QMSMFormHeatmapAbstract.hpp"
#include "../data/CDataHandler.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormHeatmapUtilization : public QMSMFormHeatmapAbstract {
		Q_OBJECT
		
		private:
			static QMSMFormHeatmapUtilization* spFormHeatUtil;
			
			Heatmap *mpHeatmapCPU;
			Heatmap *mpHeatmapGPUCore;
			Heatmap *mpHeatmapGPUMemory;
			Heatmap *mpHeatmapFPGA;
			Heatmap *mpHeatmapMIC;
			
			double *mpCPUData;
			double *mpGPUCoreData;
			double *mpGPUMemoryData;
			double *mpFPGAData;
			double *mpMICData;
			
			uint32_t mBufferSize;
			double mCurrentX;
			
		private:
			explicit QMSMFormHeatmapUtilization(QWidget *pParent, NData::CDataHandler *pDataHandler);
			void updateHeatmapData();
			double calcMean(double *data, uint32_t size);
			
		public:
			~QMSMFormHeatmapUtilization(void);
			
			static QMSMFormHeatmapUtilization* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		protected:
			void setupHeatmaps();
	};
}

#endif /* __QMSMFORMHEATMAPUTILIZATION_HPP__ */
