#include <iostream>
#include <ostream>
#include <time.h>
#include <chrono>
#include <cstring>
#include "CWebMeasure.h"

CWebMeasure::CWebMeasure(long intervalNs, int bufferSize){

	mIntervalNs = intervalNs;
	mBufferSize = bufferSize;
	mBuffer = new struct measure_data[mBufferSize]();


//	timestamp + 14*double + 14*uint32_t + 2*uint64_t + newline
//  fpga process name + nvml max process * nvml max process name

	long maxline = 
		// prefix + suffix in json mode
		27 +
		// timestamps + ,
		// analog to uint64_t
		2*(21) +
		// 13*(double + ,)
		// max digits with %f -MAX_DBL : 317
		// with %20.20f: 42
		13*(42 + 1) +
		// 14*(uint32_t + ,)
		// max uint32_t value: 4294967296
		// decimal digits: 10
		14*11 +
		// 3*(uint64_t + ,)
		// max uint64_t value: 18446744073709551616
		// decimal digits: 20
		3*21 +
		// newline + zero byte
		2 +
		// fpga process name + "",
		//MAXELER_BUFFER_SIZE +3 +
		// nvml max process count * nvml max process name + "",
		//(MAX_PROCESS_COUNT * (NVML_BUFFER_SIZE+1))+3
		0;

	mCSV = (char*) new char[maxline*(mBufferSize+1)];

	mThread = std::thread(&CWebMeasure::intervalThread, this);

}

CWebMeasure::~CWebMeasure(){

	if (0 == mStop) {
		mStop = 1;
		{
			std::unique_lock<std::timed_mutex> ul(mMutex);
			mMutexCond.notify_one();
		}
	}
	if (mThread.joinable()) {
		mThread.join();
	}

	if (0 != mBuffer) {
		delete[] mBuffer;
		mBuffer = 0;
	}

	if (0 != mCSV) {
		delete[] mCSV;
		mCSV = 0;
	}

}

void CWebMeasure::startMeasurement(){

	mpVersion             = new MS_VERSION;
	mpVersion->major      = MS_MAJOR_VERSION;
	mpVersion->minor      = MS_MINOR_VERSION;
	mpVersion->revision   = MS_REVISION_VERSION;

	mpMSystem = ms_init(mpVersion, CPU_GOVERNOR_ONDEMAND, 1500000, 2500000, GPU_FREQUENCY_CUR, IPMI_SET_TIMEOUT, SKIP_PERIODIC, VARIANT_FULL);

	mpMList = ms_alloc_measurement(mpMSystem);
	long sec = mIntervalNs/1000000000;
	long nsec = mIntervalNs%1000000000;
	ms_set_timer(mpMList, CPU   , sec,  nsec, 1);
	ms_set_timer(mpMList, GPU   , sec,  nsec, 1);
	ms_set_timer(mpMList, FPGA  , sec,  nsec, 1);
	ms_set_timer(mpMList, SYSTEM, sec,  nsec, 1);
	ms_set_timer(mpMList, MIC   , sec,  nsec, 1);

	ms_init_measurement(mpMSystem, mpMList, CPU|GPU|FPGA|MIC|SYSTEM);

	ms_start_measurement(mpMSystem);

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		mStart = 1;
		mMutexCond.notify_one();
	}
}

void CWebMeasure::stopMeasurement(){

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		mStop = 1;
		mMutexCond.notify_one();
	}
	if (mThread.joinable()) {
		mThread.join();
	}

	if (0 != mpMSystem) {
		ms_stop_measurement(mpMSystem);
		ms_join_measurement(mpMSystem);
		ms_fini_measurement(mpMSystem);
		ms_free_measurement(mpMList);
		ms_fini(mpMSystem);

		mpMList = 0;
		mpMSystem = 0;
		delete mpVersion;
		mpVersion = 0;
	}

}

