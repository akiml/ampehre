#include <stdio.h>
#include <string>

class CProtocolS{
	
public:
	enum CMD { DATA_REQ, DATA_RES, TERM_COM };
	
	CProtocolS(std::string version);
	~CProtocolS();
	int parseMsg(char *msg, const unsigned int length, int* tsk, int* reg);
	int checkVersion(char* vrs, unsigned int length);
	int checkCmd(char* cmd, unsigned int length);
	
private:
	std::string mVersion;
	int mCmd;
};

/**
 Msg from Client:
 
 Version
 Task_Code
 n bytes
 list of needed values
 
 */
