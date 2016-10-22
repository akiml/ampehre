#include "CProtocol.hpp"

CProtocol::CProtocol(std::string version) {
	mVersion = version;
}

CProtocol::CProtocol() {

}


CProtocol::~CProtocol() {

}


uint64_t CProtocol::createDataCode(std::vector<int>& values){
	uint64_t sol = 0;
	
	for(unsigned int i = 0; i < values.size(); i++){
			sol = sol | (1 << values[i]);
	}
	
	return sol;
}


int CProtocol::checkVersion(char* vrs,unsigned int length, std::string version){
	std::string cmp (vrs, length);
	if (cmp == version)
		return 0;
	else 
		return -1;
}

int CProtocol::checkCmd(char* cmd, unsigned int length) {
	std::string cmp (cmd, length);
	
	if(cmp == "DATA_REQ"){
		return DATA_REQ;
	}
	else if(cmp == "CLIENT_REG"){
		return CLIENT_REG;
	}
	else if(cmp == "DATA_RES"){
		return DATA_RES;
	}
	else if(cmp == "TERM_COM"){
		return TERM_COM;
	}
	else
		return -1;
	
}


int CProtocol::setReg(char* txt, unsigned int length, int* registry) {
	if(length != 5){
		std::cout << "reg_msg length: " << length << std::endl;
		registry = NULL;
		return -1;
	}
	std::string cmd (txt, 4);
	if(cmd == "REG:") {
		*registry = (int)txt[4];
		return 0;
	}
	else {
		registry = NULL;
		return -1;
	}
}
