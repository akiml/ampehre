/*
 * QMSMFormPower.cpp
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
 * created: 1/29/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 */

#include "QMSMFormPower.hpp"

#include "../data/CDataHandler.hpp"

namespace Ui {
	QMSMFormPower* QMSMFormPower::spFormPower = 0;
	
	QMSMFormPower::QMSMFormPower(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormMeasurementAbstract(pParent, pDataHandler),
		mpCurvePowerCpu0(new QwtPlotCurve("CPU0")),
		mpCurvePowerCpu1(new QwtPlotCurve("CPU1")),
		mpCurvePowerGpu(new QwtPlotCurve("GPU")),
		mpCurvePowerFpga(new QwtPlotCurve("FPGA")),
		mpCurvePowerMic(new QwtPlotCurve("MIC")),
		mpCurvePowerSystem(new QwtPlotCurve("System"))
		{
		
		setWindowTitle(QApplication::translate("FormMeasurement", "Power", 0, QApplication::UnicodeUTF8));
		
		qwtPlot->setAxisTitle(QwtPlot::yLeft, createTitle(QString::fromUtf8("Power [W]")));
		
		mpCurvePowerCpu0->setPen(mColorCpu);
		mpCurvePowerCpu1->setPen(mColorCpuAlternative);
		mpCurvePowerGpu->setPen(mColorGpu);
		mpCurvePowerFpga->setPen(mColorFpga);
		mpCurvePowerMic->setPen(mColorMic);
		mpCurvePowerSystem->setPen(mColorSystem);
		
		setupCurves(mpDataHandler->getSettings().mNumberOfTicks);
		
		mpCurvePowerCpu0->attach(qwtPlot);
		mpCurvePowerCpu1->attach(qwtPlot);
		mpCurvePowerGpu->attach(qwtPlot);
		mpCurvePowerFpga->attach(qwtPlot);
		mpCurvePowerMic->attach(qwtPlot);
		mpCurvePowerSystem->attach(qwtPlot);
		
		qwtPlot->insertLegend(mpLegend, QwtPlot::BottomLegend);
	}
	
	QMSMFormPower::~QMSMFormPower(void) {
		delete mpCurvePowerCpu0;
		delete mpCurvePowerCpu1;
		delete mpCurvePowerGpu;
		delete mpCurvePowerFpga;
		delete mpCurvePowerMic;
		delete mpCurvePowerSystem;
		
		spFormPower = 0;
	}
	
	QMSMFormPower *QMSMFormPower::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormPower == 0) {
			spFormPower = new QMSMFormPower(pParent, pDataHandler);
		}
		
		return spFormPower;
	}
	
	void QMSMFormPower::setupCurves(uint32_t currentTick) {
		uint32_t ticks			= mpDataHandler->getSettings().mNumberOfTicks-1;
		
		double *x				= mpDataHandler->getMeasurement().mpX+currentTick;
		double *y_power_cpu_0	= mpDataHandler->getMeasurement().mpYPowerCpu0+currentTick;
		double *y_power_cpu_1	= mpDataHandler->getMeasurement().mpYPowerCpu1+currentTick;
		double *y_power_gpu		= mpDataHandler->getMeasurement().mpYPowerGpu+currentTick;
		double *y_power_fpga	= mpDataHandler->getMeasurement().mpYPowerFpga+currentTick;
		double *y_power_mic		= mpDataHandler->getMeasurement().mpYPowerMic+currentTick;
		double *y_power_system	= mpDataHandler->getMeasurement().mpYPowerSystem+currentTick;
		
		mpCurvePowerCpu0->setRawData(x, y_power_cpu_0, ticks);
		mpCurvePowerCpu1->setRawData(x, y_power_cpu_1, ticks);
		mpCurvePowerGpu->setRawData(x, y_power_gpu, ticks);
		mpCurvePowerFpga->setRawData(x, y_power_fpga, ticks);
		mpCurvePowerMic->setRawData(x, y_power_mic, ticks);
		mpCurvePowerSystem->setRawData(x, y_power_system, ticks);
		
		scaleAxis(x[ticks-1], mpDataHandler->getSettings().mYAxisPowerMin, mpDataHandler->getSettings().mYAxisPowerMax);
	}
	
	double QMSMFormPower::getMiddleOfYAxis(void) {
		double min	= mpDataHandler->getSettings().mYAxisPowerMin;
		double max	= mpDataHandler->getSettings().mYAxisPowerMax;
		
		return min+((max-min)/2.0);
	}
}