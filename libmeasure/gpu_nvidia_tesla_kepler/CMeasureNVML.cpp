/*
 * CMeasureNVML.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 3/04/14
 * version: 0.1.0 - initial implementation
 *          0.1.2 - add GPU frequency measurements
 *          0.1.8 - add GPU utilization measurements
 *          0.1.13 - make GPU frequency settable
 *          0.2.3 - add gpu_management tool and use the tool in the libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
 */

namespace NLibMeasure {
	template <int SkipMs, int Variant>
	CMeasureNVML<SkipMs, Variant>::CMeasureNVML(CLogger& rLogger, gpu_frequency gpuFrequency) :
		CMeasureAbstractResource(rLogger),
		mGpuFrequency(gpuFrequency)
		{
		
		init();
	}
	
	template <int SkipMs, int Variant>
	CMeasureNVML<SkipMs, Variant>::~CMeasureNVML() {
		destroy();
	}
	
	template <int SkipMs, int Variant>
	void CMeasureNVML<SkipMs, Variant>::init(void) {
		if(Variant == FULL) {
			mrLog()
			<< ">>> 'nvml' (full version)" << std::endl;
		} else {
			mrLog()
			<< ">>> 'nvml' (light version)" << std::endl;
		}
		
		nvmlReturn_t result;
		int32_t rv;
		char const* args_set_pm[] = {"gpu_management", "-p 1", NULL};
		
		uint32_t device_count;
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		nvmlPciInfo_t pci;
		nvmlEnableState_t mode;
		std::string modes[2] = {"disabled", "enabled"};
		std::stringstream clk_gpu_str;
		std::stringstream clk_mem_str;
		nvmlPstates_t power_state;
		nvmlMemory_t memory;
		
		const uint32_t count			= 32;
		uint32_t clk_mem_cnt			= count;
		uint32_t clk_mem[count];
		uint32_t clk_mem_max			= 0;
		uint32_t clk_mem_min			= 0xffffffff;
		uint32_t clk_mem_set			= 0;
		uint32_t clk_gpu_min_arr_cnt	= count;
		uint32_t clk_gpu_min_arr[clk_gpu_min_arr_cnt];
		uint32_t clk_gpu_min			= 0xffffffff;
		uint32_t clk_gpu_max_arr_cnt	= count;
		uint32_t clk_gpu_max_arr[clk_gpu_max_arr_cnt];
		uint32_t clk_gpu_max			= 0;
		uint32_t clk_gpu_set			= 0;
		uint32_t memory_total			= 0;
		
		result = nvmlInit();
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot initialize nvml library. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		result = nvmlDeviceGetCount(&device_count);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot query device count. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		if (device_count > 1) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: this software has be rewritten if you want to support more than 1 device. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog() << ">>> 'nvml' (thread main): get gpu device handler...";
		mrLog.flush();
		
		result = nvmlDeviceGetHandleByIndex(0, &mDevice);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot get device handler. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog() << " done!" << std::endl;
		
		result = nvmlDeviceGetName(mDevice, name, NVML_DEVICE_NAME_BUFFER_SIZE);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot get device name. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		result = nvmlDeviceGetPciInfo(mDevice, &pci);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot get pci information. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		result = nvmlDeviceGetPowerManagementMode(mDevice, &mode);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: no power managment supported. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		result = nvmlDeviceGetPerformanceState(mDevice, &power_state);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: no performance state reading possible. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		result = nvmlDeviceGetSupportedMemoryClocks(mDevice, &clk_mem_cnt, clk_mem);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot obtain memory clock. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		for (int i=0; i<(int32_t)clk_mem_cnt; ++i) {
			clk_mem_min = (clk_mem[i]<clk_mem_min) ? clk_mem[i] : clk_mem_min;
			clk_mem_max = (clk_mem[i]>clk_mem_max) ? clk_mem[i] : clk_mem_max;
		}
		
		result = nvmlDeviceGetSupportedGraphicsClocks(mDevice, clk_mem_min, &clk_gpu_min_arr_cnt, clk_gpu_min_arr);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot obtain graphics clock. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		for (int32_t i=0; i<(int32_t)clk_gpu_min_arr_cnt; ++i) {
			clk_gpu_min = (clk_gpu_min_arr[i]<clk_gpu_min) ? clk_gpu_min_arr[i] : clk_gpu_min;
		}
		
		result = nvmlDeviceGetSupportedGraphicsClocks(mDevice, clk_mem_max, &clk_gpu_max_arr_cnt, clk_gpu_max_arr);
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: cannot obtain graphics clock. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		for (int32_t i=0; i<(int32_t)clk_gpu_max_arr_cnt; ++i) {
			clk_gpu_max = (clk_gpu_max_arr[i]>clk_gpu_max) ? clk_gpu_max_arr[i] : clk_gpu_max;
		}
		
		result = nvmlDeviceGetMemoryInfo(mDevice, &memory);
		if (NVML_SUCCESS != result) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread main): Error: cannot obtain memory informations. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		memory_total = (uint32_t)(memory.total >> 20);
		
