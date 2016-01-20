/*
 * QMSMFormTemperature.cpp
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
 * created: 2/02/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#include "QMSMFormTemperature.hpp"

#include "../data/CDataHandler.hpp"

namespace Ui {
	QMSMFormTemperature* QMSMFormTemperature::spFormTemp = 0;
	
	QMSMFormTemperature::QMSMFormTemperature(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormMeasurementAbstract(pParent, pDataHandler),
		mpCurveTempCpu0(new QwtPlotCurve("CPU0")),
		mpCurveTempCpu1(new QwtPlotCurve("CPU1")),
		mpCurveTempGpu(new QwtPlotCurve("GPU")),
		mpCurveTempFpgaM(new QwtPlotCurve("Compute FPGA")),
		mpCurveTempFpgaI(new QwtPlotCurve("Interface FPGA")),
		mpCurveTempMic(new QwtPlotCurve("MIC Die")),
		mpCurveTempSystem(new QwtPlotCurve("Mainboard"))
		{
		
		setWindowTitle(QApplication::translate("FormMeasurement", "Temperature", 0, QApplication::UnicodeUTF8));
		
		qwtPlot->setAxisTitle(QwtPlot::yLeft, createTitle(QString::fromUtf8("Temperature [°C]")));
		
		mpCurveTempCpu0->setPen(mColorCpu);
		mpCurveTempCpu1->setPen(mColorCpuAlternative);
		mpCurveTempGpu->setPen(mColorGpu);
		mpCurveTempFpgaM->setPen(mColorFpga);
		mpCurveTempFpgaI->setPen(mColorFpgaAlternative);
		mpCurveTempMic->setPen(mColorMic);
		mpCurveTempSystem->setPen(mColorSystem);
		
		setupCurves();
		
		mpCurveTempCpu0->attach(qwtPlot);
		mpCurveTempCpu1->attach(qwtPlot);
		mpCurveTempGpu->attach(qwtPlot);
		mpCurveTempFpgaM->attach(qwtPlot);
		mpCurveTempFpgaI->attach(qwtPlot);
		mpCurveTempMic->attach(qwtPlot);
		mpCurveTempSystem->attach(qwtPlot);
		
		qwtPlot->insertLegend(mpLegend, QwtPlot::BottomLegend);
	}
	
	QMSMFormTemperature::~QMSMFormTemperature(void) {
		delete mpCurveTempCpu0;
		delete mpCurveTempCpu1;
		delete mpCurveTempGpu;
		delete mpCurveTempFpgaM;
		delete mpCurveTempFpgaI;
		delete mpCurveTempMic;
		delete mpCurveTempSystem;
		
		spFormTemp = 0;
	}
	
	QMSMFormTemperature *QMSMFormTemperature::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormTemp == 0) {
			spFormTemp = new QMSMFormTemperature(pParent, pDataHandler);
		}
		
		return spFormTemp;
	}
	
	void QMSMFormTemperature::setupCurves() {
		uint32_t ticks			= mpDataHandler->getSettings().mNumberOfTicks-1;
		
		double *x				= mpDataHandler->getMeasurement().mpX->getDataPtr();
		double *y_temp_cpu_0	= mpDataHandler->getMeasurement().mpYTempCpu0->getDataPtr();
		double *y_temp_cpu_1	= mpDataHandler->getMeasurement().mpYTempCpu1->getDataPtr();
		double *y_temp_gpu		= mpDataHandler->getMeasurement().mpYTempGpu->getDataPtr();
		double *y_temp_fpga_m	= mpDataHandler->getMeasurement().mpYTempFpgaM->getDataPtr();
		double *y_temp_fpga_i	= mpDataHandler->getMeasurement().mpYTempFpgaI->getDataPtr();
		double *y_temp_mic		= mpDataHandler->getMeasurement().mpYTempMicDie->getDataPtr();
		double *y_temp_system	= mpDataHandler->getMeasurement().mpYTempSystem->getDataPtr();
		
		mpCurveTempCpu0->setRawData(x, y_temp_cpu_0, ticks);
		mpCurveTempCpu1->setRawData(x, y_temp_cpu_1, ticks);
		mpCurveTempGpu->setRawData(x, y_temp_gpu, ticks);
		mpCurveTempFpgaM->setRawData(x, y_temp_fpga_m, ticks);
		mpCurveTempFpgaI->setRawData(x, y_temp_fpga_i, ticks);
		mpCurveTempMic->setRawData(x, y_temp_mic, ticks);
		mpCurveTempSystem->setRawData(x, y_temp_system, ticks);
		
		scaleAxis(x[ticks-1], mpDataHandler->getSettings().mYAxisTempMin, mpDataHandler->getSettings().mYAxisTempMax);
	}
	
	double QMSMFormTemperature::getMiddleOfYAxis(void) {
		double min	= mpDataHandler->getSettings().mYAxisTempMin;
		double max	= mpDataHandler->getSettings().mYAxisTempMax;
		
		return min+((max-min)/2.0);
	}
}
