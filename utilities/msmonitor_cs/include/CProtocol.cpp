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
		sol |= (0x1 << values[i]);
	}
	
	return sol;
}


int CProtocol::checkVersion(std::string msg, std::string version){
	if (msg == version)
		return 0;
	else 
		return -1;
}

int CProtocol::checkCmd(std::string msg) {
	if(msg == "DATA_REQ"){
		return DATA_REQ;
	}
	else if(msg == "CLIENT_REG"){
		return CLIENT_REG;
	}
	else if(msg == "DATA_RES"){
		return DATA_RES;
	}
	else if(msg == "TERM_COM"){
		return TERM_COM;
	}
	else
		return -1;
	
}


int CProtocol::setReg(std::string msg, int* registry) {
	std::size_t found = msg.find("REG:");
	if(found != std::string::npos && found == 0){
		*registry = (int)msg.at(4);
		return 0;
	}
	else {
		registry = NULL;
		return -1;
	}
}

void CProtocol::addVersion(std::string& msg, std::string version) {
	msg = "";
	msg.append(version);
	msg.append("\r\n");
}

void CProtocol::addCmd(std::string& msg, int cmd) {
	switch(cmd){
		case CLIENT_REG:{
			msg.append("CLIENT_REG");
			break;
		}
		case DATA_REQ:{
			msg.append("DATA_REQ");
			break;
		}
		case DATA_RES:{
			msg.append("DATA_RES");
			break;
		}
		case TERM_COM:{
			msg.append("TERM_COM");
			break;
		}
	}
	msg.append("\r\n");
}

void CProtocol::addData(std::string& msg, int type, double value) {
	std::ostringstream ss;
	
	ss << type;
	msg.append(ss.str());
	ss.str("");								//clear the stream
	
	msg.push_back(':');
	
	ss << value;
	msg.append(ss.str());
	
	msg.append("\r\n");
}