		rv = exec_gpu_mgmt((char**)args_set_pm);
		if (rv) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: in gpu_management tool. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		mrLog()
		<< ">>> 'nvml' (thread main): persistence mode enabled." << std::endl;
		
		mrLog()
		<< ">>> 'nvml' (thread main):" << std::endl
		<< "     device         : " << name << std::endl
		<< "     pcie           : " << pci.busId << std::endl
		<< "     power mgmt mode: " << modes[mode] << std::endl
		<< "     power state cur: " << power_state << std::endl
		<< "     power state min: " << NVML_PSTATE_15 << std::endl
		<< "     power state max: " << NVML_PSTATE_0 << std::endl
		<< "     memory total   : " << memory_total << " MiB" << std::endl
		<< "     avail mem clks : ";
		for (int i=0; i<(int32_t)clk_mem_cnt; ++i) {
			if (i<(int32_t)clk_mem_cnt-1) {
				mrLog() << clk_mem[i] << " MHz, ";
			} else {
				mrLog() << clk_mem[i] << " MHz" << std::endl;
			}
		}
		
		mrLog()
		<< "     memory clk min : " << clk_mem_min << " MHz" << std::endl
		<< "     avail core clks: ";
		for (int32_t i=0; i<(int32_t)clk_gpu_min_arr_cnt; ++i) {
			if (i<(int32_t)clk_gpu_min_arr_cnt-1) {
				mrLog() << clk_gpu_min_arr[i] << " MHz, ";
			} else {
				mrLog() << clk_gpu_min_arr[i] << " MHz" << std::endl;
			}
		}
		mrLog()
		<< "     core clk min   : " << clk_gpu_min << " MHz" << std::endl;
		
		mrLog()
		<< "     memory clk max : " << clk_mem_max << " MHz" << std::endl
		<< "     avail core clks: ";
		for (int32_t i=0; i<(int32_t)clk_gpu_max_arr_cnt; ++i) {
			if (i<(int32_t)clk_gpu_max_arr_cnt-1) {
				mrLog() << clk_gpu_max_arr[i] << " MHz, ";
			} else {
				mrLog() << clk_gpu_max_arr[i] << " MHz" << std::endl;
			}
		}
		mrLog()
		<< "     core clk max   : " << clk_gpu_max << " MHz" << std::endl;
		
		switch (mGpuFrequency) {
			case GPU_FREQUENCY_MIN:
				clk_mem_set = clk_mem_min;
				clk_gpu_set = clk_gpu_min;
				break;
			case GPU_FREQUENCY_MAX:
				clk_mem_set = clk_mem_max;
				clk_gpu_set = clk_gpu_max;
				break;
			case GPU_FREQUENCY_CUR:
			default:
				clk_mem_set = 0;
				clk_gpu_set = 0;
				
				result = nvmlDeviceGetClockInfo(mDevice, NVML_CLOCK_MEM, &clk_mem_set);
				if (NVML_SUCCESS != result) {
					mrLog.lock();
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread main): Error: cannot read frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					mrLog.unlock();
					exit(EXIT_FAILURE);
				}
				
