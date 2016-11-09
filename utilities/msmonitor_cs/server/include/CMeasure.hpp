#ifndef CMEASURE_HPP
#define CMEASURE_HPP

#include <stdio.h>
#include <unistd.h>
#include "ms_measurement.h"

class CMeasure{
	
	
public:
	CMeasure();
	~CMeasure();
	void init();
	void start();
	void stop();
	MS_LIST* getDataList();
	
private:
	MS_LIST* mList;
	MS_SYSTEM* mSys;

};

#endif