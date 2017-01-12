/*
 * utils.cpp
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
 * created: 10/15/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation 
 */
#include "utils.h"

int ut::find(std::list<clReg> &lst, int value, std::list<clReg>::iterator &it) {

	it = lst.begin();
	
	while((*it).registry != value && it != lst.end()){
		it++;
	}
	
	if(it == lst.end())
		return -1;
	
	return 0;
}

int ut::getReg(std::list< clReg >& lst, int max) {
	std::list<clReg>::iterator it;
	
	for(int i = 0; i < max; i++){
		if(find(lst, i, it) < 0){
			return i;
		}
	}
	
	return -1;
}
