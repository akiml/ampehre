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
#include <string.h>

void printUsage();

int main(int argc, char *argv[]) {
	
	if(argc == 2){
		if(strcmp(argv[1], "s") == 0){
			printf("initiating server...\n");
			//stuff for server happens
		}
		else if(strcmp(argv[1], "c") == 0){
			printf("initialing client...\n");
			//stuff for client happens
		}
		else{
			printUsage();
		}
	}else{
		printUsage();
	}
	
	return 0;
}

void printUsage() {
	printf("Usage: ./executable [option]\n\n");
	printf("Options:\ns\tserver (e.g. heterogenous node)\nc\tclient (e.g. your local PC)\n");
}