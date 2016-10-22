#include "CProtocolS.hpp"

CProtocolS::CProtocolS(std::string version){
	mVersion = version;
	
}

CProtocolS::~CProtocolS(){}

int CProtocolS::parseMsg(char* msg, const unsigned int length, int* tsk, int* reg, uint64_t* data){
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
		if(checkData(txt, i-k, data) < 0){
			return -1;
		}
	}
	
	return 0;

}


int CProtocolS::checkData(char* txt, unsigned int length, uint64_t* data) {
	*data = 0;
	if(length == 8){ //8 bytes should be transmitted
		for (unsigned int i = 0 ; i < length ; i++ )
			*data = (*data << 8) | txt[i];		
	}
	else {
		data = NULL;
		return -1;
	}
	return 0;
}




