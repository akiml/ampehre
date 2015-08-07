/*
 * QMSMFormSettings.hpp
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

#ifndef __QMSMFORMSETTINGS_HPP__
#define __QMSMFORMSETTINGS_HPP__

#include <QWidget>
#include <QMdiSubWindow>
#include "ui_formsettings.h"

#include <QCloseEvent>

#include "../data/CDataSettings.hpp"

namespace Ui {
	class QMSMFormSettings : public QMdiSubWindow, private FormSettings {
		Q_OBJECT
		
		private:
			QWidget* mpFormSettings;
			NData::CDataSettings& mrDataSettings;
			
			QPalette mTextBlack;
			QPalette mTextRed;
			
			static QMSMFormSettings* spFormSettings;
		
		protected:
			explicit QMSMFormSettings(QWidget* pParent, NData::CDataSettings& rDataSettings);
			~QMSMFormSettings(void);
			
		public:
			static QMSMFormSettings* construct(QWidget *pParent, NData::CDataSettings& rDataSettings);
			
		private:
			void createActions(void);
			
		private slots:
			void storeSettings(void);
			void updateValues(void);
			
		protected slots:
			void close(void);
			void closeEvent(QCloseEvent *pEvent);
			bool event(QEvent *pEvent);
	};
}

#endif /* __QMSMFORMSETTINGS_HPP__ */