void CWebMeasure::intervalThread(){

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		while (0 == mStart && 0 == mStop) {
			mMutexCond.wait(ul);
		}
	}
	if (1 == mStop) {
		return;
	}

	bool cpuEnabled = false;
	bool gpuEnabled = false;
	bool fpgaEnabled = false;
	bool micEnabled = false;
	bool sysEnabled = false;
	MS_MEASUREMENT_CPU *pMsMeasurementCpu = NULL;
	MS_MEASUREMENT_GPU *pMsMeasurementGpu = NULL;
	MS_MEASUREMENT_FPGA *pMsMeasurementFpga = NULL;
	MS_MEASUREMENT_MIC *pMsMeasurementMic = NULL;
	MS_MEASUREMENT_SYS *pMsMeasurementSys = NULL;

	if(mpMSystem->config->cpu_enabled) {
		pMsMeasurementCpu = (MS_MEASUREMENT_CPU *) getMeasurement(&mpMList, CPU);
		cpuEnabled = true;
	} else {
		pMsMeasurementCpu = new MS_MEASUREMENT_CPU;
		memset(pMsMeasurementCpu ,0, sizeof(MS_MEASUREMENT_CPU));
	}

	if(mpMSystem->config->gpu_enabled) {
		pMsMeasurementGpu = (MS_MEASUREMENT_GPU *) getMeasurement(&mpMList, GPU);
		gpuEnabled = true;
	} else {
		pMsMeasurementGpu = new MS_MEASUREMENT_GPU;
		memset(pMsMeasurementGpu ,0, sizeof(MS_MEASUREMENT_GPU));
	}

	if(mpMSystem->config->fpga_enabled) {
		pMsMeasurementFpga = (MS_MEASUREMENT_FPGA *) getMeasurement(&mpMList, FPGA);
		fpgaEnabled = true;
	} else {
		pMsMeasurementFpga = new MS_MEASUREMENT_FPGA;
		memset(pMsMeasurementFpga ,0, sizeof(MS_MEASUREMENT_FPGA));
	}

	if(mpMSystem->config->mic_enabled) {
		pMsMeasurementMic = (MS_MEASUREMENT_MIC *) getMeasurement(&mpMList, MIC);
		micEnabled = true;
	} else {
		pMsMeasurementMic = new MS_MEASUREMENT_MIC;
		memset(pMsMeasurementMic ,0, sizeof(MS_MEASUREMENT_MIC));
	}

	if(mpMSystem->config->sys_enabled) {
		pMsMeasurementSys = (MS_MEASUREMENT_SYS *) getMeasurement(&mpMList, SYSTEM);
		sysEnabled = true;
	} else {
		pMsMeasurementSys = new MS_MEASUREMENT_SYS;
		memset(pMsMeasurementSys ,0, sizeof(MS_MEASUREMENT_SYS));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	std::chrono::system_clock::time_point wakeup_time = std::chrono::system_clock::now();

	{
		std::unique_lock<std::timed_mutex> ul(mMutex);
		std::cv_status waitTimeout;
		while(0 == mStop) {

			wakeup_time += std::chrono::nanoseconds(mIntervalNs);

			waitTimeout = mMutexCond.wait_until(ul, wakeup_time);

			if (std::cv_status::no_timeout == waitTimeout || 1 == mStop) {
				break;
			}

			struct measure_data* data = &(mBuffer[mBufferPos]);
			data->time.tv_sec = 
				std::chrono::time_point_cast<std::chrono::seconds>(wakeup_time).time_since_epoch().count();
			data->time.tv_nsec =
				std::chrono::time_point_cast<std::chrono::nanoseconds>(wakeup_time).time_since_epoch().count() - data->time.tv_sec*1000000000;

			data->cpu = *pMsMeasurementCpu;
			data->gpu = *pMsMeasurementGpu;
			data->fpga = *pMsMeasurementFpga;
			data->mic = *pMsMeasurementMic;
			data->sys = *pMsMeasurementSys;


			mBufferPos++;
			if (mBufferPos == mBufferSize) {
				mBufferPos = 0;
			}
			mCount++;
		}
	}

	if(false == cpuEnabled) {
		delete pMsMeasurementCpu;
	}

	if(false == gpuEnabled) {
		delete pMsMeasurementGpu;
	}

	if(false == fpgaEnabled) {
		delete pMsMeasurementFpga;
	}

	if(false == micEnabled) {
		delete pMsMeasurementMic;
	}

	if(false == sysEnabled) {
		delete pMsMeasurementSys;
	}


}

