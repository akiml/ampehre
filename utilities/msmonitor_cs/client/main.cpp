/*
 * main.cpp
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
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "CClient.hpp"

int main(int argc, char **argv) {

	std::cout << "initiating client..." << std::endl;
	CClient* client = new CClient();
	CClient* cl = new CClient();

	std::vector<int> values;
	CProtocolC::addAll(values);

	std::vector<int> v;
	CProtocolC::addUtil(v);
	
	client->registerToServer(values, 2900, "131.234.58.31");
	client->requestData();
	cl->registerToServer(v, 2900, "131.234.58.31");
	client->requestData();
	cl->requestData();
	cl->terminate();
	client->terminate();
	
	
	delete client;
	delete cl;
	
	return 0;
}

