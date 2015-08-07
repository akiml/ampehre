/*
 * QMSMFormClock.cpp
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
 */

#include "QMSMFormClock.hpp"

#include "../data/CDataHandler.hpp"

namespace Ui {
	QMSMFormClock* QMSMFormClock::spFormClock = 0;
	
	QMSMFormClock::QMSMFormClock(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormMeasurementAbstract(pParent, pDataHandler),
		mpCurveClockCpu0(new QwtPlotCurve("Avg. CPU0")),
		mpCurveClockCpu1(new QwtPlotCurve("Avg. CPU1")),
		mpCurveClockGpuCore(new QwtPlotCurve("GPU Core")),
		mpCurveClockGpuMemory(new QwtPlotCurve("GPU Memory")),
		mpCurveClockMicCore(new QwtPlotCurve("MIC Core")),
		mpCurveClockMicMemory(new QwtPlotCurve("MIC Memory"))
		{
		
		setWindowTitle(QApplication::translate("FormMeasurement", "Clock", 0, QApplication::UnicodeUTF8));
		
		qwtPlot->setAxisTitle(QwtPlot::yLeft, createTitle(QString::fromUtf8("Frequency [MHz]")));
		
		mpCurveClockCpu0->setPen(mColorCpu);
		mpCurveClockCpu1->setPen(mColorCpuAlternative);
		mpCurveClockGpuCore->setPen(mColorGpu);
		mpCurveClockGpuMemory->setPen(mColorGpuAlternative);
		mpCurveClockMicCore->setPen(mColorMic);
		mpCurveClockMicMemory->setPen(mColorMicAlternative);
		
		setupCurves(mpDataHandler->getSettings().mNumberOfTicks);
		
		mpCurveClockCpu0->attach(qwtPlot);
		mpCurveClockCpu1->attach(qwtPlot);
		mpCurveClockGpuCore->attach(qwtPlot);
		mpCurveClockGpuMemory->attach(qwtPlot);
		mpCurveClockMicCore->attach(qwtPlot);
		mpCurveClockMicMemory->attach(qwtPlot);
		
		qwtPlot->insertLegend(mpLegend, QwtPlot::BottomLegend);
	}
	
	QMSMFormClock::~QMSMFormClock(void) {
		delete mpCurveClockCpu0;
		delete mpCurveClockCpu1;
		delete mpCurveClockGpuCore;
		delete mpCurveClockGpuMemory;
		delete mpCurveClockMicCore;
		delete mpCurveClockMicMemory;
		
		spFormClock = 0;
	}
	
	QMSMFormClock *QMSMFormClock::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormClock == 0) {
			spFormClock = new QMSMFormClock(pParent, pDataHandler);
		}
		
		return spFormClock;
	}
	
	void QMSMFormClock::setupCurves(uint32_t currentTick) {
		uint32_t ticks				= mpDataHandler->getSettings().mNumberOfTicks-1;
		
		double *x					= mpDataHandler->getMeasurement().mpX+currentTick;
		double *y_clock_cpu_0		= mpDataHandler->getMeasurement().mpYClockCpu0+currentTick;
		double *y_clock_cpu_1		= mpDataHandler->getMeasurement().mpYClockCpu1+currentTick;
		double *y_clock_gpu_core	= mpDataHandler->getMeasurement().mpYClockGpuCore+currentTick;
		double *y_clock_gpu_mem		= mpDataHandler->getMeasurement().mpYClockGpuMem+currentTick;
		double *y_clock_mic_core	= mpDataHandler->getMeasurement().mpYClockMicCore+currentTick;
		double *y_clock_mic_mem		= mpDataHandler->getMeasurement().mpYClockMicMem+currentTick;
		
		mpCurveClockCpu0->setRawData(x, y_clock_cpu_0, ticks);
		mpCurveClockCpu1->setRawData(x, y_clock_cpu_1, ticks);
		mpCurveClockGpuCore->setRawData(x, y_clock_gpu_core, ticks);
		mpCurveClockGpuMemory->setRawData(x, y_clock_gpu_mem, ticks);
		mpCurveClockMicCore->setRawData(x, y_clock_mic_core, ticks);
		mpCurveClockMicMemory->setRawData(x, y_clock_mic_mem, ticks);
		
		scaleAxis(x[ticks-1], mpDataHandler->getSettings().mYAxisClockMin, mpDataHandler->getSettings().mYAxisClockMax);
	}
	
	double QMSMFormClock::getMiddleOfYAxis(void) {
		double min	= mpDataHandler->getSettings().mYAxisClockMin;
		double max	= mpDataHandler->getSettings().mYAxisClockMax;
		
		return min+((max-min)/2.0);
	}
}
