/*
 * QMSMFormHeatmapUtilization.cpp
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
 * version: 0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#include "QMSMFormHeatmapUtilization.hpp"
namespace Ui {
	QMSMFormHeatmapUtilization* QMSMFormHeatmapUtilization::spFormHeatUtil = 0;
	
	QMSMFormHeatmapUtilization::QMSMFormHeatmapUtilization(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormHeatmapAbstract(pParent, pDataHandler),
		mpHeatmapCPU(addHeatmap("CPU\n\n[%]")),
		mpHeatmapGPUCore(addHeatmap("GPU\nCore\n[%]")),
		mpHeatmapGPUMemory(addHeatmap("GPU\nMemory\n[%]")),
		mpHeatmapFPGA(addHeatmap("Compute\nFPGA\n[%]")),
		mpHeatmapMIC(addHeatmap("MIC\n\n[%]"))
		{
		//only one minute is stored here
		mBufferSize		= mpDataHandler->getSettings().mTimeToShowData / mpDataHandler->getSettings().mHeatmapSamplingRate;
		mpCPUData		= (double *)malloc(mBufferSize * sizeof(double));
		mpGPUCoreData	= (double *)malloc(mBufferSize * sizeof(double));
		mpGPUMemoryData	= (double *)malloc(mBufferSize * sizeof(double));
		mpFPGAData		= (double *)malloc(mBufferSize * sizeof(double));
		mpMICData		= (double *)malloc(mBufferSize * sizeof(double));
		
		setWindowTitle(QApplication::translate("FormHeatmap", "Utilization", 0, QApplication::UnicodeUTF8));
		
		setupHeatmaps();
	}
	
	QMSMFormHeatmapUtilization::~QMSMFormHeatmapUtilization(void) {
		delete mpCPUData;
		delete mpGPUCoreData;
		delete mpGPUMemoryData;
		delete mpFPGAData;
		delete mpMICData;
		
		spFormHeatUtil = 0;
	}
	
	QMSMFormHeatmapUtilization *QMSMFormHeatmapUtilization::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormHeatUtil == 0) {
			spFormHeatUtil = new QMSMFormHeatmapUtilization(pParent, pDataHandler);
		}
		
		return spFormHeatUtil;
	}
	
	void QMSMFormHeatmapUtilization::setupHeatmaps() {
		updateHeatmapData();
		
		mpHeatmapCPU->setData(mpCPUData, mBufferSize);
		mpHeatmapCPU->setXInterval(mCurrentX-50, mCurrentX+10);
		
		mpHeatmapGPUCore->setData(mpGPUCoreData, mBufferSize);
		mpHeatmapGPUCore->setXInterval(mCurrentX-50, mCurrentX+10);
		
		mpHeatmapGPUMemory->setData(mpGPUMemoryData, mBufferSize);
		mpHeatmapGPUMemory->setXInterval(mCurrentX-50, mCurrentX+10);
		
		mpHeatmapFPGA->setData(mpFPGAData, mBufferSize);
		mpHeatmapFPGA->setXInterval(mCurrentX-50, mCurrentX+10);
		
		mpHeatmapMIC->setData(mpMICData, mBufferSize);
		mpHeatmapMIC->setXInterval(mCurrentX-50, mCurrentX+10);
	}
	
    void QMSMFormHeatmapUtilization::updateHeatmapData() {
		uint32_t factorDataToHeatmapSamplingRate = mpDataHandler->getSettings().mHeatmapSamplingRate / mpDataHandler->getSettings().mDataSamplingRate;
		uint32_t bufferedSamples = ((mpDataHandler->getSettings().mTimeToBufferData - mpDataHandler->getSettings().mTimeToShowData) /
									mpDataHandler->getSettings().mDataSamplingRate)-1;
		uint32_t indexStartX = 0;
		
		updateCurrentX();
		
		//calculate the index of the first element
		indexStartX = mIndexCurrentX - (mpDataHandler->getSettings().mTimeToShowData -10000) / mpDataHandler->getSettings().mDataSamplingRate;
		
		double *util_cpu		= mpDataHandler->getMeasurement().mpYUtilCpu->getDataPtr() + bufferedSamples + indexStartX;
		double *util_gpu_core	= mpDataHandler->getMeasurement().mpYUtilGpuCore->getDataPtr() + bufferedSamples + indexStartX;
		double *util_gpu_mem	= mpDataHandler->getMeasurement().mpYUtilGpuMem->getDataPtr() + bufferedSamples + indexStartX;
		double *util_fpga		= mpDataHandler->getMeasurement().mpYUtilFpga->getDataPtr() + bufferedSamples + indexStartX;
		double *util_mic		= mpDataHandler->getMeasurement().mpYUtilMic->getDataPtr() + bufferedSamples + indexStartX;
		
		//calculate the mean and fill the rest with 0
		for (uint32_t i = 0; i < (mBufferSize); i++) {
			if(i < mBufferSize-(indexStartX/factorDataToHeatmapSamplingRate) - 1) {
				mpCPUData[i]		= calcMean(util_cpu, factorDataToHeatmapSamplingRate);
				mpGPUCoreData[i]	= calcMean(util_gpu_core, factorDataToHeatmapSamplingRate);
				mpGPUMemoryData[i]	= calcMean(util_gpu_mem, factorDataToHeatmapSamplingRate);
				mpFPGAData[i]		= calcMean(util_fpga, factorDataToHeatmapSamplingRate);
				mpMICData[i]		= calcMean(util_mic, factorDataToHeatmapSamplingRate);
				util_cpu		+= factorDataToHeatmapSamplingRate;
				util_gpu_core	+= factorDataToHeatmapSamplingRate;
				util_gpu_mem	+= factorDataToHeatmapSamplingRate;
				util_fpga		+= factorDataToHeatmapSamplingRate;
				util_mic		+= factorDataToHeatmapSamplingRate;
			} else {
				mpCPUData[i]		= 0;
				mpGPUCoreData[i]	= 0;
				mpGPUMemoryData[i]	= 0;
				mpFPGAData[i]		= 0;
				mpMICData[i]		= 0;
			}
		}
    }
    
    double QMSMFormHeatmapUtilization::calcMean(double* data, uint32_t size) {
		double sum = 0;
		for (uint32_t i = 0; i < size; i++) {
			sum += data[i];
		}
		return sum / size;
    }
}
