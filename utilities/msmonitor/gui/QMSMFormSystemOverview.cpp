/*
 * QMSMFormSystemOverview.cpp
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
 * created: 02/24/16
 * version: 0.7.4 - add query for currently active processes to libmeasure and show them in msmonitor
 */

#include "QMSMFormSystemOverview.hpp"

namespace Ui {
	QMSMFormSystemOverview* QMSMFormSystemOverview::spFormSystemOverview = 0;
	
	QMSMFormSystemOverview::QMSMFormSystemOverview(QWidget *pParent, NData::CDataHandler* pDataHandler):
		QMdiSubWindow(pParent),
		FormSystemOverview(),
		mpFormSystemOverview(new QWidget(pParent)),
		mpDataHandler(pDataHandler),
		mTimer(mpDataHandler->getSettings().mGUIRefreshRate, this),
		mGPUProcessList()
		{
		
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
		
		this->setFixedHeight(640);
		this->setMinimumWidth(510);
		
		setupUi(mpFormSystemOverview);
		
		setWidget(mpFormSystemOverview);
		
		initGUI();
		
		createActions();
	}

	QMSMFormSystemOverview::~QMSMFormSystemOverview() {
		delete mpPixmapFPGAActivity;
		delete mpPixmapGPUActivity;
		
		delete mpPixmapCPUUtilization;
		delete mpPixmapGPUCoreUtilization;
		delete mpPixmapGPUMemUtilization;
		delete mpPixmapFPGAUtilization;
		delete mpPixmapMICUtilization;
		
		delete mpPixmapCPU0Temperature;
		delete mpPixmapCPU1Temperature;
		delete mpPixmapGPUTemperature;
		delete mpPixmapFPGAComputeTemperature;
		delete mpPixmapFPGAInterfaceTemperature;
		delete mpPixmapMICTemperature;
		
		delete mpColorMap;
		delete mpFormSystemOverview;
	}
	
