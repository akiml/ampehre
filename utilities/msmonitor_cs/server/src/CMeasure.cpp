/*
 * CMeasure.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 10/5/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

#include "CMeasure.hpp"


CMeasure::CMeasure():
	mpHandler(new NData::CDataHandler())
{
	//nothing to do
}

CMeasure::~CMeasure()
{
	//nothing to do
}


void CMeasure::start() {
	mpHandler->startCollectData();
}


void CMeasure::stop() {
	mpHandler->stopCollectData();
}

NData::CDataSettings& CMeasure::getSettings(){
	return mpHandler->getSettings();
}

void CMeasure::getProcesses(std::vector< std::string >& processes) 
{
	processes.clear();
	NData::CDataMeasurement &mData = mpHandler->getMeasurement();
	
	std::stringstream ss;
	std::string s;
	ss << mData.mMaxelerActiveProcessPid;
	s = ss.str();
	
	std::string maxeler = "f ";
	maxeler += s;
	maxeler += " ";
	maxeler += mData.mMaxelerActiveProcessName;
	maxeler += " ";
	maxeler += mData.mMaxelerActiveProcessUser;
	maxeler += "\r\n";
	if(mData.mMaxelerActiveProcessName != "")
		processes.push_back(maxeler);
	
	for(unsigned int i = 0; i < mData.mNVMLActiveProcessesCount; i++)
	{
		std::string a = "g ";
		std::stringstream ss;
		std::string s;
		ss << mData.mNVMLActiveProcessesPid[i];
		s = ss.str();
		a += s;
		a += " ";
		a += mData.mNVMLActiveProcessesName[i];
		a += "\r\n";
		processes.push_back(a);
	}
}

void CMeasure::getCurrentTime(double& time) 
{
	time = mpHandler->getMeasurement().mpX->getLast();
}


void CMeasure::getValues(std::vector<double>& sol, std::vector<int>& req){
	sol.clear();
	
	NData::CDataMeasurement &mData = mpHandler->getMeasurement();
	for(unsigned int i = 0; i < req.size(); i++) {
		switch(req[i]){
			case X:{
				sol.push_back(mData.mpX->getLast());
				break;
			}
			case YPowerCpu0:{
				sol.push_back(mData.mpYPowerCpu0->getLast());
				break;
			}
			case YPowerCpu1:{
				sol.push_back(mData.mpYPowerCpu1->getLast());
				break;
			}
			case YPowerGpu:{
				sol.push_back(mData.mpYPowerGpu->getLast());
				break;
			}
			case YPowerFpga:{
				sol.push_back(mData.mpYPowerFpga->getLast());
				break;
			}
			case YPowerMic:{
				sol.push_back(mData.mpYPowerMic->getLast());
				break;
			}
			case YPowerSystem:{
				sol.push_back(mData.mpYPowerSystem->getLast());
				break;
			}
			case YTempCpu0:{
				sol.push_back(mData.mpYTempCpu0->getLast());
				break;
			}
			case YTempCpu1:{
				sol.push_back(mData.mpYTempCpu1->getLast());
				break;
			}
			case YTempGpu:{
				sol.push_back(mData.mpYTempGpu->getLast());
				break;
			}
			case YTempFpgaM:{
				sol.push_back(mData.mpYTempFpgaM->getLast());
				break;
			}
			case YTempFpgaI:{
				sol.push_back(mData.mpYTempFpgaI->getLast());
				break;
			}
			case YTempMicDie:{
				sol.push_back(mData.mpYTempMicDie->getLast());
				break;
			}
			case YTempSystem:{
				sol.push_back(mData.mpYTempSystem->getLast());
				break;
			}
			case YClockCpu0:{
				sol.push_back(mData.mpYClockCpu0->getLast());
				break;
			}
			case YClockCpu1:{
				sol.push_back(mData.mpYClockCpu1->getLast());
				break;
			}
			case YClockGpuCore:{
				sol.push_back(mData.mpYClockGpuCore->getLast());
				break;
			}
			case YClockGpuMem:{
				sol.push_back(mData.mpYClockGpuMem->getLast());
				break;
			}
			case YClockMicCore:{
				sol.push_back(mData.mpYClockMicCore->getLast());
				break;
			}
			case YClockMicMem:{
				sol.push_back(mData.mpYClockMicMem->getLast());
				break;
			}
			case YUtilCpu:{
				sol.push_back(mData.mpYUtilCpu->getLast());
				break;
			}
			case YUtilGpuCore:{
				sol.push_back(mData.mpYUtilGpuCore->getLast());
				break;
			}
			case YUtilGpuMem:{
				sol.push_back(mData.mpYUtilGpuMem->getLast());
				break;
			}
			case YUtilFpga:{
				sol.push_back(mData.mpYUtilFpga->getLast());
				break;
			}
			case YUtilMic:{
				sol.push_back(mData.mpYUtilMic->getLast());
				break;
			}
			case YMemoryCpu:{
				sol.push_back(mData.mpYMemoryCpu->getLast());
				break;
			}
			case YSwapCpu:{
				sol.push_back(mData.mpYSwapCpu->getLast());
				break;
			}
			case YMemoryGpu:{
				sol.push_back(mData.mpYMemoryGpu->getLast());
				break;
			}
			case YMemoryMic:{
				sol.push_back(mData.mpYMemoryMic->getLast());
				break;
			}
			
		}
	}
}
