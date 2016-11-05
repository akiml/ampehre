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