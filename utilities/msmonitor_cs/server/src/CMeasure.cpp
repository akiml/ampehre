#include "CMeasure.hpp"


CMeasure::CMeasure():
	mHandler(NData::CDataHandler())
{
	//nothing to do
}

CMeasure::~CMeasure()
{
	//nothing to do
}


void CMeasure::start() {
	mHandler.startCollectData();
}


void CMeasure::stop() {
	mHandler.stopCollectData();
}

void CMeasure::getValues(std::vector<double>& sol, std::vector<int>& req){
	sol.clear();
	NData::CDataMeasurement mData = mHandler.getMeasurement();
	for(unsigned int i = 0; i < req.size(); i++) {
		switch(req[i]){
			case 0:{
				sol.push_back(mData.mpX->getLast());
				break;
			}
			case 1:{
				sol.push_back(mData.mpYPowerCpu0->getLast());
				break;
			}
			case 2:{
				sol.push_back(mData.mpYPowerCpu1->getLast());
				break;
			}
			case 3:{
				sol.push_back(mData.mpYPowerGpu->getLast());
				break;
			}
			case 4:{
				sol.push_back(mData.mpYPowerFpga->getLast());
				break;
			}
			case 5:{
				sol.push_back(mData.mpYPowerMic->getLast());
				break;
			}
			case 6:{
				sol.push_back(mData.mpYPowerSystem->getLast());
				break;
			}
			case 7:{
				sol.push_back(mData.mpYTempCpu0->getLast());
				break;
			}
			case 8:{
				sol.push_back(mData.mpYTempCpu1->getLast());
				break;
			}
			case 9:{
				sol.push_back(mData.mpYTempGpu->getLast());
				break;
			}
			case 10:{
				sol.push_back(mData.mpYTempFpgaM->getLast());
				break;
			}
			case 11:{
				sol.push_back(mData.mpYTempFpgaI->getLast());
				break;
			}
			case 12:{
				sol.push_back(mData.mpYTempMicDie->getLast());
				break;
			}
			case 13:{
				sol.push_back(mData.mpYTempSystem->getLast());
				break;
			}
			case 14:{
				sol.push_back(mData.mpYClockCpu0->getLast());
				break;
			}
			case 15:{
				sol.push_back(mData.mpYClockCpu1->getLast());
				break;
			}
			case 16:{
				sol.push_back(mData.mpYClockGpuCore->getLast());
				break;
			}
			case 17:{
				sol.push_back(mData.mpYClockGpuMem->getLast());
				break;
			}
			case 18:{
				sol.push_back(mData.mpYClockMicCore->getLast());
				break;
			}
			case 19:{
				sol.push_back(mData.mpYClockMicMem->getLast());
				break;
			}
			case 20:{
				sol.push_back(mData.mpYUtilCpu->getLast());
				break;
			}
			case 21:{
				sol.push_back(mData.mpYUtilGpuCore->getLast());
				break;
			}
			case 22:{
				sol.push_back(mData.mpYUtilGpuMem->getLast());
				break;
			}
			case 23:{
				sol.push_back(mData.mpYUtilFpga->getLast());
				break;
			}
			case 24:{
				sol.push_back(mData.mpYUtilMic->getLast());
				break;
			}
			case 25:{
				sol.push_back(mData.mpYMemoryCpu->getLast());
				break;
			}
			case 26:{
				sol.push_back(mData.mpYSwapCpu->getLast());
				break;
			}
			case 27:{
				sol.push_back(mData.mpYMemoryGpu->getLast());
				break;
			}
			case 28:{
				sol.push_back(mData.mpYMemoryMic->getLast());
				break;
			}
			
		}
	}
}