/*
 * QMSMFormHeatmapTemperature.hpp
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
 * created: 02/09/16
 * version: 0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#ifndef __QMSMFORMHEATMAPTEMPERATURE_HPP__
#define __QMSMFORMHEATMAPTEMPERATURE_HPP__

#include "QMSMFormHeatmapAbstract.hpp"
#include "../data/CDataHandler.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormHeatmapTemperature : public QMSMFormHeatmapAbstract {
		Q_OBJECT
		
		private:
			static QMSMFormHeatmapTemperature* spFormHeatTemp;
			
			QMSMHeatmap *mpHeatmapCPU0;
			QMSMHeatmap *mpHeatmapCPU1;
			QMSMHeatmap *mpHeatmapGPU;
			QMSMHeatmap *mpHeatmapFPGACompute;
			QMSMHeatmap *mpHeatmapFPGAInterface;
			QMSMHeatmap *mpHeatmapMIC;
			QMSMHeatmap *mpHeatmapSysboard;
			
			double *mpCPU0Data;
			double *mpCPU1Data;
			double *mpGPUData;
			double *mpFPGAComputeData;
			double *mpFPGAInterfaceData;
			double *mpMICData;
			double *mpSysboardData;
			
			uint32_t mBufferSize;
			
		private:
			explicit QMSMFormHeatmapTemperature(QWidget *pParent, NData::CDataHandler *pDataHandler);
			void updateHeatmapData();
			double calcMean(double *data, uint32_t size);
			
		public:
			~QMSMFormHeatmapTemperature(void);
			
			static QMSMFormHeatmapTemperature* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		protected:
			void setupHeatmaps();
	};
}

#endif /* __QMSMFORMHEATMAPTEMPERATURE_HPP__ */
