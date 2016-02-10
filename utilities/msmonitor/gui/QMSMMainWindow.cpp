/*
 * QMSMMainWindow.cpp
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
 * created: 1/16/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.7.3 - add enum for ipmi_timeout_setting in libmeasure
 */

#include "QMSMMainWindow.hpp"

#include "../../../include/ms_version.h"

#include "QMSMFormInfo.hpp"
#include "QMSMFormSettings.hpp"

#include "QMSMFormPower.hpp"
#include "QMSMFormTemperature.hpp"
#include "QMSMFormClock.hpp"
#include "QMSMFormUtilization.hpp"
#include "QMSMFormMemory.hpp"
#include "QMSMFormHeatmapUtilization.hpp"
#include "QMSMFormHeatmapTemperature.hpp"

namespace Ui {
	QMSMMainWindow::QMSMMainWindow(QWidget* pParent, NData::CDataHandler& rDataHandler, QApplication* pApplication) :
		QMainWindow(pParent),
		MainWindow(),
		mpApplication(pApplication),
		mpFormInfo(0),
		mpFormSettings(0),
		mrDataHandler(rDataHandler),
		mpFormPower(0),
		mpFormTemperature(0),
		mpFormClock(0),
		mpFormUtilization(0),
		mpFormMemory(0),
		mpFormHeatmapUtilization(0),
		mpFormHeatmapTemperature(0),
		mGuiMarker(this)
		{
		
		setupUi(this);
		
		QString windowTitleKey = "MSMonitor v";
		windowTitleKey += QString::number(MS_MAJOR_VERSION);
		windowTitleKey += ".";
		windowTitleKey += QString::number(MS_MINOR_VERSION);
		windowTitleKey += ".";
		windowTitleKey += QString::number(MS_REVISION_VERSION);
		
		setWindowTitle(QApplication::translate("MainWindow", qPrintable(windowTitleKey), 0, QApplication::UnicodeUTF8));
		
		mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		
		pushButtonStop->setEnabled(false);
		
		createActions();
		createShortcuts();
		
		mpFormInfo					= QMSMFormInfo::construct(this);
		mpFormSettings				= QMSMFormSettings::construct(this, mrDataHandler.getSettings());
		mpFormPower					= QMSMFormPower::construct(this, &mrDataHandler);
		mpFormTemperature			= QMSMFormTemperature::construct(this, &mrDataHandler);
		mpFormClock					= QMSMFormClock::construct(this, &mrDataHandler);
		mpFormUtilization			= QMSMFormUtilization::construct(this, &mrDataHandler);
		mpFormMemory				= QMSMFormMemory::construct(this, &mrDataHandler);
		mpFormHeatmapUtilization	= QMSMFormHeatmapUtilization::construct(this, &mrDataHandler);
		mpFormHeatmapTemperature	= QMSMFormHeatmapTemperature::construct(this, &mrDataHandler);
		
		mdiArea->addSubWindow(mpFormInfo);
		mdiArea->addSubWindow(mpFormSettings);
		mdiArea->addSubWindow(mpFormPower);
		mdiArea->addSubWindow(mpFormTemperature);
		mdiArea->addSubWindow(mpFormClock);
		mdiArea->addSubWindow(mpFormUtilization);
		mdiArea->addSubWindow(mpFormMemory);
		mdiArea->addSubWindow(mpFormHeatmapUtilization);
		mdiArea->addSubWindow(mpFormHeatmapTemperature);
		
		mpFormInfo->hide();
		mpFormSettings->hide();
		mpFormPower->hide();
		mpFormTemperature->hide();
		mpFormClock->hide();
		mpFormUtilization->hide();
		mpFormMemory->hide();
		mpFormHeatmapUtilization->hide();
		mpFormHeatmapTemperature->hide();
		
		QString status_message("Listening on socket \"");
		status_message += mGuiMarker.getSocketName();
		status_message += "\".";
		QLabel status_label(status_message);
		statusBar()->addPermanentWidget(&status_label);
		
		show();
		
		statusbar->showMessage(status_message);
	}
	
	QMSMMainWindow::~QMSMMainWindow() {
		mrDataHandler.stopCollectData();
		mGuiMarker.stop();
	}
	
