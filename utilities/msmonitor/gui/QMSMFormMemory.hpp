/*
 * QMSMFormMemory.hpp
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

#ifndef __QMSMFORMMEMORY_HPP__
#define __QMSMFORMMEMORY_HPP__

#include "QMSMFormMeasurementAbstract.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormMemory : public QMSMFormMeasurementAbstract {
		Q_OBJECT
		
		private:
			static QMSMFormMemory* spFormMemory;
			
			QwtPlotCurve *mpCurveMemoryCpu;
			QwtPlotCurve *mpCurveSwapCpu;
			QwtPlotCurve *mpCurveMemoryGpu;
			QwtPlotCurve *mpCurveMemoryMic;
			
		private:
			explicit QMSMFormMemory(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		public:
			~QMSMFormMemory(void);
			
			static QMSMFormMemory* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		protected:
			void setupCurves();
			double getMiddleOfYAxis(void);
	};
}

#endif /* __QMSMFORMMEMORY_HPP__ */
