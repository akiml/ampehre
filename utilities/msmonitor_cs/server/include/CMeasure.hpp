#ifndef CMEASURE_HPP
#define CMEASURE_HPP

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "ms_measurement.h"
#include "CDataHandler.hpp"

class CMeasure{
	
	
public:
	CMeasure();
	~CMeasure();
	void start();
	void stop();
	void getValues(std::vector<double>& sol, std::vector<int>& req);
	
private:
	NData::CDataHandler mHandler;


};

#endif