	void QMSMMainWindow::createActions(void) {
		connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(quitApplication()));
		connect(actionSettings, SIGNAL(triggered(bool)), this, SLOT(showSettings()));
		connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));
		
		connect(actionPower, SIGNAL(triggered(bool)), this, SLOT(showPower()));
		connect(actionTemperature, SIGNAL(triggered(bool)), this, SLOT(showTemperature()));
		connect(actionClock, SIGNAL(triggered(bool)), this, SLOT(showClock()));
		connect(actionUtilization, SIGNAL(triggered(bool)), this, SLOT(showUtilization()));
		connect(actionMemory, SIGNAL(triggered(bool)), this, SLOT(showMemory()));
		connect(actionHeatmapUtilization, SIGNAL(triggered(bool)), this, SLOT(showHeatmapUtilization()));
		connect(actionHeatmapTemperature, SIGNAL(triggered(bool)), this, SLOT(showHeatmapTemperature()));
		
		connect(pushButtonStart, SIGNAL(clicked(bool)), this, SLOT(pressStart()));
		connect(pushButtonStop, SIGNAL(clicked(bool)), this, SLOT(pressStop()));
		connect(pushButtonReset, SIGNAL(clicked(bool)), this, SLOT(pressReset()));
	}
	
	void QMSMMainWindow::createShortcuts(void) {
		actionQuit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
		actionSettings->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
		actionAbout->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
		
		actionPower->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
		actionTemperature->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
		actionClock->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
		actionUtilization->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
		actionMemory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
		actionHeatmapUtilization->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_U));
		actionHeatmapTemperature->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
		
		pushButtonStart->setShortcut(QKeySequence(Qt::Key_PageUp));
		pushButtonStop->setShortcut(QKeySequence(Qt::Key_PageDown));
		pushButtonReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	}
	
	void QMSMMainWindow::quitApplication(void) {
		mpApplication->exit();
	}
	
	void QMSMMainWindow::showSettings(void) {
		mpFormSettings->show();
	}
	
	void QMSMMainWindow::showAbout(void) {
		mpFormInfo->show();
	}
	
	void QMSMMainWindow::pressStart(void) {
		pushButtonStart->setEnabled(false);
		pushButtonReset->setEnabled(false);
		pushButtonStop->setEnabled(true);
		
		mrDataHandler.resetCollectData();
		mrDataHandler.startCollectData();
		
		mpFormPower->resetMarker();
		mpFormTemperature->resetMarker();
		mpFormClock->resetMarker();
		mpFormUtilization->resetMarker();
		mpFormMemory->resetMarker();
		
		mpFormPower->startTimer();
		mpFormTemperature->startTimer();
		mpFormClock->startTimer();
		mpFormUtilization->startTimer();
		mpFormMemory->startTimer();
		
		mpFormHeatmapUtilization->startTimer();
		mpFormHeatmapTemperature->startTimer();
		
		mGuiMarker.start();
	}
	
	void QMSMMainWindow::pressStop(void) {
		pushButtonStop->setEnabled(false);
		pushButtonReset->setEnabled(true);
		pushButtonStart->setEnabled(true);
		
		mpFormPower->stopTimer();
		mpFormTemperature->stopTimer();
		mpFormUtilization->stopTimer();
		mpFormClock->stopTimer();
		mpFormMemory->stopTimer();
		mpFormHeatmapUtilization->stopTimer();
		mpFormHeatmapTemperature->stopTimer();
		
		mpFormPower->joinTimer();
		mpFormTemperature->joinTimer();
		mpFormClock->joinTimer();
		mpFormUtilization->joinTimer();
		mpFormMemory->joinTimer();
		mpFormHeatmapUtilization->joinTimer();
		mpFormHeatmapTemperature->joinTimer();
		
		mrDataHandler.stopCollectData();
		
		mGuiMarker.stop();
	}
	
	void QMSMMainWindow::pressReset(void) {
		mrDataHandler.resetCollectData();
		
		mpFormPower->resetMarker();
		mpFormTemperature->resetMarker();
		mpFormClock->resetMarker();
		mpFormUtilization->resetMarker();
	}
	
	void QMSMMainWindow::showPower(void) {
		mpFormPower->show();
	}
	
	void QMSMMainWindow::showTemperature(void) {
		mpFormTemperature->show();
	}
	
	void QMSMMainWindow::showClock(void) {
		mpFormClock->show();
	}
	
	void QMSMMainWindow::showUtilization(void) {
		mpFormUtilization->show();
	}
	
	void QMSMMainWindow::showMemory(void) {
		mpFormMemory->show();
	}
	
    void QMSMMainWindow::showHeatmapUtilization(void) {
		mpFormHeatmapUtilization->show();
    }
    
    void QMSMMainWindow::showHeatmapTemperature(void) {
		mpFormHeatmapTemperature->show();
    }

	
	bool QMSMMainWindow::event(QEvent *pEvent) {
		if (pEvent->type() == QEvent::ToolTip) {
			return true;
		}
		
		return QMainWindow::event(pEvent);
	}
	
	void QMSMMainWindow::addMarker(int pid, int id, int signal) {
		QString label("");
		
		if (signal == 0) {
			label += "< ";
		}
		
		label += QString::number(pid);
		label += ":";
		label += QString::number(id);
		
		if (signal == 1) {
			label += " >";
		} else if (signal != 0) {
			label += ":";
			label += QString::number(signal);
		}
		
		mpFormPower->triggerMarkerSignal(label);
		mpFormTemperature->triggerMarkerSignal(label);
		mpFormClock->triggerMarkerSignal(label);
		mpFormUtilization->triggerMarkerSignal(label);
		mpFormMemory->triggerMarkerSignal(label);
	}
}
