/*
 * QMSMFormPower.hpp
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
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#ifndef __QMSMFORMPOWER_HPP__
#define __QMSMFORMPOWER_HPP__

#include "QMSMFormMeasurementAbstract.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormPower : public QMSMFormMeasurementAbstract {
		Q_OBJECT
		
		private:
			static QMSMFormPower* spFormPower;
			
			QwtPlotCurve *mpCurvePowerCpu0;
			QwtPlotCurve *mpCurvePowerCpu1;
			QwtPlotCurve *mpCurvePowerGpu;
			QwtPlotCurve *mpCurvePowerFpga;
			QwtPlotCurve *mpCurvePowerMic;
			QwtPlotCurve *mpCurvePowerSystem;
			
		private:
			explicit QMSMFormPower(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		public:
			~QMSMFormPower(void);
			
			static QMSMFormPower* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		protected:
			void setupCurves();
			double getMiddleOfYAxis(void);
	};
}

#endif /* __QMSMFORMPOWER_HPP__ */
