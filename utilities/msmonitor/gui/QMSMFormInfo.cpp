/*
 * QMSMFormInfo.cpp
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

#include "QMSMFormInfo.hpp"

#include "../../../include/ms_version.h"

namespace Ui {
	QMSMFormInfo* QMSMFormInfo::spFormInfo = 0;
	
	QMSMFormInfo::QMSMFormInfo(QWidget *pParent) :
		QMdiSubWindow(pParent),
		FormInfo(),
		mpFormInfo(new QWidget(pParent))
		{
		
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
		
		setupUi(mpFormInfo);
		
		QString labelTextKey = "MSMonitor v";
		labelTextKey += QString::number(MS_MAJOR_VERSION);
		labelTextKey += ".";
		labelTextKey += QString::number(MS_MINOR_VERSION);
		labelTextKey += ".";
		labelTextKey += QString::number(MS_REVISION_VERSION);
		
		labelText->setText(QApplication::translate("FormInfo", qPrintable(labelTextKey), 0, QApplication::UnicodeUTF8));
		
		setWidget(mpFormInfo);
		
		createActions();
	}
	
	QMSMFormInfo::~QMSMFormInfo(void) {
		delete mpFormInfo;
		
		spFormInfo = 0;
	}
	
	QMSMFormInfo *QMSMFormInfo::construct(QWidget *pParent) {
		if (spFormInfo == 0) {
			spFormInfo = new QMSMFormInfo(pParent);
		}
		
		return spFormInfo;
	}
	
	void QMSMFormInfo::createActions(void) {
		connect(pushButtonOK, SIGNAL(clicked(bool)), this, SLOT(close()));
	}
	
	void QMSMFormInfo::close(void) {
		hide();
	}
	
	void QMSMFormInfo::closeEvent(QCloseEvent *pEvent) {
		hide();
	}
	
	bool QMSMFormInfo::event(QEvent *pEvent) {
		if (pEvent->type() == QEvent::ToolTip) {
			return true;
		}
		
		return QMdiSubWindow::event(pEvent);
	}
}
