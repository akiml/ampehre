#include "CProtocolC.hpp"

CProtocolC::CProtocolC(std::string version) {
	mVersion = version;
}

CProtocolC::~CProtocolC() {

}

int CProtocolC::parseMsg(char* msg, unsigned int length, bool rflag, int* reg, int* tsk, std::vector< double >& values){
	//ähnlich zu server
	unsigned int i = 0;
	int k = 0;
	char txt[length];
	
	// parsing Version - start
	while(msg[i] != 10 && i < length){	//while not newline 
		txt[i] = msg[i];
		i++;
	}
	if(checkVersion(txt, i-1, mVersion) < 0){
		std::cout << "wrong version! Abort" << std::endl;
		return -1;
	}
	// parsing Version - end
	
	// parsing Command - start
	k = i;
	while(msg[i] != 10 && i < length){	//while not newline 
		txt[i-k] = msg[i];
		i++;
	}
	*tsk = checkCmd(txt, i-k);
	if(*tsk < 0){
		std::cout << "unknown command! Abort" << std::endl;
		return -1;
	}
	// parsing Command - end
	
	// parsing last line -> check if registry
	k = i;
	while(msg[i] != 10 && i < length){	//while not newline 
		txt[i-k] = msg[i];
		i++;
	}
	if(setReg(txt, i-k, reg) < 0){
		if(getData(txt, i-k, values) < 0){
			return -1;
		}
		else
			rflag = false;
	}
	else
		rflag = true;
	
	return 0;
	
}

std::string CProtocolC::regMsg(uint64_t code) {
	std::string msg = "";
	msg.append(this->mVersion);
	msg.append("\r\n");
	msg.append("CLIENT_REG\r\n");
	
	//send the uint64_t code as 8 char values
	uint64_t v = code;
	int i = 0;
	char parts[8] = {0};
	do {
		parts[i]=v&0xFF; 
		msg.push_back(parts[i]);
		i++;
	} while (v>>=8);
	
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::requestMsg(char reg) {
	std::string msg = "";
	msg.append(this->mVersion);
	msg.append("\r\n");
	msg.append("DATA_REQ\r\n");
	msg.append("REG: ");
	msg.push_back(reg);
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::termMsg(char reg) {
	std::string msg = "";
	msg.append(this->mVersion);
	msg.append("\r\n");
	msg.append("TERM_COM\r\n");
	msg.append("REG: ");
	msg.push_back(reg);
	msg.append("\r\n");
	
	return msg;
}

int CProtocolC::getData(char* msg, int length, std::vector< double >& values) {
	return 0;
}
