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
#include <string.h>
#include <getopt.h>
#include "CServer.hpp"
#include "CClient.hpp"

void printUsage();

int main(int argc, char **argv) {
	int c;
	while( (c = getopt(argc, argv, "sch")) != -1){
		switch(c){
			case 's':{
				std::cout << "initiating server..." << std::endl; 
				CServer* srv = new CServer();
				srv -> init();
				srv -> acceptLoop();
				break;
			}
			case 'c':{
				std::cout << "initiating client..." << std::endl;
				CClient* client = new CClient();
				std::vector<int> values;
				values.push_back(X);
				values.push_back(YClockCpu0);
				values.push_back(YClockCpu1);
				values.push_back(YPowerCpu0);
				client->registerToServer(values);
				break;
			}
			case 'h':
				printUsage();
				break;
			default:
				printUsage();
				break;
		}
	}
	
	return 0;
}

void printUsage() {
	std::cout << "Usage: ./executable [option]\n" << std::endl;
	std::cout << "Options:\n-s\tserver (e.g. heterogenous node)\n-c\tclient (e.g. your local PC)\n-h\thelp (show this help)" << std::endl;
}