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
	CClient* client_a = new CClient();
	std::vector<int> values;
	values.push_back(X);
	values.push_back(YPowerCpu0);
	values.push_back(YPowerCpu1);
	client->registerToServer(values, 2900, "131.234.58.31");
	sleep(1);
//	client->requestData();
//	sleep(2);
//	values.push_back(YPowerSystem);
//	values.push_back(YTempCpu0);
	client_a->registerToServer(values, 2900, "131.234.58.31");
//	client_a->requestData();
	sleep(1);
	client_a->terminate();
	sleep(3);
	client->terminate();
	return 0;
}

