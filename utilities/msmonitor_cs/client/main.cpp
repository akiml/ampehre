/*
 * main.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Ahmad El-Ali	<aelali@mail.upb.de>
 * created: 06.10.16
 * version: 0.1- client/server monitoring implementation
 */

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "CClient.hpp"

int main(int argc, char **argv) {

	std::cout << "initiating client..." << std::endl;
	CClient* client = new CClient();
	CClient* client_1 = new CClient();

	std::vector<int> values;
	CProtocolC::addAll(values);
	
	std::vector<int> values_1;
	values_1.clear();
	CProtocolC::addMemory(values_1);
	
	client->registerToServer(values, 2900, "131.234.58.31");
	client_1->registerToServer(values_1, 2900, "131.234.58.31");
	client->requestData();
	sleep(1);
	client_1->requestData();
	sleep(2);
	client->requestData();
	sleep(1);
	client_1->requestData();
	sleep(1);
	client->terminate();
	client_1->terminate();
	
	
	delete client;
	delete client_1;
	
	return 0;
}

