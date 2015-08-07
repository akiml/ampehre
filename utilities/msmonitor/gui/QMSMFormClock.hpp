/*
 * QMSMFormClock.hpp
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

#ifndef __QMSMFORMCLOCK_HPP__
#define __QMSMFORMCLOCK_HPP__

#include "QMSMFormMeasurementAbstract.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormClock : public QMSMFormMeasurementAbstract {
		Q_OBJECT
		
		private:
			static QMSMFormClock* spFormClock;
			
			QwtPlotCurve *mpCurveClockCpu0;
			QwtPlotCurve *mpCurveClockCpu1;
			QwtPlotCurve *mpCurveClockGpuCore;
			QwtPlotCurve *mpCurveClockGpuMemory;
			QwtPlotCurve *mpCurveClockMicCore;
			QwtPlotCurve *mpCurveClockMicMemory;
			
		private:
			explicit QMSMFormClock(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		public:
			~QMSMFormClock(void);
			
			static QMSMFormClock* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		protected:
			void setupCurves(uint32_t currentTick);
			double getMiddleOfYAxis(void);
	};
}

#endif /* __QMSMFORMCLOCK_HPP__ */
