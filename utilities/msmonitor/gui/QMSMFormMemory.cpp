/*
 * QMSMFormMemory.cpp
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 5/05/14
 * version: 0.5.0 - add cpu, gpu and mic memory information
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#include "QMSMFormMemory.hpp"

#include "../data/CDataHandler.hpp"

namespace Ui {
	QMSMFormMemory* QMSMFormMemory::spFormMemory = 0;
	
	QMSMFormMemory::QMSMFormMemory(QWidget *pParent, NData::CDataHandler *pDataHandler) :
		QMSMFormMeasurementAbstract(pParent, pDataHandler),
		mpCurveMemoryCpu(new QwtPlotCurve("CPU RAM")),
		mpCurveSwapCpu(new QwtPlotCurve("CPU SWAP")),
		mpCurveMemoryGpu(new QwtPlotCurve("GPU")),
		mpCurveMemoryMic(new QwtPlotCurve("MIC"))
		{
		
		setWindowTitle(QApplication::translate("FormMeasurement", "Memory", 0, QApplication::UnicodeUTF8));
		
		qwtPlot->setAxisTitle(QwtPlot::yLeft, createTitle(QString::fromUtf8("Memory [MiB]")));
		
		mpCurveMemoryCpu->setPen(mColorCpu);
		mpCurveSwapCpu->setPen(mColorCpuAlternative);
		mpCurveMemoryGpu->setPen(mColorGpu);
		mpCurveMemoryMic->setPen(mColorMic);
		
		setupCurves();
		
		mpCurveMemoryCpu->attach(qwtPlot);
		mpCurveSwapCpu->attach(qwtPlot);
		mpCurveMemoryGpu->attach(qwtPlot);
		mpCurveMemoryMic->attach(qwtPlot);
		
		qwtPlot->insertLegend(mpLegend, QwtPlot::BottomLegend);
	}
	
	QMSMFormMemory::~QMSMFormMemory(void) {
		delete mpCurveMemoryCpu;
		delete mpCurveSwapCpu;
		delete mpCurveMemoryGpu;
		delete mpCurveMemoryMic;
		
		spFormMemory = 0;
	}
	
	QMSMFormMemory *QMSMFormMemory::construct(QWidget *pParent, NData::CDataHandler *pDataHandler) {
		if (spFormMemory == 0) {
			spFormMemory = new QMSMFormMemory(pParent, pDataHandler);
		}
		
		return spFormMemory;
	}
	
	void QMSMFormMemory::setupCurves() {
		uint32_t ticks			= mpDataHandler->getSettings().mNumberOfTicks-1;
		
		double *x				= mpDataHandler->getMeasurement().mpX->getDataPtr();
		double *y_memory_cpu	= mpDataHandler->getMeasurement().mpYMemoryCpu->getDataPtr();
		double *y_swap_cpu		= mpDataHandler->getMeasurement().mpYSwapCpu->getDataPtr();
		double *y_memory_gpu	= mpDataHandler->getMeasurement().mpYMemoryGpu->getDataPtr();
		double *y_memory_mic	= mpDataHandler->getMeasurement().mpYMemoryMic->getDataPtr();
				
		mpCurveMemoryCpu->setRawSamples(x, y_memory_cpu, ticks);
		mpCurveSwapCpu->setRawSamples(x, y_swap_cpu, ticks);
		mpCurveMemoryGpu->setRawSamples(x, y_memory_gpu, ticks);
		mpCurveMemoryMic->setRawSamples(x, y_memory_mic, ticks);
				
		scaleAxis(x[ticks-1], mpDataHandler->getSettings().mYAxisMemoryMin, mpDataHandler->getSettings().mYAxisMemoryMax);
	}
	
	double QMSMFormMemory::getMiddleOfYAxis(void) {
		double min	= mpDataHandler->getSettings().mYAxisMemoryMin;
		double max	= mpDataHandler->getSettings().mYAxisMemoryMax;
		
		return min+((max-min)/2.0);
	}
}
