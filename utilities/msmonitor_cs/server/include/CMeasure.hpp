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
	MS_LIST* getDataList();
	
private:
	MS_LIST* m1;

};

#endif