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
    mCom(CComClient()),
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
	
	initSocket();
	
    void* rep = NULL;
	int rep_len = 0;
	int reg = -1, tsk;
	
	uint64_t code = mProtocol.createDataCode(values);
	std::string msg = mProtocol.regMsg(code);
	std::vector<double> ignore;
    std::vector<std::string> ig;
	
    mCom.msm_send(msg.c_str());
    mCom.msm_recv(&rep, rep_len);
    std::cout << std::string((char*) rep, rep_len) << std::endl;

    if(rep == NULL)
    {
        std::cout << "rep is NULL..., length is" << rep_len << std::endl;
        return -1;
    }
    mProtocol.parseMsg(rep, rep_len, reg, tsk, ignore, ig);

	
	free(rep);
	if(reg >= 0){
		this->mReg = reg;
		return 0;
	}
	else 
		return -1;
}

int CClient::requestData()
{
    void* rep = NULL;
	int rep_len = 0;
	int reg = -1, tsk;
	std::string msg = mProtocol.requestMsg(mReg);
    mCom.msm_send(msg.c_str());
    mCom.msm_recv(&rep, rep_len);

    if(rep == NULL)
    {
        std::cout << "rep is NULL..., length is" << rep_len << std::endl;
        return -1;
    }
    mProtocol.parseMsg(rep, rep_len, reg, tsk, mValues, mValues_pid);
    extractAppSignals();
	free(rep);

    return 0;
	
}

int CClient::terminate()
{
    void* rep = NULL;
	int rep_len = 0;

	std::string msg = mProtocol.termMsg(mReg);

    mCom.msm_send(msg.c_str());
    mCom.msm_recv(&rep, rep_len);
	free(rep);

    mCom.msm_shutdown();


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
    void* rep = malloc(4096);
    if (NULL == rep){
        std::cout << "[FATAL] out of memory!" << std::endl;
        exit(-1);
    }
    int rep_len = 0;

    std::string msg = mProtocol.freqMsg(mReg);
    mCom.msm_send(msg.c_str());
    mCom.msm_recv(&rep, rep_len);
    if(rep == NULL)
    {
        std::cout << "rep is NULL..., length is" << rep_len << std::endl;
    }

    mProtocol.getData(rep, rep_len, vals);

    free(rep);
}

void CClient::initSocket()
{
    mCom.msm_socket();
    mCom.msm_connect(mPort, mIPaddr);
}
