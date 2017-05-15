/*
 * CMeasure.hpp
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
 * created: 10/5/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */

#ifndef CMEASURE_HPP
#define CMEASURE_HPP

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "CProtocol.hpp"
#include "ms_measurement.h"
#include "CDataHandler.hpp"


class CMeasure{
	
	
public:
	CMeasure();
	~CMeasure();
	void start();
	void stop();
	void getValues(std::vector<double>& sol, std::vector<int>& req);
	void getProcesses(std::vector<std::string>& processes);
	void getCurrentTime(double& time);
	NData::CDataSettings& getSettings();
	
private:
	NData::CDataHandler* mpHandler;


};

#endif
