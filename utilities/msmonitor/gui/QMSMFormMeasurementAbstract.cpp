/*
 * QMSMFormMeasurementAbstract.cpp
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
 * created: 1/28/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.3.2 - add a networking component to show influence of a task to measurements in GUI
 *          0.4.1 - add MIC support to msmonitor
 *          0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#include "QMSMFormMeasurementAbstract.hpp"

#include "../data/CDataHandler.cpp"

#include <iostream>

namespace Ui {
	uint32_t QMSMFormMeasurementAbstract::sNumberOfImages = 0;
	
	QMSMFormMeasurementAbstract::QMSMFormMeasurementAbstract(QWidget *pParent, NData::CDataHandler* pDataHandler) :
		QMdiSubWindow(pParent),
		FormMeasurement(),
		mpFormMeasurementAbstract(new QWidget(pParent)),
		mpDataHandler(pDataHandler),
		mTimer(mpDataHandler->getSettings().mGUIRefreshRate, this),
		mColorCpu(0, 0, 255),
		mColorCpuAlternative(0, 128, 255),
		mColorGpu(255, 0, 0),
		mColorGpuAlternative(153, 0, 0),
		mColorFpga(0, 153, 0),
		mColorFpgaAlternative(0, 255, 0),
		mColorMic(153, 0, 255),
		mColorMicAlternative(255, 0, 255),
		mColorSystem(255, 128, 0),
		mColorSystemAlternative(255, 255, 0),
		mVerticalLine(QwtSymbol::VLine, QBrush(Qt::black), QPen(Qt::black), QSize(1, 300)),
		mMarker(),
		mMarkerLabel(""),
		mMarkerTime(0),
		mpLegend(new QwtLegend()),
		mpPanner(0),
		mpMagnifier(0),
		mpGrid(new QwtPlotGrid())
		{
		
		//setAttribute(Qt::WA_DeleteOnClose);
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
		
		setupUi(mpFormMeasurementAbstract);
		
		setWidget(mpFormMeasurementAbstract);
		
		qwtPlot->setAxisTitle(QwtPlot::xBottom, createTitle(QString::fromUtf8("Time [s]")));
		
		mpLegend->setFrameStyle(QFrame::Box | QFrame::Sunken);
		
		mpPanner	= new QwtPlotPanner(qwtPlot->canvas());
		mpMagnifier = new QwtPlotMagnifier(qwtPlot->canvas());
		
		mpGrid->enableXMin(true);
		mpGrid->enableYMin(true);
		mpGrid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
		mpGrid->setMinPen(QPen(Qt::gray, 0, Qt::DotLine));
		mpGrid->attach(qwtPlot);
		
		createActions();
	}
	
	QMSMFormMeasurementAbstract::~QMSMFormMeasurementAbstract(void) {
		delete mpLegend;
		delete mpPanner;
		delete mpMagnifier;
		delete mpGrid;
		delete mpFormMeasurementAbstract;
	}
	
	void QMSMFormMeasurementAbstract::createActions(void) {
		connect(pushButtonOK, SIGNAL(clicked(bool)), this, SLOT(close()));
		connect(pushButtonSave, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
		connect(this, SIGNAL(signalRefreshGui(void)), this, SLOT(slotRefreshGui()));
		connect(this, SIGNAL(signalAddMarker(void)), this, SLOT(addMarker()));
	}
	
	QwtText QMSMFormMeasurementAbstract::createTitle(QString title) {
		QwtText qTitle(title);
		
		qTitle.setFont(QFont("Helvetica", 12));
		
		return qTitle;
	}
	
	void QMSMFormMeasurementAbstract::close(void) {
		hide();
	}
	
	void QMSMFormMeasurementAbstract::closeEvent(QCloseEvent *pEvent) {
		hide();
	}
	
	bool QMSMFormMeasurementAbstract::event(QEvent *pEvent) {
		if (pEvent->type() == QEvent::ToolTip) {
			return true;
		}
		
		return QMdiSubWindow::event(pEvent);
	}
	
	void QMSMFormMeasurementAbstract::refreshGui(void) {
		emit signalRefreshGui();
	}
	
	void QMSMFormMeasurementAbstract::slotRefreshGui(void) {
		setupCurves();
		
		qwtPlot->replot();
	}
	
	void QMSMFormMeasurementAbstract::saveImage(void) {
		const char *home_directory = getenv("HOME");
		
		QString filename(home_directory);
		filename += "/plot_";
		filename += QString::number(sNumberOfImages++);
		filename += ".png";
		
		QPixmap pixmap(mpDataHandler->getSettings().mImageSizeX, mpDataHandler->getSettings().mImageSizeY);
		
		
		QwtPlotPrintFilter filter;
		//filter.color(Qt::white, QwtPlotPrintFilter::CanvasBackground);
		//filter.setOptions(QwtPlotPrintFilter::PrintBackground);
		filter.setOptions(QwtPlotPrintFilter::PrintAll);
		
		qwtPlot->print(pixmap, filter);
		
		if (pixmap.save(filename, "png")) {
			std::cout << "INFO : Image stored as '" << qPrintable(filename) << "'." << std::endl;
		} else {
			std::cerr << "ERROR: Could not store '" << qPrintable(filename) << "'!" << std::endl;
		}
	}
	
	void QMSMFormMeasurementAbstract::scaleAxis(double xValue, double yValueMin, double yValueMax) {
		int32_t first_tick		= 0;
		int32_t second_tick		= 0;
		double time_show_data	= 0.0;
		
		time_show_data	= mpDataHandler->getSettings().mTimeToShowData/1000.0;
		
		first_tick		= ((int)(xValue-time_show_data)/10)*10;
		first_tick		= (first_tick >= 0 && (xValue-time_show_data) > 0) ? first_tick+10 : first_tick;
		second_tick		= ((int)(xValue)/10)*10+10;
		
		qwtPlot->setAxisScale(QwtPlot::xBottom, first_tick, second_tick);
		
		qwtPlot->setAxisScale(QwtPlot::yLeft, yValueMin, yValueMax);
	}
	
	void QMSMFormMeasurementAbstract::startTimer(void) {
		mTimer.startTimer();
	}
	
	void QMSMFormMeasurementAbstract::stopTimer(void) {
		mTimer.stopTimer();
	}
	
	void QMSMFormMeasurementAbstract::joinTimer(void) {
		mTimer.joinTimer();
	}
	
	void QMSMFormMeasurementAbstract::addMarker(void) {
		QwtPlotMarker *marker = new QwtPlotMarker();
		
		marker->setSymbol(mVerticalLine);
		marker->setLabel(QwtText(mMarkerLabel));
		marker->setLabelAlignment(Qt::AlignTop);
		marker->setValue(mMarkerTime, getMiddleOfYAxis());
		marker->attach(qwtPlot);
		
		mMarker.push_back(marker);
	}
	
	void QMSMFormMeasurementAbstract::triggerMarkerSignal(QString &rLabel) {
		mMarkerTime		= mpDataHandler->getMeasurement().getTime();
		mMarkerLabel	= rLabel;
		
		emit signalAddMarker();
	}
	
	void QMSMFormMeasurementAbstract::resetMarker(void) {
		for(std::vector<QwtPlotMarker *>::iterator it = mMarker.begin(); it != mMarker.end(); ++it) {
			(*it)->setVisible(false);
		}
	}
}