    void QMSMFormSystemOverview::initGUI(void) {
		MS_CONFIG* pConfig = mpDataHandler->getLibrary().getMS_CONFIG();
		
		mpPainter = new QPainter();
		
		mpPixmapGPUActivity		= new QPixmap(35, 35);
		mpPixmapFPGAActivity	= new QPixmap(35, 35);
		
		mpPixmapCPUUtilization		= new QPixmap(60, 35);
		mpPixmapGPUCoreUtilization	= new QPixmap(60, 35);
		mpPixmapGPUMemUtilization	= new QPixmap(60, 35);
		mpPixmapFPGAUtilization		= new QPixmap(60, 35);
		mpPixmapMICUtilization		= new QPixmap(60, 35);
		
		mpPixmapCPU0Temperature			= new QPixmap(60, 35);
		mpPixmapCPU1Temperature			= new QPixmap(60, 35);
		mpPixmapGPUTemperature			= new QPixmap(60, 35);
		mpPixmapFPGAComputeTemperature 	= new QPixmap(60, 35);
		mpPixmapFPGAInterfaceTemperature= new QPixmap(60, 35);
		mpPixmapMICTemperature			= new QPixmap(60, 35);
		
		mpColorMap = new QwtLinearColorMap(Qt::blue, Qt::red);
		mpColorMap->addColorStop(0.3, Qt::cyan);
		mpColorMap->addColorStop(0.5, Qt::green);
		mpColorMap->addColorStop(0.7, Qt::yellow);
		
		setActivity(mpPixmapGPUActivity, labelGPUActivity, false);
		setActivity(mpPixmapFPGAActivity, labelFPGAActivity, false);
		
		setUtilization(mpPixmapCPUUtilization, labelCPUPixmapUtiliziation, 0);
		setUtilization(mpPixmapGPUCoreUtilization, labelGPUCorePixmapUtilization, 0);
		setUtilization(mpPixmapGPUMemUtilization, labelGPUMemPixmapUtilization, 0);
		setUtilization(mpPixmapFPGAUtilization, labelFPGAPixmapUtilization,0);
		setUtilization(mpPixmapMICUtilization, labelMICPixmapUtilization,  0);
		
		setTemperature(mpPixmapCPU0Temperature, labelCPU0PixmapTemperature, 0, 100, 0);
		setTemperature(mpPixmapCPU1Temperature, labelCPU1PixmapTemperature, 0, 100, 0);
		setTemperature(mpPixmapGPUTemperature, labelGPUPixmapTemperature, 0, 100, 0);
		setTemperature(mpPixmapFPGAComputeTemperature, labelFPGAComputePixmapTemperature, 0, 100, 0);
		setTemperature(mpPixmapFPGAInterfaceTemperature, labelFPGAInterfacePixmapTemperature, 0, 100, 0);
		setTemperature(mpPixmapMICTemperature, labelMICPixmapTemperature, 0, 100, 0);
		
		tableWidgetGPUProcesses->verticalHeader()->hide();
		tableWidgetGPUProcesses->setRowCount(MAX_VISIBLE_GPU_PROCSSES);
		for(int i = 0; i < MAX_VISIBLE_GPU_PROCSSES; i++) {
			tableWidgetGPUProcesses->setItem(i, 0, new QTableWidgetItem());
			tableWidgetGPUProcesses->setItem(i, 1, new QTableWidgetItem());
		}
		tableWidgetGPUProcesses->resizeColumnsToContents();
		tableWidgetGPUProcesses->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tableWidgetGPUProcesses->horizontalHeader()->setStretchLastSection(true);
		
		tableWidgetFPGAProcesses->verticalHeader()->hide();
		tableWidgetFPGAProcesses->setRowCount(1);
		tableWidgetFPGAProcesses->setItem(0, 0, new QTableWidgetItem());
		tableWidgetFPGAProcesses->setItem(0, 1, new QTableWidgetItem());
		tableWidgetFPGAProcesses->setItem(0, 2, new QTableWidgetItem());
		tableWidgetFPGAProcesses->resizeColumnsToContents();
		tableWidgetFPGAProcesses->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tableWidgetFPGAProcesses->horizontalHeader()->setStretchLastSection(true);
		
		if(!(pConfig->cpu_enabled)) {
			groupBoxCPU->setEnabled(false);
		}
		if(!(pConfig->gpu_enabled)) {
			groupBoxGPU->setEnabled(false);
		}
		if(!(pConfig->fpga_enabled)) {
			groupBoxFPGA->setEnabled(false);
		}
		if(!(pConfig->mic_enabled)) {
			groupBoxMIC->setEnabled(false);
		}
    }

	
	void QMSMFormSystemOverview::refreshGui(void) {
		emit signalRefreshGui();
    }
    
	void QMSMFormSystemOverview::startTimer(void) {
		mTimer.startTimer();
	}
	
	void QMSMFormSystemOverview::stopTimer(void) {
		mTimer.stopTimer();
	}
	
	void QMSMFormSystemOverview::joinTimer(void) {
		mTimer.joinTimer();
	}
	
	void QMSMFormSystemOverview::closeEvent(QCloseEvent *pEvent) {
		hide();
	}
	
	bool QMSMFormSystemOverview::event(QEvent *pEvent) {
		if (pEvent->type() == QEvent::ToolTip) {
			return true;
		}
		
		return QMdiSubWindow::event(pEvent);
	}
	
	void QMSMFormSystemOverview::slotRefreshGui(void) {
		if(isVisible()){
			updateActiveProcesses();
			updateUtilization();
			updateTemperature();
		}
	}
	
    void QMSMFormSystemOverview::createActions(void) {
		connect(this, SIGNAL(signalRefreshGui(void)), this, SLOT(slotRefreshGui()));
    }
    
    QMSMFormSystemOverview* QMSMFormSystemOverview::construct(QWidget* pParent, NData::CDataHandler* pDataHandler) {
		if (spFormSystemOverview == 0) {
			spFormSystemOverview = new QMSMFormSystemOverview(pParent, pDataHandler);
		}
		
		return spFormSystemOverview;
    }
    
