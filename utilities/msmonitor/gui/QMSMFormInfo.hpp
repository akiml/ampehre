/*
 * QMSMFormInfo.hpp
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
 * created: 1/26/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 */

#ifndef __QMSMFORMINFO_HPP__
#define __QMSMFORMINFO_HPP__

#include <QWidget>
#include <QMdiSubWindow>
#include "ui_forminfo.h"

#include <QCloseEvent>

namespace Ui {
	class QMSMFormInfo : public QMdiSubWindow, private FormInfo {
		Q_OBJECT
		
		private:
			QWidget* mpFormInfo;
			
			static QMSMFormInfo* spFormInfo;
		
		protected:
			explicit QMSMFormInfo(QWidget* pParent);
			~QMSMFormInfo(void);
			
		public:
			static QMSMFormInfo* construct(QWidget *pParent);
			
		private:
			void createActions(void);
			
		protected slots:
			void close(void);
			void closeEvent(QCloseEvent *pEvent);
			bool event(QEvent *pEvent);
	};
}

#endif /* __QMSMFORMINFO_HPP__ */
