/*
 * CClient.cpp
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
 * created: 10/25/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

#include "CClient.hpp"

CClient::CClient():
    mVERSION("0.1"),
    mProtocol(CProtocolC(mVERSION)),
    mReg(0)
{
	std::cout << "Client Version: " << mVERSION << std::endl;
}

CClient::~CClient()
{

}

int CClient::registerToServer(std::vector< int >& values, int port, std::string ipaddr)
{
	mIPaddr = ipaddr;
	mPort = port;
	mAmtValues = values.size();

    mCom = new CComTCPClient(port, ipaddr.c_str());

	initSocket();
	
    char* rep = NULL;
    ssize_t rep_len = 0;
	int reg = -1, tsk;
	
	uint64_t code = mProtocol.createDataCode(values);
	std::string msg = mProtocol.regMsg(code);
	std::vector<double> ignore;
    std::vector<std::string> ig;
	
    mClientData->setMsg(msg.c_str());
    mCom->msmSend(mClientData);
    mCom->msmRecv(mClientData);
    rep = mClientData->getMsg(rep_len);

    std::cout << std::string(rep, rep_len) << std::endl;

    if(rep == NULL)
    {
        std::cout << "rep is NULL..., length is" << rep_len << std::endl;
        return -1;
    }
    mProtocol.parseMsg(rep, rep_len, reg, tsk, ignore, ig);

    if(reg >= 0){
		this->mReg = reg;
		return 0;
	}
	else 
		return -1;
}

int CClient::requestData()
{
    char* rep = NULL;
    ssize_t rep_len = 0;
    int reg = -1, tsk;
    std::string msg = mProtocol.requestMsg(mReg);

    mClientData->setMsg(msg.c_str());
    mCom->msmSend(mClientData);
    mCom->msmRecv(mClientData);
    rep = mClientData->getMsg(rep_len);

    if(rep == NULL)
    {
        std::cout << "rep is NULL..., length is" << rep_len << std::endl;
        return -1;
    }
    mProtocol.parseMsg(rep, rep_len, reg, tsk, mValues, mValues_pid);
    extractAppSignals();

    return 0;
	
}

int CClient::terminate()
{
    char* rep = NULL;
    ssize_t rep_len = 0;

	std::string msg = mProtocol.termMsg(mReg);

    mClientData->setMsg(msg.c_str());
    mCom->msmSend(mClientData);
    mCom->msmRecv(mClientData);
    rep = mClientData->getMsg(rep_len);

    mCom->msmShutdown(&mClientData);

    return 0;
}

void CClient::extractAppSignals()
{
    std::string tmp = "";
    Application app;
    mSigPid.clear();
    for(unsigned int i = 0; i < mValues_pid.size(); i++)
    {
        if(mValues_pid[i][0] == 'y')    //application started
        {
            unsigned int k = 2;
            while(k < mValues_pid[i].size() && mValues_pid[i][k] != ' ')
            {
                tmp += mValues_pid[i][k];
                k++;
            }
            //got pid
            app.mPid = atoi(tmp.c_str());
            tmp = "";
            k++;

            while(k < mValues_pid[i].size() && mValues_pid[i][k] != ' ')
            {
                tmp += mValues_pid[i][k];
                k++;
            }
            //got time
            app.mTime = atoi(tmp.c_str());
            tmp = "";
            app.start = true;

            mSigPid.push_back(app);
        }
        else if(mValues_pid[i][0] == 'z')    //application terminated
        {
            unsigned int k = 2;
            while(k < mValues_pid[i].size() && mValues_pid[i][k] != ' ')
            {
                tmp += mValues_pid[i][k];
                k++;
            }
            //got pid
            app.mPid = atoi(tmp.c_str());
            tmp = "";
            k++;

            while(k < mValues_pid[i].size() && mValues_pid[i][k] != ' ')
            {
                tmp += mValues_pid[i][k];
                k++;
            }
            //got time
            app.mTime = atoi(tmp.c_str());
            tmp = "";
            app.start = false;

            mSigPid.push_back(app);
        }
    }
}

void CClient::getFreq(std::vector<uint64_t>& vals)
{
    char* rep = NULL;
    ssize_t rep_len = 0;

    std::string msg = mProtocol.freqMsg(mReg);

    mClientData->setMsg(msg.c_str());
    mCom->msmSend(mClientData);
    mCom->msmRecv(mClientData);
    rep = mClientData->getMsg(rep_len);

    if(rep == NULL)
    {
        std::cout << "rep is NULL..., length is" << rep_len << std::endl;
    }

    mProtocol.getData(rep, rep_len, vals);
}

void CClient::initSocket()
{
    mCom->msmSocket();
    mCom->msmConnect(&mClientData);
}
