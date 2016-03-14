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
 * version: 0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
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
		
		this->setFixedHeight(670);
		this->setMinimumWidth(550);
		
		setupUi(mpFormSystemOverview);
		
		setWidget(mpFormSystemOverview);
		
		initGUI();
		
		createActions();
	}

	QMSMFormSystemOverview::~QMSMFormSystemOverview() {
		delete mpGroupboxCPU;
		delete mpGroupboxGPU;
		delete mpGroupboxFPGA;
		delete mpGroupboxMIC;
		
		delete mpFormSystemOverview;
	}
	
    void QMSMFormSystemOverview::initGUI(void) {
		MS_CONFIG* pConfig = mpDataHandler->getLibrary().getMS_CONFIG();
		
		std::string columnNamesGPU[] = {"PID", "Name"};
		mpGroupboxCPU = new QMSMGroupboxSystemOverview(mpFormSystemOverview, "CPU", "", "", "CPU 0", "CPU 1", false);
		mpGroupboxGPU = new QMSMGroupboxSystemOverview(mpFormSystemOverview, "GPU", "Core", "Memory", "", "", true, columnNamesGPU, 2, MAX_VISIBLE_GPU_PROCSSES);
		mpGroupboxFPGA = new QMSMGroupboxSystemOverview(mpFormSystemOverview, "FPGA", "", "", "Compute", "Interface", true);
		mpGroupboxMIC = new QMSMGroupboxSystemOverview(mpFormSystemOverview, "MIC", "", "", "", "", false);
		
		verticalLayout->addWidget(mpGroupboxCPU);
		verticalLayout->addWidget(mpGroupboxGPU);
		verticalLayout->addWidget(mpGroupboxFPGA);
		verticalLayout->addWidget(mpGroupboxMIC);
		
		//initialize all labels which should be visible
		mpGroupboxGPU->setActivity(false);
		mpGroupboxFPGA->setActivity(false);
		
		mpGroupboxCPU->setUtilization(0, 0);
		mpGroupboxGPU->setUtilization(0, 0);
		mpGroupboxGPU->setUtilization(1, 0);
		mpGroupboxFPGA->setUtilization(0, 0);
		mpGroupboxMIC->setUtilization(0, 0);
		
		mpGroupboxCPU->setTemperature(0, 0, 100, 0);
		mpGroupboxCPU->setTemperature(1, 0, 100, 0);
		mpGroupboxGPU->setTemperature(0, 0, 100, 0);
		mpGroupboxFPGA->setTemperature(0, 0, 100, 0);
		mpGroupboxFPGA->setTemperature(1, 0, 100, 0);
		mpGroupboxMIC->setTemperature(0, 0, 100, 0);
		
		if(!(pConfig->cpu_enabled)) {
			mpGroupboxCPU->setEnabled(false);
		}
		if(!(pConfig->gpu_enabled)) {
			mpGroupboxGPU->setEnabled(false);
		}
		if(!(pConfig->fpga_enabled)) {
			mpGroupboxFPGA->setEnabled(false);
		}
		if(!(pConfig->mic_enabled)) {
			mpGroupboxMIC->setEnabled(false);
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
			mpGroupboxFPGA->setActivity(true);
		} else {
			mpGroupboxFPGA->setActivity(false);
		}
		
		addFPGAProcessToTable(mpDataHandler->getMeasurement().mMaxelerActiveProcessPid,
								mpDataHandler->getMeasurement().mMaxelerActiveProcessName,
								mpDataHandler->getMeasurement().mMaxelerActiveProcessUser);
		
		if(mpDataHandler->getMeasurement().mNVMLActiveProcessesCount > 0) {
			mpGroupboxGPU->setActivity(true);
		} else {
			mpGroupboxGPU->setActivity(false);
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
		
		mpGroupboxCPU->setUtilization(0, utilCPU);
		mpGroupboxGPU->setUtilization(0, utilGPUCore);
		mpGroupboxGPU->setUtilization(1, utilGPUMem);
		mpGroupboxFPGA->setUtilization(0, utilFPGA);
		mpGroupboxMIC->setUtilization(0,  utilMIC);
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
		
		mpGroupboxCPU->setTemperature(0, rSettings.mHeatmapTempCPUMinY, rSettings.mHeatmapTempCPUMaxY, round(tempCPU0));
		mpGroupboxCPU->setTemperature(1, rSettings.mHeatmapTempCPUMinY, rSettings.mHeatmapTempCPUMaxY, round(tempCPU1));
		mpGroupboxGPU->setTemperature(0, rSettings.mHeatmapTempGPUMinY, rSettings.mHeatmapTempGPUMaxY, round(tempGPU));
		mpGroupboxFPGA->setTemperature(0, rSettings.mHeatmapTempFPGAMinY, rSettings.mHeatmapTempFPGAMaxY, round(tempFPGAm));
		mpGroupboxFPGA->setTemperature(1, rSettings.mHeatmapTempFPGAMinY, rSettings.mHeatmapTempFPGAMaxY, round(tempFPGAi));
		mpGroupboxMIC->setTemperature(0, rSettings.mHeatmapTempMICMinY, rSettings.mHeatmapTempMICMaxY, round(tempMIC));
    }

	void QMSMFormSystemOverview::addFPGAProcessToTable(uint32_t pid, const std::string& name, const std::string& user) {
		QTableWidget *processTable = mpGroupboxFPGA->getProcessTable();
		if(pid != 0) {
			processTable->item(0, 0)->setText(QString("%1").arg(pid));
			processTable->item(0, 1)->setText(QString::fromStdString(name));
			processTable->item(0, 2)->setText(QString::fromStdString(user));
			processTable->item(0,0)->setTextColor(Qt::black);
			processTable->item(0,1)->setTextColor(Qt::black);
			processTable->item(0,2)->setTextColor(Qt::black);
		} else {
			processTable->item(0,0)->setTextColor(Qt::gray);
			processTable->item(0,1)->setTextColor(Qt::gray);
			processTable->item(0,2)->setTextColor(Qt::gray);
		}
		processTable->resizeColumnsToContents();
		processTable->horizontalHeader()->setStretchLastSection(true);
	}
	
    void QMSMFormSystemOverview::addGPUProcessesToTable(uint32_t processCount, uint32_t* pids, std::string* names) {
		QTableWidget *processTable = mpGroupboxGPU->getProcessTable();
		bool newProcess = true;
		//update the list of GPU processes
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
			// delete old processes from the queue if necessary
			while(mGPUProcessList.size() > MAX_VISIBLE_GPU_PROCSSES) {
				mGPUProcessList.pop_back();
			}
		}
		
		//add all processes from the list to the processTable
		for(uint32_t i = 0; i < MAX_VISIBLE_GPU_PROCSSES; i++) {
			if(mGPUProcessList.size() > i) {
				processTable->item(i, 0)->setText(QString("%1").arg(mGPUProcessList[i].first));
				processTable->item(i, 1)->setText(QString::fromStdString(mGPUProcessList[i].second));
				if(processCount > i){
					processTable->item(i,0)->setTextColor(Qt::black);
					processTable->item(i,1)->setTextColor(Qt::black);
				} else {
					processTable->item(i,0)->setTextColor(Qt::gray);
					processTable->item(i,1)->setTextColor(Qt::gray);
				}
			} else {
				processTable->item(i, 0)->setText("");
				processTable->item(i, 1)->setText("");	
			}
		}
		
		processTable->resizeColumnsToContents();
		processTable->horizontalHeader()->setStretchLastSection(true);
	}
    
	double QMSMFormSystemOverview::calcMean(double* data, uint32_t size) {
		double sum = 0;
		for (uint32_t i = 0; i < size; i++) {
			sum += data[i];
		}
		return sum / size;
	}
}
