#include <CMeasure.hpp>


CMeasure::CMeasure(){}

CMeasure::~CMeasure(){}

void CMeasure::init(){
	// Initialize library and measurement system
	MS_VERSION version = { .major = MS_MAJOR_VERSION, .minor = MS_MINOR_VERSION, .revision = MS_REVISION_VERSION };
	MS_SYSTEM *ms = ms_init(&version, CPU_GOVERNOR_ONDEMAND, 2000000, 2500000, GPU_FREQUENCY_CUR, IPMI_SET_TIMEOUT, SKIP_PERIODIC, VARIANT_FULL);
	
	// Allocate measurement list
	m1 = ms_alloc_measurement(ms);
	
	// Set timer for m1. Measurements perform every (10ms/30ms)*10 = 100ms/300ms.
	ms_set_timer(m1, CPU , 0, 10000000, 10);
	ms_set_timer(m1, GPU , 0, 30000000, 10);
	ms_set_timer(m1, FPGA , 0, 30000000, 10);
	ms_set_timer(m1, SYSTEM , 0, 100000000, 10);
	ms_set_timer(m1, MIC , 0, 30000000, 10);
	ms_init_measurement(ms, m1, ALL);
}

MS_LIST* CMeasure::getDataList(){
	return this->m1;
}