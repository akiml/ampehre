/*
 * QMSMFormUtilization.cpp
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
 * created: 2/03/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#include "QMSMFormUtilization.hpp"

#include "../data/CDataHandler.hpp"

#include <qwt_scale_engine.h>
#include <iostream>
namespace Ui {
	QMSMFormUtilization* QMSMFormUtilization::spFormUtil = 0;
	
	QMSMFormUtilization::QMSMFormUtilization(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormMeasurementAbstract(pParent, pDataHandler),
		mpCurveUtilCpu(new QwtPlotCurve("CPU")),
		mpCurveUtilGpuCore(new QwtPlotCurve("GPU Core")),
		mpCurveUtilGpuMemory(new QwtPlotCurve("GPU Memory")),
		mpCurveUtilFpga(new QwtPlotCurve("Compute FPGA")),
		mpCurveUtilMic(new QwtPlotCurve("MIC"))
		{
		
		setWindowTitle(QApplication::translate("FormMeasurement", "Utilization", 0, QApplication::UnicodeUTF8));
		
		qwtPlot->setAxisTitle(QwtPlot::yLeft, createTitle(QString::fromUtf8("Utilization [%]")));
		
		mpCurveUtilCpu->setPen(mColorCpu);
		mpCurveUtilGpuCore->setPen(mColorGpu);
		mpCurveUtilGpuMemory->setPen(mColorGpuAlternative);
		mpCurveUtilFpga->setPen(mColorFpga);
		mpCurveUtilMic->setPen(mColorMic);
		
		setupCurves();
		
		mpCurveUtilCpu->attach(qwtPlot);
		mpCurveUtilGpuCore->attach(qwtPlot);
		mpCurveUtilGpuMemory->attach(qwtPlot);
		mpCurveUtilFpga->attach(qwtPlot);
		mpCurveUtilMic->attach(qwtPlot);
		
		qwtPlot->insertLegend(mpLegend, QwtPlot::BottomLegend);
	}
	
	QMSMFormUtilization::~QMSMFormUtilization(void) {
		delete mpCurveUtilCpu;
		delete mpCurveUtilGpuCore;
		delete mpCurveUtilGpuMemory;
		delete mpCurveUtilFpga;
		delete mpCurveUtilMic;
		
		spFormUtil = 0;
	}
	
	QMSMFormUtilization *QMSMFormUtilization::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormUtil == 0) {
			spFormUtil = new QMSMFormUtilization(pParent, pDataHandler);
		}
		
		return spFormUtil;
	}
	
	void QMSMFormUtilization::setupCurves() {
		uint32_t ticks			= mpDataHandler->getSettings().mNumberOfTicks-1;
		
		double *x				= mpDataHandler->getMeasurement().mpX->getDataPtr();
		double *y_util_cpu		= mpDataHandler->getMeasurement().mpYUtilCpu->getDataPtr();
		double *y_util_gpu_core	= mpDataHandler->getMeasurement().mpYUtilGpuCore->getDataPtr();
		double *y_util_gpu_mem	= mpDataHandler->getMeasurement().mpYUtilGpuMem->getDataPtr();
		double *y_util_fpga		= mpDataHandler->getMeasurement().mpYUtilFpga->getDataPtr();
		double *y_util_mic		= mpDataHandler->getMeasurement().mpYUtilMic->getDataPtr();
		
		mpCurveUtilCpu->setRawSamples(x, y_util_cpu, ticks);
		mpCurveUtilGpuCore->setRawSamples(x, y_util_gpu_core, ticks);
		mpCurveUtilGpuMemory->setRawSamples(x, y_util_gpu_mem, ticks);
		mpCurveUtilFpga->setRawSamples(x, y_util_fpga, ticks);
		mpCurveUtilMic->setRawSamples(x, y_util_mic, ticks);
		
		scaleAxis(x[ticks-1], mpDataHandler->getSettings().mYAxisUtilMin, mpDataHandler->getSettings().mYAxisUtilMax);
	}
	
	double QMSMFormUtilization::getMiddleOfYAxis(void) {
		double min	= mpDataHandler->getSettings().mYAxisUtilMin;
		double max	= mpDataHandler->getSettings().mYAxisUtilMax;
		
		return min+((max-min)/2.0);
	}
}