				result = nvmlDeviceGetClockInfo(mDevice, NVML_CLOCK_GRAPHICS, &clk_gpu_set);
				if (NVML_SUCCESS != result) {
					mrLog.lock();
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread main): Error: cannot read frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					mrLog.unlock();
					exit(EXIT_FAILURE);
				}
				
				break;
		}
		
		if (mGpuFrequency == GPU_FREQUENCY_MIN || mGpuFrequency == GPU_FREQUENCY_MAX) {
			// In these cases we actually set the GPU frequencies either to the maximum or minimum value.
			clk_gpu_str << "-c " << clk_gpu_set;
			clk_mem_str << "-m " << clk_mem_set;
			char const* args_set_clk[] = {"gpu_management", clk_gpu_str.str().c_str() , clk_mem_str.str().c_str(), NULL};
			rv = exec_gpu_mgmt((char**)args_set_clk);
			if (rv) {
				mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: in gpu_management tool. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
			}
						
			mrLog()
			<< ">>> 'nvml' (thread main): set core clk to " << clk_gpu_set << " MHz and mem clk to " << clk_mem_set << " MHz." << std::endl;
		} else {
			// We name the values *_set, but we don't set the frequency. We just print the current GPU frequency.
			mrLog()
			<< ">>> 'nvml' (thread main): current core clk is " << clk_gpu_set << " MHz and mem clk is " << clk_mem_set << " MHz." << std::endl;
		}
		mrLog()
		<< ">>> 'nvml' (thread main): wait for 15s to throttle gpu clocks." << std::endl;
		sleep(15);
		
		mrLog()
		<< ">>> 'nvml' (thread main): initialization done." << std::endl
		<< std::endl;
	}
	
	template <int SkipMs, int Variant>
	void CMeasureNVML<SkipMs, Variant>::destroy(void) {
		nvmlReturn_t result;
		int rv;
		char const* args_dis_pm[] = {"gpu_management", "-p 0", "-r", NULL};
		rv = exec_gpu_mgmt((char**)args_dis_pm);
		if (rv) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: in gpu_management tool. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		mrLog()
		<< "<<< 'nvml' (thread main): reset clocks and persistence mode disabled." << std::endl;
	
		result = nvmlShutdown();
		if (NVML_SUCCESS != result) {
			mrLog(CLogger::scErr) << "!!! 'nvml' (thread main): Error: Failed to destroy library environment. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureNVML<SkipMs, Variant>::read_memory_total(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		nvmlReturn_t result;
		nvmlMemory_t memory;
		
		result = nvmlDeviceGetMemoryInfo(mDevice, &memory);
		if (NVML_SUCCESS != result) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot obtain memory informations. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		pMeasurement->nvml_memory_total = (uint32_t)(memory.total >> 10);
	}
	
	template <int SkipMs, int Variant>
	void CMeasureNVML<SkipMs, Variant>::measure(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		nvmlReturn_t result;
		
		result = nvmlDeviceGetPowerUsage(mDevice, &(pMeasurement->nvml_power_cur));
		if (NVML_SUCCESS != result) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: no power usage reading possible. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		if(Variant == FULL) {
			nvmlMemory_t memory;
			if(!(mMeasureCounter++%SkipMs)) {
				result = nvmlDeviceGetMemoryInfo(mDevice, &memory);
				if (NVML_SUCCESS != result) {
					mrLog.lock();
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot obtain memory informations. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					mrLog.unlock();
					exit(EXIT_FAILURE);
				}
				pMeasurement->nvml_memory_free_cur = (uint32_t)(memory.free >> 10);
				pMeasurement->nvml_memory_used_cur = (uint32_t)(memory.used >> 10);
				
				result = nvmlDeviceGetPerformanceState(mDevice, (nvmlPstates_t*)&(pMeasurement->internal.nvml_power_state));
				if (NVML_SUCCESS != result) {
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: no performance state reading possible. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					exit(EXIT_FAILURE);
				}
			
				nvmlTemperatureSensors_t sensorType = NVML_TEMPERATURE_GPU;
			
				result = nvmlDeviceGetTemperature(mDevice, sensorType, &(pMeasurement->nvml_temperature_cur));
				if (NVML_SUCCESS != result) {
					mrLog.lock();
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot read temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					mrLog.unlock();
					exit(EXIT_FAILURE);
				}
			
				result = nvmlDeviceGetClockInfo(mDevice, NVML_CLOCK_SM, &(pMeasurement->nvml_clock_sm_cur));
				if (NVML_SUCCESS != result) {
					mrLog.lock();
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot read frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					mrLog.unlock();
					exit(EXIT_FAILURE);
				}
			
				result = nvmlDeviceGetClockInfo(mDevice, NVML_CLOCK_MEM, &(pMeasurement->nvml_clock_mem_cur));
				if (NVML_SUCCESS != result) {
					mrLog.lock();
					mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot read frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					mrLog.unlock();
					exit(EXIT_FAILURE);
				}
			}
			
			result = nvmlDeviceGetClockInfo(mDevice, NVML_CLOCK_GRAPHICS, &(pMeasurement->nvml_clock_graphics_cur));
			if (NVML_SUCCESS != result) {
				mrLog.lock();
				mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot read frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				mrLog.unlock();
				exit(EXIT_FAILURE);
			}
			
			nvmlUtilization_t utilization;
			
			result = nvmlDeviceGetUtilizationRates(mDevice, &utilization);
			if (NVML_SUCCESS != result) {
				mrLog.lock();
				mrLog(CLogger::scErr) << "!!! 'nvml thread' (thread #" << rThreadNum << "): Error: cannot obtain utilization rates. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				mrLog.unlock();
				exit(EXIT_FAILURE);
			}
			
			pMeasurement->nvml_util_gpu_cur	= utilization.gpu;
			pMeasurement->nvml_util_mem_cur	= utilization.memory;
		}
		
	}
	
	template <int SkipMs, int Variant>
	int CMeasureNVML<SkipMs, Variant>::exec_gpu_mgmt(char* args[]){
		pid_t new_pid = 0;
		int status_child = 1;

		new_pid = fork();
		if (new_pid == 0) {
			// I am the child process.
			execvp(args[0], args);
			// These lines of code should not been executed if execvp were successful!
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'nvml thread' Error: gpu_management tool couldn't be executed successfully. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		} else if (new_pid > 0) {
			// I am the parent process.
			wait(&status_child);
		} else {
			// Error: my_pid < 0
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'nvml thread' Error: Cannot fork process. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		return status_child;
		
	}
}