    void QMSMFormSystemOverview::updateActiveProcesses(void) {
		if(mpDataHandler->getMeasurement().mMaxelerActiveProcessPid != 0) {
			setActivity(mpPixmapFPGAActivity, labelFPGAActivity, true);
		} else {
			setActivity(mpPixmapFPGAActivity, labelFPGAActivity, false);
		}
		
		addFPGAProcessToTable(mpDataHandler->getMeasurement().mMaxelerActiveProcessPid,
								mpDataHandler->getMeasurement().mMaxelerActiveProcessName,
								mpDataHandler->getMeasurement().mMaxelerActiveProcessUser);
		
		if(mpDataHandler->getMeasurement().mNVMLActiveProcessesCount > 0) {
			setActivity(mpPixmapGPUActivity, labelGPUActivity, true);
		} else {
			setActivity(mpPixmapGPUActivity, labelGPUActivity, false);
		}
		
		addGPUProcessesToTable(mpDataHandler->getMeasurement().mNVMLActiveProcessesCount,
								mpDataHandler->getMeasurement().mNVMLActiveProcessesPid,
								mpDataHandler->getMeasurement().mNVMLActiveProcessesName);
    }
    
    void QMSMFormSystemOverview::updateUtilization(void) {
		uint32_t factorDataToHeatmapSamplingRate = mpDataHandler->getSettings().mHeatmapSamplingRate / 
													mpDataHandler->getSettings().mDataSamplingRate;
		
		double *util_cpu		= mpDataHandler->getMeasurement().mpYUtilCpu->getDataPtr()
									+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate - 1;
		double *util_gpu_core	= mpDataHandler->getMeasurement().mpYUtilGpuCore->getDataPtr()
									+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate - 1;
		double *util_gpu_mem	= mpDataHandler->getMeasurement().mpYUtilGpuMem->getDataPtr()
									+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate - 1;
		double *util_fpga		= mpDataHandler->getMeasurement().mpYUtilFpga->getDataPtr()
									+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate - 1;
		double *util_mic		= mpDataHandler->getMeasurement().mpYUtilMic->getDataPtr()
									+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate - 1;
		
		double utilCPU		= calcMean(util_cpu, factorDataToHeatmapSamplingRate);
		double utilGPUCore	= calcMean(util_gpu_core, factorDataToHeatmapSamplingRate);
		double utilGPUMem	= calcMean(util_gpu_mem, factorDataToHeatmapSamplingRate);
		double utilFPGA		= calcMean(util_fpga, factorDataToHeatmapSamplingRate);
		double utilMIC		= calcMean(util_mic, factorDataToHeatmapSamplingRate);
		
		setUtilization(mpPixmapCPUUtilization, labelCPUPixmapUtiliziation, utilCPU);
		setUtilization(mpPixmapGPUCoreUtilization, labelGPUCorePixmapUtilization, utilGPUCore);
		setUtilization(mpPixmapGPUMemUtilization, labelGPUMemPixmapUtilization, utilGPUMem);
		setUtilization(mpPixmapFPGAUtilization, labelFPGAPixmapUtilization, utilFPGA);
		setUtilization(mpPixmapMICUtilization, labelMICPixmapUtilization,  utilMIC);
    }
    
