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
