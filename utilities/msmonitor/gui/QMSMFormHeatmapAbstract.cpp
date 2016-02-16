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
		mHeatmaps()
		{
		
		//setAttribute(Qt::WA_DeleteOnClose);
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
		
		setupUi(mpFormHeatmapAbstract);
		
		setWidget(mpFormHeatmapAbstract);
		
		createActions();
	}
	
	QMSMFormHeatmapAbstract::~QMSMFormHeatmapAbstract(void) {
		for(std::vector<Ui::Heatmap*>::iterator it = mHeatmaps.begin(); it != mHeatmaps.end(); ++it) {
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
		setupHeatmaps();
		
		for(std::vector<Ui::Heatmap*>::iterator it = mHeatmaps.begin(); it != mHeatmaps.end(); ++it) {
			(*it)->refresh();
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
	
    Heatmap* QMSMFormHeatmapAbstract::addHeatmap(const QString &title) {
		Ui::Heatmap *heatmap = new Heatmap(this);
		
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
