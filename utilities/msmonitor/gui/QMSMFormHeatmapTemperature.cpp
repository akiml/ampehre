/*
 * QMSMFormHeatmapTemperature.cpp
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
 * version: 0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#include "QMSMFormHeatmapTemperature.hpp"
namespace Ui {
	QMSMFormHeatmapTemperature* QMSMFormHeatmapTemperature::spFormHeatTemp = 0;
	
	QMSMFormHeatmapTemperature::QMSMFormHeatmapTemperature(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormHeatmapAbstract(pParent, pDataHandler),
		mpHeatmapCPU0(addHeatmap(QString::fromUtf8("CPU0\n\n [°C]"))),
		mpHeatmapCPU1(addHeatmap(QString::fromUtf8("CPU1\n\n [°C]"))),
		mpHeatmapGPU(addHeatmap(QString::fromUtf8("GPU\n\n[°C]"))),
		mpHeatmapFPGACompute(addHeatmap(QString::fromUtf8("Compute\nFPGA\n[°C]"))),
		mpHeatmapFPGAInterface(addHeatmap(QString::fromUtf8("Interface\nFPGA\n[°C]"))),
		mpHeatmapMIC(addHeatmap(QString::fromUtf8("MIC Die\n\n[°C]"))),
		mpHeatmapSysboard(addHeatmap(QString::fromUtf8("Main-\nboard\n[°C]"))),
		mCurrentX(0)
		{
		//only one minute is stored here
		mBufferSize = mpDataHandler->getSettings().mTimeToShowData / mpDataHandler->getSettings().mHeatmapSamplingRate;
		mpCPU0Data			= (double *)malloc(mBufferSize * sizeof(double));
		mpCPU1Data			= (double *)malloc(mBufferSize * sizeof(double));
		mpGPUData			= (double *)malloc(mBufferSize * sizeof(double));
		mpFPGAComputeData	= (double *)malloc(mBufferSize * sizeof(double));
		mpFPGAInterfaceData	= (double *)malloc(mBufferSize * sizeof(double));
		mpMICData			= (double *)malloc(mBufferSize * sizeof(double));
		mpSysboardData		= (double *)malloc(mBufferSize * sizeof(double));
				
		setWindowTitle(QApplication::translate("FormHeatmap", "Temperature", 0, QApplication::UnicodeUTF8));
		
		setupHeatmaps();
	}
	
	QMSMFormHeatmapTemperature::~QMSMFormHeatmapTemperature(void) {
		delete mpCPU0Data;
		delete mpCPU1Data;
		delete mpGPUData;
		delete mpFPGAComputeData;
		delete mpFPGAInterfaceData;
		delete mpMICData;
		delete mpSysboardData;
		
		spFormHeatTemp = 0;
	}
	
	QMSMFormHeatmapTemperature *QMSMFormHeatmapTemperature::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormHeatTemp == 0) {
			spFormHeatTemp = new QMSMFormHeatmapTemperature(pParent, pDataHandler);
		}
		
		return spFormHeatTemp;
	}
	
	void QMSMFormHeatmapTemperature::setupHeatmaps() {
		updateHeatmapData();
		
		mpHeatmapCPU0->setData(mpCPU0Data, mBufferSize);
		mpHeatmapCPU0->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapCPU0->setYInterval(mpDataHandler->getSettings().mHeatmapTempCPUMinY, mpDataHandler->getSettings().mHeatmapTempCPUMaxY);
		
		mpHeatmapCPU1->setData(mpCPU1Data, mBufferSize);
		mpHeatmapCPU1->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapCPU1->setYInterval(mpDataHandler->getSettings().mHeatmapTempCPUMinY, mpDataHandler->getSettings().mHeatmapTempCPUMaxY);
		
		mpHeatmapGPU->setData(mpGPUData, mBufferSize);
		mpHeatmapGPU->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapGPU->setYInterval(mpDataHandler->getSettings().mHeatmapTempGPUMinY, mpDataHandler->getSettings().mHeatmapTempGPUMaxY);
		
		mpHeatmapFPGACompute->setData(mpFPGAComputeData, mBufferSize);
		mpHeatmapFPGACompute->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapFPGACompute->setYInterval(mpDataHandler->getSettings().mHeatmapTempFPGAMinY, mpDataHandler->getSettings().mHeatmapTempFPGAMaxY);
		
		mpHeatmapFPGAInterface->setData(mpFPGAInterfaceData, mBufferSize);
		mpHeatmapFPGAInterface->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapFPGAInterface->setYInterval(mpDataHandler->getSettings().mHeatmapTempFPGAMinY, mpDataHandler->getSettings().mHeatmapTempFPGAMaxY);
		
		mpHeatmapMIC->setData(mpMICData, mBufferSize);
		mpHeatmapMIC->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapMIC->setYInterval(mpDataHandler->getSettings().mHeatmapTempMICMinY, mpDataHandler->getSettings().mHeatmapTempMICMaxY);
		
		mpHeatmapSysboard->setData(mpSysboardData, mBufferSize);
		mpHeatmapSysboard->setXInterval(mCurrentX-50, mCurrentX+10);
		mpHeatmapSysboard->setYInterval(mpDataHandler->getSettings().mHeatmapTempSYSMinY, mpDataHandler->getSettings().mHeatmapTempSYSMaxY);
	}
	
    void QMSMFormHeatmapTemperature::updateHeatmapData() {
		uint32_t factorDataToHeatmapSamplingRate = mpDataHandler->getSettings().mHeatmapSamplingRate / mpDataHandler->getSettings().mDataSamplingRate;
		uint32_t bufferedSamples = ((mpDataHandler->getSettings().mTimeToBufferData - mpDataHandler->getSettings().mTimeToShowData) /
									mpDataHandler->getSettings().mDataSamplingRate)-1;
		uint32_t showSamples = (mpDataHandler->getSettings().mTimeToShowData / mpDataHandler->getSettings().mDataSamplingRate) - 1;
		uint32_t indexCurrentX = 0;
		uint32_t indexStartX = 0;
		
		double *x = mpDataHandler->getMeasurement().mpX->getDataPtr() + bufferedSamples;
		
		//shift x axis every 10 s
		if(x[showSamples] - mCurrentX > 10) {
			mCurrentX += 10;
		}
		
		//reset x axis if necessary 
		if(x[showSamples] < mCurrentX) {
			mCurrentX = 0;
		}
		
		//search the index corresponding to mCurrentX
		for (int i = showSamples; i >= 0; i--) {
			if(x[i] <= (mCurrentX + ((double) mpDataHandler->getSettings().mDataSamplingRate/1000/3))){
				 indexCurrentX = i;
				 break;
			}
		}
		
		//calculate the index of the first element
		indexStartX = indexCurrentX - (mpDataHandler->getSettings().mTimeToShowData -10000) / mpDataHandler->getSettings().mDataSamplingRate;
		
		double *temp_cpu_0	= mpDataHandler->getMeasurement().mpYTempCpu0->getDataPtr() + bufferedSamples + indexStartX;
		double *temp_cpu_1	= mpDataHandler->getMeasurement().mpYTempCpu1->getDataPtr() + bufferedSamples + indexStartX;
		double *temp_gpu	= mpDataHandler->getMeasurement().mpYTempGpu->getDataPtr() + bufferedSamples + indexStartX;
		double *temp_fpga_m	= mpDataHandler->getMeasurement().mpYTempFpgaM->getDataPtr() + bufferedSamples + indexStartX;
		double *temp_fpga_i	= mpDataHandler->getMeasurement().mpYTempFpgaI->getDataPtr()  + bufferedSamples + indexStartX;
		double *temp_mic	= mpDataHandler->getMeasurement().mpYTempMicDie->getDataPtr()  + bufferedSamples + indexStartX;
		double *temp_system	= mpDataHandler->getMeasurement().mpYTempSystem->getDataPtr() + bufferedSamples + indexStartX;
		
		//calculate the mean and fill the rest with 0
		for (uint32_t i = 0; i < (mBufferSize); i++) {
			if(i < mBufferSize-(indexStartX/factorDataToHeatmapSamplingRate)) {
				mpCPU0Data[i]			= calcMean(temp_cpu_0, factorDataToHeatmapSamplingRate);
				mpCPU1Data[i]			= calcMean(temp_cpu_1, factorDataToHeatmapSamplingRate);
				mpGPUData[i]			= calcMean(temp_gpu, factorDataToHeatmapSamplingRate);
				mpFPGAComputeData[i]	= calcMean(temp_fpga_m, factorDataToHeatmapSamplingRate);
				mpFPGAInterfaceData[i]	= calcMean(temp_fpga_i, factorDataToHeatmapSamplingRate);
				mpMICData[i]			= calcMean(temp_mic, factorDataToHeatmapSamplingRate);
				mpSysboardData[i]		= calcMean(temp_system, factorDataToHeatmapSamplingRate);
				
				temp_cpu_0	+= factorDataToHeatmapSamplingRate;
				temp_cpu_1	+= factorDataToHeatmapSamplingRate;
				temp_gpu	+= factorDataToHeatmapSamplingRate;
				temp_fpga_m	+= factorDataToHeatmapSamplingRate;
				temp_fpga_i	+= factorDataToHeatmapSamplingRate;
				temp_mic	+= factorDataToHeatmapSamplingRate;
				temp_system	+= factorDataToHeatmapSamplingRate;
			} else {
				mpCPU0Data[i]			= 0;
				mpCPU1Data[i]			= 0;
				mpGPUData[i]			= 0;
				mpFPGAComputeData[i]	= 0;
				mpFPGAInterfaceData[i]	= 0;
				mpMICData[i]			= 0;
				mpSysboardData[i]		= 0;
			}
		}
    }
    
    double QMSMFormHeatmapTemperature::calcMean(double* data, uint32_t size) {
		double sum = 0;
		for (uint32_t i = 0; i < size; i++) {
			sum += data[i];
		}
		return sum / size;
    }
}
