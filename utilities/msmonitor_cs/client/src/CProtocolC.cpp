#include "CProtocolC.hpp"

CProtocolC::CProtocolC(std::string version) {
	mVersion = version;
}

CProtocolC::~CProtocolC() {

}

int CProtocolC::parseMsg(char* msg, unsigned int length, bool rflag, int* reg, int* tsk, std::vector< double >& values){
	//ähnlich zu server
    
	
	return 0;
	
}

std::string CProtocolC::regMsg(uint64_t code) {
	std::string msg = "";
	addVersion(msg, mVersion);
	addCmd(msg, CLIENT_REG);
	
	char c;
	for(unsigned int i = 0; i < 8; i++){
		c = 0xFF & (code >> i*8);
		std::cout << i << ":\t" << (int)c+33 << std::endl;
		msg.push_back(c+33);			//33 offset needed to make it work, is subtracted on server
	}
	
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::requestMsg(char reg) {
	std::string msg = "";
	addVersion(msg, mVersion);
	addCmd(msg, DATA_REQ);
	msg.append("REG: ");
	msg.push_back(reg);
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::termMsg(char reg) {
	std::string msg = "";
	addVersion(msg, mVersion);
	addCmd(msg, TERM_COM);
	msg.append("REG: ");
	msg.push_back(reg);
	msg.append("\r\n");
	
	return msg;
}

int CProtocolC::getData(char* msg, int length, std::vector< double >& values) {
	return 0;
}
