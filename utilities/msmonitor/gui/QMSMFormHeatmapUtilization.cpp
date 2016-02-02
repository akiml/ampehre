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
 * version: 0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#include "QMSMFormHeatmapUtilization.hpp"
namespace Ui {
	QMSMFormHeatmapUtilization* QMSMFormHeatmapUtilization::spFormHeatUtil = 0;
	
	QMSMFormHeatmapUtilization::QMSMFormHeatmapUtilization(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormHeatmapAbstract(pParent, pDataHandler),
		mpHeatmapCPU(addHeatmap("CPU\n")),
		mpHeatmapGPUCore(addHeatmap("GPU\nCore")),
		mpHeatmapGPUMemory(addHeatmap("GPU\nMemory")),
		mpHeatmapFPGA(addHeatmap("Compute\nFPGA")),
		mpHeatmapMIC(addHeatmap("MIC\n"))
		{
		//only one minute is stored here
		uint32_t size = mpDataHandler->getSettings().mTimeToShowData / mpDataHandler->getSettings().mHeatmapSamplingRate;
		mpCPUData		= (double *)malloc(size * sizeof(double));
		mpGPUCoreData	= (double *)malloc(size * sizeof(double));
		mpGPUMemoryData	= (double *)malloc(size * sizeof(double));
		mpFPGAData		= (double *)malloc(size * sizeof(double));
		mpMICData		= (double *)malloc(size * sizeof(double));
		
		setWindowTitle(QApplication::translate("FormMeasurement", "Utilization", 0, QApplication::UnicodeUTF8));
		
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
		uint32_t ticks			= mpDataHandler->getSettings().mNumberOfTicks-1;
		
		double *x				= mpDataHandler->getMeasurement().mpX->getDataPtr();
		double *y_util_cpu		= mpDataHandler->getMeasurement().mpYUtilCpu->getDataPtr();
		double *y_util_gpu_core	= mpDataHandler->getMeasurement().mpYUtilGpuCore->getDataPtr();
		double *y_util_gpu_mem	= mpDataHandler->getMeasurement().mpYUtilGpuMem->getDataPtr();
		double *y_util_fpga		= mpDataHandler->getMeasurement().mpYUtilFpga->getDataPtr();
		double *y_util_mic		= mpDataHandler->getMeasurement().mpYUtilMic->getDataPtr();
		
		//TODO
		
		y_util_cpu += 4000;
		mpHeatmapCPU->setData(y_util_cpu, 2000);
		mpHeatmapCPU->setXInterval(0, 60);
		
		y_util_gpu_core += 4000;
		mpHeatmapGPUCore->setData(y_util_gpu_core, 2000);
		mpHeatmapGPUCore->setXInterval(0, 60);
		
		y_util_gpu_mem += 4000;
		mpHeatmapGPUMemory->setData(y_util_gpu_mem, 2000);
		mpHeatmapGPUMemory->setXInterval(0, 60);
		
		y_util_fpga += 4000;
		mpHeatmapFPGA->setData(y_util_fpga, 2000);
		mpHeatmapFPGA->setXInterval(0, 60);
		
		y_util_mic += 4000;
		mpHeatmapMIC->setData(y_util_mic, 2000);
		mpHeatmapMIC->setXInterval(0, 60);
		/**std::cout << "x: ";
		for (int i = 0; i< 6000; i++){
			std::cout << x[i] << " ";
		}*/
		//std::cout<< std::endl;
		/**y_util_gpu_core += 4000;
		heatmapGPU->setData(y_util_gpu_core, 2000);
		heatmapGPU->setXInterval(0, 60);
		y_util_fpga += 4000;
		heatmapFPGA->setData(y_util_fpga, 2000);
		heatmapFPGA->setXInterval(0, 60);
		y_util_mic += 4000;
		heatmapMIC->setData(y_util_mic, 2000);
		heatmapMIC->setXInterval(0, 60);*/
	}
}