int printList(char* target, struct CWebMeasure::measure_data* data){

	double cpu0_power = (data->cpu.msr_power_cur[0][PKG] 
		+ data->cpu.msr_power_cur[0][DRAM])/1000.0;
	double cpu1_power = (data->cpu.msr_power_cur[1][PKG] 
		+ data->cpu.msr_power_cur[1][DRAM])/1000.0;
	double gpu_power = data->gpu.nvml_power_cur/1000.0;
	double fpga_power = data->fpga.maxeler_power_cur[POWER]/1000.0;
	double mic_power = data->mic.mic_power_cur[MIC_POWER]/1000.0;
	double sys_power = data->sys.ipmi_power_server_cur;

	uint32_t cpu0_temp = data->cpu.msr_temperature_pkg_cur[0];
	uint32_t cpu1_temp = data->cpu.msr_temperature_pkg_cur[1];
	uint32_t gpu_temp = data->gpu.nvml_temperature_cur;
	double fpga_tempM = data->fpga.maxeler_temperature_cur[MTEMP];
	double fpga_tempI = data->fpga.maxeler_temperature_cur[ITEMP];
	uint32_t mic_temp = data->mic.mic_temperature_cur[MIC_DIE_TEMP];
	double sys_temp = data->sys.ipmi_temperature_sysboard_cur;

	double clockCpu0_temp = 0.0;
	double clockCpu1_temp = 0.0;
	
	for (int i=0; i<CORES; ++i) {
		clockCpu0_temp  += data->cpu.msr_freq_core_eff_cur[0][i];
		clockCpu1_temp  += data->cpu.msr_freq_core_eff_cur[1][i];
	}

	double cpu0_clock = clockCpu0_temp / CORES;
	double cpu1_clock = clockCpu1_temp / CORES;

	uint32_t gpu_clock_graphics = data->gpu.nvml_clock_graphics_cur;
	uint32_t gpu_clock_mem = data->gpu.nvml_clock_mem_cur;

	uint32_t mic_clock_core = data->mic.mic_freq_core_cur;
	uint32_t mic_clock_mem = data->mic.mic_freq_mem_cur;

	double cpu_util = data->cpu.measure_util_avg_cur;
	uint32_t gpu_util_core = data->gpu.nvml_util_gpu_cur;
	uint32_t gpu_util_mem = data->gpu.nvml_util_mem_cur;
	double fpga_util = data->fpga.maxeler_util_comp_cur;
	uint64_t mic_util = data->mic.mic_util_avg_cur;

	uint64_t cpu_memory = data->cpu.measure_memory_cur[CPU_MEM_RAM_USED]>>10;
	uint64_t cpu_swap = data->cpu.measure_memory_cur[CPU_MEM_SWAP_USED]>>10;
	uint32_t gpu_memory = data->gpu.nvml_memory_used_cur>>10;
	uint32_t mic_memory = data->mic.mic_memory_used_cur>>10;

	//uint32_t fpga_pid = data->fpga.maxeler_active_process_pid;
	//char* fpga_pname = data->fpga.maxeler_active_process_name;
	//char* fpga_puser = data->fpga.maxeler_active_process_user;

	uint32_t gpu_pcount = data->gpu.nvml_active_processes_count_cur;

	int ret = 0;

	ret = sprintf(target,
		"%ld,%ld,%f,%f,%f,%f,%f,%f,%d,%d,%d,%f,%f,%d,%f,%f,%f,%d,%d,%d,%d,%f,%d,%d,%f,%ld,%ld,%ld,%d,%d,%d",
		data->time.tv_sec,
		data->time.tv_nsec,
		cpu0_power,
		cpu1_power,
		gpu_power,
		fpga_power,
		mic_power,
		sys_power,
		cpu0_temp,
		cpu1_temp,
		gpu_temp,
		fpga_tempM,
		fpga_tempI,
		mic_temp,
		sys_temp,
		cpu0_clock,
		cpu1_clock,
		gpu_clock_graphics,
		gpu_clock_mem,
		mic_clock_core,
		mic_clock_mem,
		cpu_util,
		gpu_util_core,
		gpu_util_mem,
		fpga_util,
		mic_util,
		cpu_memory,
		cpu_swap,
		gpu_memory,
		mic_memory,
		gpu_pcount
		);
		// fpga_pid

////  add fpga/gpu process names
//	int size = strnlen(data->fpga.maxeler_active_process_name, MAXELER_BUFFER_SIZE);
//	ret += sprintf(&(target[ret]), ",\"%.*s\",\"", size, data->fpga.maxeler_active_process_name);
//
//	for (int i=0; i<gpu_pcount; i++) {
//		size = strnlen(data->gpu.nvml_active_processes_name[i], NVML_BUFFER_SIZE);
//		ret += sprintf(&(target[ret]), "%.*s,", size, data->gpu.nvml_active_processes_name[i]);
//	}
//
//	if ( 0 < gpu_pcount) {
//		ret--; // remove last comma
//	}
//	ret += sprintf(&(target[ret]), "\"");

	return ret;
}
	const char* csv_header = 
