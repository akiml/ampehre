#include <CProtocolS.hpp>

CProtocolS::CProtocolS(std::string version){
	this->mVersion = version;
}

CProtocolS::~CProtocolS(){}

int CProtocolS::parseMsg(char* msg, const unsigned int length, int* tsk, int* reg){
	unsigned int i = 0;
	int k = 0;
	char txt[length];
	
	// parsing Version - start
	while(msg[i] != 10 || i >= length){	//while not newline 
		txt[i] = msg[i];
		i++;
	}
	if(checkVersion(txt, i-1) < 0){
		printf("wrong version! Abort");
		return -1;
	}
	// parsing Version - end
	
	//parsing Command - start
	k = i;
	while(msg[i] != 10 || i >= length){	//while not newline 
		txt[i-k] = msg[i];
		i++;
	}
	if(checkCmd(txt, i-k) < 0){
		printf("unknown command! Abort");
		return -1;
	}
	//parsing Command - end
	
	k = i;
	
	return 0;

}

int CProtocolS::checkVersion(char* vrs, unsigned int length){
	std::string cmp (vrs, length);
	if (cmp == this->mVersion)
		return 0;
	else 
		return -1;
}

int CProtocolS::checkCmd(char* cmd, unsigned int length) {
	std::string cmp (cmd, length);
	
	if(cmp == "DATA_REQ"){
		this->mCmd = DATA_REQ;
		return DATA_REQ;
	}
	else if(cmp == "DATA_RES"){
		this->mCmd = DATA_RES;
		return DATA_RES;
	}
	else if(cmp == "TERM_COM"){
		this->mCmd = TERM_COM;
		return TERM_COM;
	}
	else
		return -1;
	
}
