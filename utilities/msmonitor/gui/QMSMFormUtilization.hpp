/*
 * QMSMFormUtilization.hpp
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

#ifndef __QMSMFORMUTILIZATION_HPP__
#define __QMSMFORMUTILIZATION_HPP__

#include "QMSMFormMeasurementAbstract.hpp"

namespace NData {
	class CDataHandler;
}

namespace Ui {
	class QMSMFormUtilization : public QMSMFormMeasurementAbstract {
		Q_OBJECT
		
		private:
			static QMSMFormUtilization* spFormUtil;
			
			QwtPlotCurve *mpCurveUtilCpu;
			QwtPlotCurve *mpCurveUtilGpuCore;
			QwtPlotCurve *mpCurveUtilGpuMemory;
			QwtPlotCurve *mpCurveUtilFpga;
			QwtPlotCurve *mpCurveUtilMic;
			
		private:
			explicit QMSMFormUtilization(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		public:
			~QMSMFormUtilization(void);
			
			static QMSMFormUtilization* construct(QWidget *pParent, NData::CDataHandler *pDataHandler);
			
		protected:
			void setupCurves();
			double getMiddleOfYAxis(void);
	};
}

#endif /* __QMSMFORMUTILIZATION_HPP__ */
