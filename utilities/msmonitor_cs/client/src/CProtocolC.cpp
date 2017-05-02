/*
 * CProtocolC.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

#include "CProtocolC.hpp"

CProtocolC::CProtocolC(std::string version) :
	CProtocol(version)
	{
	
	// nothing todo
}

CProtocolC::~CProtocolC() {
	// nothing todo
}

int CProtocolC::parseMsg(void* msg, unsigned int length, int& reg, int& tsk, std::vector< double >& values, std::vector<std::string>& values_pid){
	unsigned int i = 0;
	std::string msg_str ((char*)msg, length);
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
                tsk = checkCmdVersion(submsg);
				if(tsk < 0){
					std::cout << "wrong version" << std::endl;
					return -1;
				}
                i++;
				break;
			  case 1:
                if(tsk == CLIENT_REG){
                    if(setReg(submsg, reg) < 0){
                        return -1;
                    }
                    return 0;
                }
                else if(tsk == DATA_RES){
                    if(getData(msg+before, length-before, values, values_pid) < 0){
                        return -1;
                    }
                    return 0;
                }
				break;
			}
		}
		next += 2;

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

std::string CProtocolC::freqMsg(int reg)
{
    std::string msg = "";
    addCmdVersion(msg, SET_FREQ, mVersion);
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

int CProtocolC::getData(void* msg, int size, std::vector< double >& values, std::vector<std::string> &v) {

	values.clear();
    v.clear();
	void* k = msg;
	double val = 0;
    int i = 0;

 
    while(k < size+msg && i < 29)
    {
		memcpy(&val, k, sizeof(double));
		std::cout << val << std::endl;
		values.push_back(val);
		k+= sizeof(double)+2*sizeof(char);
        i++;
	}
    char* cptr = (char*)k;
    std::string tmp = "";
    while(cptr < size+(char*)msg)
    {
        while(cptr < size+(char*)msg && *cptr != '\r')
        {
            tmp += *cptr;
            cptr++;
        }
        v.push_back(tmp);
        std::cout << tmp <<std::endl;
        tmp = "";
        cptr+=2;
    }


	
	return 0;
}

int CProtocolC::getData(void *msg, int size, std::vector<uint64_t> &values)
{
    std::string str ((char*)msg, size);
    std::size_t found = str.find('\n');
    std::size_t before = 0;
    std::string submsg= str.substr(found+1, str.size());
    before = found+1;
    uint64_t freqVal;

    values.clear();
    while(found != std::string::npos)
    {
        found = str.find('\n', before+1);
        submsg = str.substr(before, found-2);
        std::istringstream(submsg) >> freqVal;
        values.push_back(freqVal);
        before = found+1;
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
	values.clear();
	values.push_back(X);
	values.push_back(YClockCpu0);
	values.push_back(YClockCpu1);
	values.push_back(YClockGpuCore);
	values.push_back(YClockGpuMem);
	values.push_back(YClockMicCore);
	values.push_back(YClockMicMem);
}

void CProtocolC::addMemory(std::vector< int >& values) {
	values.clear();
	values.push_back(X);
	values.push_back(YMemoryCpu);
	values.push_back(YSwapCpu);
	values.push_back(YMemoryGpu);
	values.push_back(YMemoryMic);
}

void CProtocolC::addPower(std::vector< int >& values) {
	values.clear();
	values.push_back(X);
	values.push_back(YPowerCpu0);
	values.push_back(YPowerCpu1);
	values.push_back(YPowerGpu);
	values.push_back(YPowerFpga);
	values.push_back(YPowerMic);
	values.push_back(YPowerSystem);
}

void CProtocolC::addTemp(std::vector< int >& values) {
	values.clear();
	values.push_back(X);
	values.push_back(YTempCpu0);
	values.push_back(YTempCpu1);
	values.push_back(YTempGpu);
	values.push_back(YTempFpgaM);
	values.push_back(YTempFpgaI);
	values.push_back(YTempMicDie);
	values.push_back(YTempSystem);
}

void CProtocolC::addUtil(std::vector< int >& values) {
	values.clear();
	values.push_back(X);
	values.push_back(YUtilCpu);
	values.push_back(YUtilGpuCore);
	values.push_back(YUtilGpuMem);
	values.push_back(YUtilFpga);
	values.push_back(YUtilMic);
}