"timestamp_s,\
timestamp_ns,\
cpu0_power,\
cpu1_power,\
gpu_power,\
fpga_power,\
mic_power,\
sys_power,\
cpu0_temp,\
cpu1_temp,\
gpu_temp,\
fpga_tempM,\
fpga_tempI,\
mic_temp,\
sys_temp,\
cpu0_clock,\
cpu1_clock,\
gpu_clock_graphics,\
gpu_clock_mem,\
mic_clock_core,\
mic_clock_mem,\
cpu_util,\
gpu_util_core,\
gpu_util_mem,\
fpga_util,\
mic_util,\
cpu_memory,\
cpu_swap,\
gpu_memory,\
mic_memory,\
gpu_pcount\
\n";
//fpga_pid,
//fpga_pname,
//gpu_pnames

char* CWebMeasure::generateCSV(){


	long pos = 0;
	strcpy(&(mCSV[pos]),csv_header);
	pos += strlen(csv_header);
	
	// for first $mBufferSize elements don't print empty elements
	if (mCount >= mBufferSize) {
		for (int i=mBufferPos; i<mBufferSize; i++) {
			pos += printList(&(mCSV[pos]), &(mBuffer[i]));
			pos += sprintf(&(mCSV[pos]),"\n");
		}
	}

	for (int i=0; i<mBufferPos; i++) {
		pos += printList(&(mCSV[pos]), &(mBuffer[i]));
		pos += sprintf(&(mCSV[pos]),"\n");
	}

	mCSV[pos] = 0;

	// TODO: parallel calls may corrupt buffer, duplicate string or move buffer to client?
	return mCSV;
}

char* CWebMeasure::generateJsonCSVBulk(){

	long pos = 0;

	// for first $mBufferSize elements don't print empty elements
	if (mCount >= mBufferSize) {
		for (int i=mBufferPos; i<mBufferSize; i++) {
			pos += sprintf(&(mCSV[pos]),"{\"index\": {}}\n{\"_data\":\"");
			pos += printList(&(mCSV[pos]), &(mBuffer[i]));
			pos += sprintf(&(mCSV[pos]),"\"}\n");
		}
	}

	for (int i=0; i<mBufferPos; i++) {
		pos += sprintf(&(mCSV[pos]),"{\"index\": {}}\n{\"_data\":\"");
		pos += printList(&(mCSV[pos]), &(mBuffer[i]));
		pos += sprintf(&(mCSV[pos]),"\"}\n");
	}

	mCSV[pos] = 0;

	return mCSV;
}
