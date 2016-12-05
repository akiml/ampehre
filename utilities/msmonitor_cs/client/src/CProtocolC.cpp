#include "CProtocolC.hpp"

CProtocolC::CProtocolC(std::string version) {
	mVersion = version;
}

CProtocolC::~CProtocolC() {

}

int CProtocolC::parseMsg(char* msg, unsigned int length, int& reg, int& tsk, std::vector< double >& values){
	unsigned int i = 0;
	int k = 0;
	std::string msg_str (msg, strlen(msg));
	std::string submsg;
	
	std::size_t found = 1;
	std::size_t next = 0;
	std::size_t before = 0;
	while(found != std::string::npos){
		found = msg_str.find('\r', next);
		before = next;
		next = found;
		if(found == std::string::npos){
			break;
		}
		else{
			submsg = msg_str.substr(before, next-before);
			switch(i){
			  case 0:
				tsk = checkCmdVersion(submsg, mVersion);
				if(tsk < 0){
					return -1;
				}
				break;
			  case 1:
				if(setReg(submsg, reg) < 0){
					if(getData(submsg, values) < 0){
						return -1;
					} 
				}
				break;
			}
		}
		next += 2;
		i++;
	}
	return 0;
}

std::string CProtocolC::regMsg(uint64_t code) {
	std::string msg = "";
	addCmdVersion(msg, CLIENT_REG, mVersion);
	char c;
	for(unsigned int i = 0; i < 8; i++){
		c = 0xFF & (code >> i*8);
		msg.push_back(c+20);
	}
	
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::requestMsg(int reg) {
	std::string msg = "";
	addCmdVersion(msg, DATA_REQ, mVersion);
	msg.append("REG: ");
	std::ostringstream ss;
	ss << reg;
	msg.append(ss.str());
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::termMsg(int reg) {
	std::string msg = "";
	addCmdVersion(msg, TERM_COM, mVersion);
	msg.append("REG: ");
	std::ostringstream ss;
	ss << reg;
	msg.append(ss.str());
	msg.append("\r\n");
	
	return msg;
}

int CProtocolC::getData(std::string &msg, std::vector< double >& values) {
	values.clear();
	std::string end ("\r\n");
	std::string submsg;
	
	std::size_t found = 1;
	std::size_t next = 0;
	std::size_t before = 0;
	double val;
	
	while(found != std::string::npos){
		found = msg.find(end, next);
		before = next;
		next = found;
		if(found == std::string::npos){
			break;
		}
		else{
			submsg = msg.substr(before, next-before);
			std::istringstream ss(submsg);
			ss >> val;
			values.push_back(val);
		}
	}
	
	return 0;
}

void CProtocolC::addAll(std::vector< int >& values) {
	
	values.clear();
	for(unsigned int i = 0; i < VALUESIZE; i++){
		values.push_back(i);
	}
	
}

void CProtocolC::addClock(std::vector< int >& values) {
	values.push_back(YClockCpu0);
	values.push_back(YClockCpu1);
	values.push_back(YClockGpuCore);
	values.push_back(YClockGpuMem);
	values.push_back(YClockMicCore);
	values.push_back(YClockMicMem);
}

void CProtocolC::addMemory(std::vector< int >& values) {
	values.push_back(YMemoryCpu);
	values.push_back(YSwapCpu);
	values.push_back(YMemoryGpu);
	values.push_back(YMemoryMic);
}

void CProtocolC::addPower(std::vector< int >& values) {
	values.push_back(YPowerCpu0);
	values.push_back(YPowerCpu1);
	values.push_back(YPowerGpu);
	values.push_back(YPowerFpga);
	values.push_back(YPowerMic);
	values.push_back(YPowerSystem);
}

void CProtocolC::addTemp(std::vector< int >& values) {
	values.push_back(YTempCpu0);
	values.push_back(YTempCpu1);
	values.push_back(YTempGpu);
	values.push_back(YTempFpgaM);
	values.push_back(YTempFpgaI);
	values.push_back(YTempMicDie);
	values.push_back(YTempSystem);
}

void CProtocolC::addUtil(std::vector< int >& values) {
	values.push_back(YUtilCpu);
	values.push_back(YUtilGpuCore);
	values.push_back(YUtilGpuMem);
	values.push_back(YUtilFpga);
	values.push_back(YUtilMic);
}
