#include "CProtocolC.hpp"

CProtocolC::CProtocolC(std::string version) {
	mVersion = version;
}

CProtocolC::~CProtocolC() {

}

int CProtocolC::parseMsg(char* msg, unsigned int length, bool rflag, int* reg, int* tsk, std::vector< double >& values){
	unsigned int i = 0;
	int k = 0;
	std::string msg_str (msg, length);
	std::string end ("\r\n");
	std::string submsg;
	
	std::size_t found = 1;
	std::size_t next = 0;
	std::size_t before = 0;
	
	while(found != std::string::npos){
		found = msg_str.find(end, next);
		before = next;
		next = found;
		if(found == std::string::npos){
			break;
		}
		else{
			submsg = msg_str.substr(before, next-before);
			switch(i){
			  case 0:
				if(checkVersion(submsg, mVersion) < 0){
					std::cout << "wrong Version - abort..." << std::endl;
					return -1;
				}
				break;
				
			  case 1:
				k = checkCmd(submsg);
				if(k < 0){
					std::cout << "unknown command! abort..." << std::endl;
					return -1;
				} else {
					*tsk = k;
				}
				break;
			  case 2:
				if(setReg(submsg, reg) < 0){
					if(getData(submsg, values) < 0){
						std::cout << "aquiring data failed" << std::endl; 
						return -1;
					} else{
						std::cout << "requested data saved..." << std::endl;
					}
				}
				else{
					rflag = true;
					std::cout << "acquired registry" << std::endl;
				}
				break;
			}
		}
		next += end.size();
		i++;
	}
	
	return 0;
	
}

std::string CProtocolC::regMsg(uint64_t code) {
	std::string msg = "";
	addVersion(msg, mVersion);
	addCmd(msg, CLIENT_REG);
	
	char c;
	for(unsigned int i = 0; i < 8; i++){
		c = 0xFF & (code >> i*8);
		msg.push_back(c+33);			//33 offset needed to make it work, is subtracted on server
	}
	
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::requestMsg(int reg) {
	std::string msg = "";
	addVersion(msg, mVersion);
	addCmd(msg, DATA_REQ);
	msg.append("REG: ");
	std::ostringstream ss;
	ss << reg;
	msg.append(ss.str());
	msg.append("\r\n");
	
	return msg;
}

std::string CProtocolC::termMsg(int reg) {
	std::string msg = "";
	addVersion(msg, mVersion);
	addCmd(msg, TERM_COM);
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
