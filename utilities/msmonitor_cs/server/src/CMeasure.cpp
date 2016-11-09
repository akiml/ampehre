#include "CMeasure.hpp"


CMeasure::CMeasure(){}

CMeasure::~CMeasure(){}

void CMeasure::init(){
	// Initialize library and measurement system
	MS_VERSION version = {MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION };
	mSys = ms_init(&version, CPU_GOVERNOR_ONDEMAND, 2000000, 2500000, GPU_FREQUENCY_CUR, IPMI_SET_TIMEOUT, SKIP_PERIODIC, VARIANT_FULL);
	
	// Allocate measurement list
	mList = ms_alloc_measurement(mSys);
	
	// Set timer for m1. Measurements perform every (10ms/30ms)*10 = 100ms/300ms.
	ms_set_timer(mList, CPU , 0, 10000000, 10);
	ms_set_timer(mList, GPU , 0, 30000000, 10);
	ms_set_timer(mList, FPGA , 0, 30000000, 10);
	ms_set_timer(mList, SYSTEM , 0, 100000000, 10);
	ms_set_timer(mList, MIC , 0, 30000000, 10);
	ms_init_measurement(mSys, mList, ALL);
}

MS_LIST* CMeasure::getDataList(){
	return this->mList;
}

void CMeasure::start() {
	ms_start_measurement(mSys);
}

void CMeasure::stop() {
		// Stop all measurement procedures.
	ms_stop_measurement(mSys);
	
	// Join measurement threads and remove thread objects.
	ms_join_measurement(mSys);
	ms_fini_measurement(mSys);
		// Cleanup the environment before exiting the program
	ms_free_measurement(mList);
	ms_fini(mSys);
}