    void QMSMFormSystemOverview::updateTemperature(void) {
		uint32_t factorDataToHeatmapSamplingRate = mpDataHandler->getSettings().mHeatmapSamplingRate / 
													mpDataHandler->getSettings().mDataSamplingRate;
													
		double *temp_cpu_0	= mpDataHandler->getMeasurement().mpYTempCpu0->getDataPtr()
								+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate -1;
		double *temp_cpu_1	= mpDataHandler->getMeasurement().mpYTempCpu1->getDataPtr()
								+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate -1;
		double *temp_gpu	= mpDataHandler->getMeasurement().mpYTempGpu->getDataPtr()
								+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate -1;
		double *temp_fpga_m	= mpDataHandler->getMeasurement().mpYTempFpgaM->getDataPtr()
								+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate -1;
		double *temp_fpga_i	= mpDataHandler->getMeasurement().mpYTempFpgaI->getDataPtr()
								+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate -1;
		double *temp_mic	= mpDataHandler->getMeasurement().mpYTempMicDie->getDataPtr()
								+ mpDataHandler->getSettings().mNumberOfTicks - factorDataToHeatmapSamplingRate -1;
		
		double tempCPU0		= calcMean(temp_cpu_0, factorDataToHeatmapSamplingRate);
		double tempCPU1		= calcMean(temp_cpu_1, factorDataToHeatmapSamplingRate);
		double tempGPU 		= calcMean(temp_gpu, factorDataToHeatmapSamplingRate);
		double tempFPGAm	= calcMean(temp_fpga_m, factorDataToHeatmapSamplingRate);
		double tempFPGAi	= calcMean(temp_fpga_i, factorDataToHeatmapSamplingRate);
		double tempMIC		= calcMean(temp_mic, factorDataToHeatmapSamplingRate);
		
		NData::CDataSettings &rSettings = mpDataHandler->getSettings();
		
		setTemperature(mpPixmapCPU0Temperature, labelCPU0PixmapTemperature,
					   rSettings.mHeatmapTempCPUMinY, rSettings.mHeatmapTempCPUMaxY, round(tempCPU0));
		setTemperature(mpPixmapCPU1Temperature, labelCPU1PixmapTemperature,
					   rSettings.mHeatmapTempCPUMinY, rSettings.mHeatmapTempCPUMaxY, round(tempCPU1));
		setTemperature(mpPixmapGPUTemperature, labelGPUPixmapTemperature,
					   rSettings.mHeatmapTempGPUMinY, rSettings.mHeatmapTempGPUMaxY, round(tempGPU));
		setTemperature(mpPixmapFPGAComputeTemperature, labelFPGAComputePixmapTemperature,
					   rSettings.mHeatmapTempFPGAMinY, rSettings.mHeatmapTempFPGAMaxY, round(tempFPGAm));
		setTemperature(mpPixmapFPGAInterfaceTemperature, labelFPGAInterfacePixmapTemperature,
					   rSettings.mHeatmapTempFPGAMinY, rSettings.mHeatmapTempFPGAMaxY, round(tempFPGAi));
		setTemperature(mpPixmapMICTemperature, labelMICPixmapTemperature,
					   rSettings.mHeatmapTempMICMinY, rSettings.mHeatmapTempMICMaxY, round(tempMIC));
    }


	void QMSMFormSystemOverview::setActivity(QPixmap* pPixmap, QLabel* pLabel, bool active) {
		pPixmap->fill(this, 0, 0);
		mpPainter->begin(pPixmap);
		mpPainter->setRenderHint(QPainter::Antialiasing);
		mpPainter->setPen(Qt::black);
		QRadialGradient radialGradient(16, 16, 16);
		radialGradient.setColorAt(0.0, Qt::white);
		radialGradient.setColorAt(1.0, Qt::black);
		if(active) {
			radialGradient.setColorAt(0.7, Qt::red);
		} else {
			radialGradient.setColorAt(0.7, Qt::green);
		}
		mpPainter->setBrush(radialGradient);
		mpPainter->drawEllipse(1, 1, 30, 30);
		mpPainter->end();
		pLabel->setPixmap(*pPixmap);
	}

	void QMSMFormSystemOverview::setUtilization(QPixmap* pPixmap, QLabel* pLabel, double value) {
		pPixmap->fill(this, 0, 0);
		mpPainter->begin(pPixmap);
		mpPainter->setRenderHint(QPainter::Antialiasing);
		mpPainter->setPen(Qt::black);
		QRect rect = QRect(1, 1, 58, 33);
		mpPainter->drawRect(rect);
		mpPainter->fillRect(rect, mpColorMap->color(QwtDoubleInterval(0,100), value));
		std::ostringstream sstream;
		sstream.precision(2);
		sstream << std::fixed << value << " %";
		mpPainter->drawText(1 ,1, 58, 33, Qt::AlignCenter, sstream.str().c_str());
		mpPainter->end();
		pLabel->setPixmap(*pPixmap);
	}

