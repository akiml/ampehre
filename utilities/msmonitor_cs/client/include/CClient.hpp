/*
 * CClient.hpp
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
 * created: 11/2/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */


#ifndef CCLIENT_HPP
#define CCLIENT_HPP

#include <vector>
#include "CProtocolC.hpp"
#include "CComTCPClient.hpp"

class CClient{
public:
	CClient();
	~CClient();

	int registerToServer(std::vector<int>& values, int port, std::string ipaddr);
    int requestData();
    int terminate();
    void getFreq(std::vector<uint64_t> &vals);

    std::vector<double> mValues;
    std::vector<std::string> mValues_pid;
    std::vector<Application> mSigPid;
	
private:
    void initSocket();
    void extractAppSignals();
	std::string mVERSION;
	CProtocolC mProtocol;
	int mReg;
	int mPort;
	int mAmtValues;
	std::string mIPaddr;
    CComTCPClient* mCom;
    CComTCPData* mClientData;
	
};



#endif
