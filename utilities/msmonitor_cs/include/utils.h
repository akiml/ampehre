/*
 * utils.h
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

#ifndef UTILS_AEA
#define UTILS_AEA

#include <list>
#include <inttypes.h>

struct clReg{
	int registry;
	uint64_t dataCode;
};

namespace ut{
	
	//returns first value found
	int find (std::list<clReg> &lst, int value, std::list<clReg>::iterator &it);
	
	//search free registry with max clients
	int getReg(std::list<clReg> &lst, int max);
	
}



#endif