	void QMSMFormSystemOverview::setTemperature(QPixmap* pPixmap, QLabel* pLabel, double minVal, double maxVal, double value) {
		pPixmap->fill(this, 0, 0);
		mpPainter->begin(pPixmap);
		mpPainter->setRenderHint(QPainter::Antialiasing);
		mpPainter->setPen(Qt::black);
		QRect rect = QRect(1, 1, 58, 33);
		mpPainter->drawRect(rect);
		mpPainter->fillRect(rect, mpColorMap->color(QwtDoubleInterval(minVal, maxVal), value));
		std::ostringstream sstream;
		sstream << value << " \u00b0C";
		mpPainter->drawText(1 ,1, 58, 33, Qt::AlignCenter, QString::fromUtf8(sstream.str().c_str()));
		mpPainter->end();
		pLabel->setPixmap(*pPixmap);
	}

	void QMSMFormSystemOverview::addFPGAProcessToTable(uint32_t pid, const std::string& name, const std::string& user) {
		if(pid != 0) {
			tableWidgetFPGAProcesses->item(0, 0)->setText(QString("%1").arg(pid));
			tableWidgetFPGAProcesses->item(0, 1)->setText(QString::fromStdString(name));
			tableWidgetFPGAProcesses->item(0, 2)->setText(QString::fromStdString(user));
			tableWidgetFPGAProcesses->item(0,0)->setTextColor(Qt::black);
			tableWidgetFPGAProcesses->item(0,1)->setTextColor(Qt::black);
			tableWidgetFPGAProcesses->item(0,2)->setTextColor(Qt::black);
		} else {
			tableWidgetFPGAProcesses->item(0,0)->setTextColor(Qt::gray);
			tableWidgetFPGAProcesses->item(0,1)->setTextColor(Qt::gray);
			tableWidgetFPGAProcesses->item(0,2)->setTextColor(Qt::gray);
		}
		tableWidgetFPGAProcesses->resizeColumnsToContents();
		tableWidgetFPGAProcesses->horizontalHeader()->setStretchLastSection(true);
	}
	
    void QMSMFormSystemOverview::addGPUProcessesToTable(uint32_t processCount, uint32_t* pids, std::string* names) {
		bool newProcess = true;
		if(processCount > MAX_VISIBLE_GPU_PROCSSES) {
			mGPUProcessList.clear();
			for(int i = 0; i < MAX_VISIBLE_GPU_PROCSSES; i++) {
				mGPUProcessList.push_back(std::pair<uint32_t, std::string>(pids[i], names[i]));
			}
		} else {
			for(uint32_t i = 0; i < processCount; i++) {
				newProcess = true;
				for (std::deque<std::pair<uint32_t, std::string> >::iterator it = mGPUProcessList.begin(); it != mGPUProcessList.end(); ++it){
					if(it->first == pids[i]) {
						newProcess = false;
					}
				}
				if(newProcess) {
					mGPUProcessList.push_front(std::pair<uint32_t, std::string>(pids[i], names[i]));
				}
			}
			while(mGPUProcessList.size() > MAX_VISIBLE_GPU_PROCSSES) {
				mGPUProcessList.pop_back();
			}
		}
		
		for(uint32_t i = 0; i < MAX_VISIBLE_GPU_PROCSSES; i++) {
			if(mGPUProcessList.size() > i) {
				tableWidgetGPUProcesses->item(i, 0)->setText(QString("%1").arg(mGPUProcessList[i].first));
				tableWidgetGPUProcesses->item(i, 1)->setText(QString::fromStdString(mGPUProcessList[i].second));
				if(processCount > i){
					tableWidgetGPUProcesses->item(i,0)->setTextColor(Qt::black);
					tableWidgetGPUProcesses->item(i,1)->setTextColor(Qt::black);
				} else {
					tableWidgetGPUProcesses->item(i,0)->setTextColor(Qt::gray);
					tableWidgetGPUProcesses->item(i,1)->setTextColor(Qt::gray);
				}
			} else {
				tableWidgetGPUProcesses->item(i, 0)->setText("");
				tableWidgetGPUProcesses->item(i, 1)->setText("");	
			}
		}
		
		tableWidgetGPUProcesses->resizeColumnsToContents();
		tableWidgetGPUProcesses->horizontalHeader()->setStretchLastSection(true);
    }
    
	double QMSMFormSystemOverview::calcMean(double* data, uint32_t size) {
		double sum = 0;
		for (uint32_t i = 0; i < size; i++) {
			sum += data[i];
		}
		return sum / size;
    }
}