/*
 * QMSMFormSettings.cpp
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
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 *          0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#include "QMSMFormSettings.hpp"

namespace Ui {
	QMSMFormSettings* QMSMFormSettings::spFormSettings = 0;
	
	QMSMFormSettings::QMSMFormSettings(QWidget *pParent, NData::CDataSettings& rDataSettings) :
		QMdiSubWindow(pParent),
		FormSettings(),
		mpFormSettings(new QWidget(pParent)),
		mrDataSettings(rDataSettings),
		mTextBlack(),
		mTextRed()
		{
		
		mTextBlack.setColor(QPalette::WindowText, Qt::black);
		mTextRed.setColor(QPalette::WindowText, Qt::red);
		
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
		
		setupUi(mpFormSettings);
		
		horizontalSliderCPU->setValue(mrDataSettings.mCPUSamplingRate/10);
		horizontalSliderGPU->setValue(mrDataSettings.mGPUSamplingRate/10);
		horizontalSliderFPGA->setValue(mrDataSettings.mFPGASamplingRate/10);
		horizontalSliderMIC->setValue(mrDataSettings.mMICSamplingRate/10);
		horizontalSliderSystem->setValue(mrDataSettings.mSystemSamplingRate/10);
		horizontalSliderData->setValue(mrDataSettings.mDataSamplingRate/10);
		horizontalSliderHeatmap->setValue(mrDataSettings.mHeatmapSamplingRate/30);
		horizontalSliderGUI->setValue(mrDataSettings.mGUIRefreshRate/50);
		
		updateValues();
		
		setWidget(mpFormSettings);
		
		createActions();
	}
	
	QMSMFormSettings::~QMSMFormSettings(void) {
		delete mpFormSettings;
		
		spFormSettings = 0;
	}
	
	QMSMFormSettings *QMSMFormSettings::construct(QWidget *pParent, NData::CDataSettings& rDataSettings) {
		if (spFormSettings == 0) {
			spFormSettings = new QMSMFormSettings(pParent, rDataSettings);
		}
		
		return spFormSettings;
	}
	
	void QMSMFormSettings::createActions(void) {
		connect(pushButtonOK           , SIGNAL(clicked(bool))    , this, SLOT(storeSettings()));
		connect(pushButtonCancel       , SIGNAL(clicked(bool))    , this, SLOT(close()));
		
		connect(horizontalSliderCPU    , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderGPU    , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderFPGA   , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderMIC    , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderSystem , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderData   , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderHeatmap, SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
		connect(horizontalSliderGUI    , SIGNAL(valueChanged(int)), this, SLOT(updateValues()));
	}
	
	void QMSMFormSettings::storeSettings(void) {
		mrDataSettings.lock();
		mrDataSettings.mCPUSamplingRate		= 10*horizontalSliderCPU->value();
		mrDataSettings.mGPUSamplingRate		= 10*horizontalSliderGPU->value();
		mrDataSettings.mFPGASamplingRate	= 10*horizontalSliderFPGA->value();
		mrDataSettings.mMICSamplingRate		= 10*horizontalSliderMIC->value();
		mrDataSettings.mSystemSamplingRate	= 10*horizontalSliderSystem->value();
		mrDataSettings.mDataSamplingRate	= 10*horizontalSliderData->value();
		mrDataSettings.mHeatmapSamplingRate	= 30*horizontalSliderHeatmap->value();
		mrDataSettings.mNumberOfTicks		= mrDataSettings.mTimeToBufferData/mrDataSettings.mDataSamplingRate;
		mrDataSettings.mGUIRefreshRate		= 50*horizontalSliderGUI->value();
		mrDataSettings.mWriteResultsToCsv	= checkBoxPrintToCsv->isChecked();
		mrDataSettings.unlock();
		
		close();
	}
	
	void QMSMFormSettings::updateValues(void) {
		labelSamplingCPUCurrent->setNum(10*horizontalSliderCPU->value());
		labelSamplingGPUCurrent->setNum(10*horizontalSliderGPU->value());
		labelSamplingFPGACurrent->setNum(10*horizontalSliderFPGA->value());
		labelSamplingMICCurrent->setNum(10*horizontalSliderMIC->value());
		labelSamplingSystemCurrent->setNum(10*horizontalSliderSystem->value());
		labelSamplingDataCurrent->setNum(10*horizontalSliderData->value());
		labelSamplingHeatmapCurrent->setNum(30*horizontalSliderHeatmap->value());
		labelRefreshGUICurrent->setNum(50*horizontalSliderGUI->value());
		
		if (10*horizontalSliderCPU->value() < 20) {
			labelSamplingCPUCurrent->setPalette(mTextRed);
			labelCPUMs->setPalette(mTextRed);
		} else {
			labelSamplingCPUCurrent->setPalette(mTextBlack);
			labelCPUMs->setPalette(mTextBlack);
		}
		
		if (10*horizontalSliderGPU->value() < 30) {
			labelSamplingGPUCurrent->setPalette(mTextRed);
			labelGPUMs->setPalette(mTextRed);
		} else {
			labelSamplingGPUCurrent->setPalette(mTextBlack);
			labelGPUMs->setPalette(mTextBlack);
		}
		
		if (10*horizontalSliderFPGA->value() < 50) {
			labelSamplingFPGACurrent->setPalette(mTextRed);
			labelFPGAMs->setPalette(mTextRed);
		} else {
			labelSamplingFPGACurrent->setPalette(mTextBlack);
			labelFPGAMs->setPalette(mTextBlack);
		}
		
		if (10*horizontalSliderMIC->value() < 50) {
			labelSamplingMICCurrent->setPalette(mTextRed);
			labelMICMs->setPalette(mTextRed);
		} else {
			labelSamplingMICCurrent->setPalette(mTextBlack);
			labelMICMs->setPalette(mTextBlack);
		}
		
		if (10*horizontalSliderSystem->value() < 100) {
			labelSamplingSystemCurrent->setPalette(mTextRed);
			labelSystemMs->setPalette(mTextRed);
		} else {
			labelSamplingSystemCurrent->setPalette(mTextBlack);
			labelSystemMs->setPalette(mTextBlack);
		}
		
		if (10*horizontalSliderData->value() < 30) {
			labelSamplingDataCurrent->setPalette(mTextRed);
			labelDataMs->setPalette(mTextRed);
		} else {
			labelSamplingDataCurrent->setPalette(mTextBlack);
			labelDataMs->setPalette(mTextBlack);
		}
		
		if (30*horizontalSliderHeatmap->value() < 30) {
			labelSamplingHeatmapCurrent->setPalette(mTextRed);
			labelHeatmapMs->setPalette(mTextRed);
		} else {
			labelSamplingHeatmapCurrent->setPalette(mTextBlack);
			labelHeatmapMs->setPalette(mTextBlack);
		}
		
		if (50*horizontalSliderGUI->value() < 150) {
			labelRefreshGUICurrent->setPalette(mTextRed);
			labelGUIMs->setPalette(mTextRed);
		} else {
			labelRefreshGUICurrent->setPalette(mTextBlack);
			labelGUIMs->setPalette(mTextBlack);
		}
	}
	
	void QMSMFormSettings::close(void) {
		hide();
	}
	
	void QMSMFormSettings::closeEvent(QCloseEvent *pEvent) {
		hide();
	}
	
	bool QMSMFormSettings::event(QEvent *pEvent) {
		if (pEvent->type() == QEvent::ToolTip) {
			return true;
		}
		
		return QMdiSubWindow::event(pEvent);
	}
}
