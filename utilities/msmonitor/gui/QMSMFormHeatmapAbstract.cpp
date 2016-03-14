/*
 * QMSMFormHeatmapAbstract.cpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 01/27/16
 * version: 0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
 */

#include "QMSMFormHeatmapAbstract.hpp"

namespace Ui {
	uint32_t QMSMFormHeatmapAbstract::sNumberOfImages = 0;
	
	QMSMFormHeatmapAbstract::QMSMFormHeatmapAbstract(QWidget *pParent, NData::CDataHandler* pDataHandler) :
		QMdiSubWindow(pParent),
		FormHeatmap(),
		mpFormHeatmapAbstract(new QWidget(pParent)),
		mpDataHandler(pDataHandler),
		mTimer(mpDataHandler->getSettings().mGUIRefreshRate, this),
		mHeatmaps(),
		mCurrentX(0),
		mIndexCurrentX(0)
		{
		
		//setAttribute(Qt::WA_DeleteOnClose);
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
		
		setupUi(mpFormHeatmapAbstract);
		
		setWidget(mpFormHeatmapAbstract);
		
		createActions();
	}
	
	QMSMFormHeatmapAbstract::~QMSMFormHeatmapAbstract(void) {
		for(std::vector<Ui::QMSMHeatmap*>::iterator it = mHeatmaps.begin(); it != mHeatmaps.end(); ++it) {
			delete (*it);
		}
		delete mpFormHeatmapAbstract;
	}
	
	void QMSMFormHeatmapAbstract::createActions(void) {
		connect(pushButtonOK, SIGNAL(clicked(bool)), this, SLOT(close()));
		connect(pushButtonSave, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
		connect(this, SIGNAL(signalRefreshGui(void)), this, SLOT(slotRefreshGui()));
	}
	
	QwtText QMSMFormHeatmapAbstract::createTitle(QString title) {
		QwtText qTitle(title);
		
		qTitle.setFont(QFont("Helvetica", 12));
		
		return qTitle;
	}
	
	void QMSMFormHeatmapAbstract::updateCurrentX(void) {
		uint32_t bufferedSamples = ((mpDataHandler->getSettings().mTimeToBufferData - mpDataHandler->getSettings().mTimeToShowData) /
									mpDataHandler->getSettings().mDataSamplingRate)-1;
		uint32_t showSamples = (mpDataHandler->getSettings().mTimeToShowData / mpDataHandler->getSettings().mDataSamplingRate) - 1;
		double *x = mpDataHandler->getMeasurement().mpX->getDataPtr() + bufferedSamples;
		
		//shift x axis every 10 s
		if(x[showSamples] - mCurrentX > 10) {
			mCurrentX += 10;
		}
		
		//reset x axis if necessary 
		if(x[showSamples] < mCurrentX) {
			mCurrentX = 0;
		}
		
		//search the index corresponding to mCurrentX
		mIndexCurrentX = 0;
		for (int i = showSamples; i >= 0; i--) {
			if(x[i] <= (mCurrentX + ((double) mpDataHandler->getSettings().mDataSamplingRate/1000/3))){
				 mIndexCurrentX = i;
				 break;
			}
		}
	}
	
	void QMSMFormHeatmapAbstract::close(void) {
		hide();
	}
	
	void QMSMFormHeatmapAbstract::closeEvent(QCloseEvent *pEvent) {
		hide();
	}
	
	bool QMSMFormHeatmapAbstract::event(QEvent *pEvent) {
		if (pEvent->type() == QEvent::ToolTip) {
			return true;
		}
		
		return QMdiSubWindow::event(pEvent);
	}
	
	void QMSMFormHeatmapAbstract::refreshGui(void) {
		emit signalRefreshGui();
	}
	
	void QMSMFormHeatmapAbstract::slotRefreshGui(void) {
		if(isVisible()) {
			setupHeatmaps();
			
			for(std::vector<Ui::QMSMHeatmap*>::iterator it = mHeatmaps.begin(); it != mHeatmaps.end(); ++it) {
				(*it)->refresh();
			}
		} else {
			updateCurrentX();
		}
	}
	
	void QMSMFormHeatmapAbstract::saveImage(void) {
		//TODO needs to be implemented
	}
	
	void QMSMFormHeatmapAbstract::startTimer(void) {
		mTimer.startTimer();
	}
	
	void QMSMFormHeatmapAbstract::stopTimer(void) {
		mTimer.stopTimer();
	}
	
	void QMSMFormHeatmapAbstract::joinTimer(void) {
		mTimer.joinTimer();
	}
	
    QMSMHeatmap* QMSMFormHeatmapAbstract::addHeatmap(const QString &title) {
		Ui::QMSMHeatmap *heatmap = new QMSMHeatmap(this);
		
		heatmap->setObjectName(title);
		heatmap->setFrameShape(QFrame::NoFrame);
		heatmap->setFrameShadow(QFrame::Raised);
		heatmap->setTitle(title.toStdString());
		verticalLayoutMeasurement->insertWidget(mHeatmaps.size(), heatmap);
		
		mHeatmaps.push_back(heatmap);
		resize(650, 50 + 110 * mHeatmaps.size());
		
		return heatmap;
	}
}
