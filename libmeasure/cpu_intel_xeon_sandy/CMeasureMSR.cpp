/*
 * CMeasureMSR.cpp
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
 * created: 3/20/14
 * version: 0.1.0 - initial implementation
 *          0.1.3 - add CPU frequency measurements
 *          0.1.5 - add CPU performance counters
 *          0.1.7 - add CPU utilization measurements
 *          0.1.15 - make CPU frequency settable
 *          0.2.0 - add support for msr registers to the measure driver
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library. 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cpufreq.h>

#define UINT32_MAX (0xffffffff)

namespace NLibMeasure {
	template <int SkipMs, int Variant>
	CMeasureMSR<SkipMs, Variant>::CMeasureMSR(CLogger& rLogger, cpu_governor cpuGovernor, uint64_t cpuFrequencyMin, uint64_t cpuFrequencyMax) :
		CMeasureAbstractResource(rLogger),
		mMinTccActivationTemp(0),
		mEnergyUnit(0.0),
		mCpuGovernor(cpuGovernor),
		mCpuFrequencyMin(cpuFrequencyMin),
		mCpuFrequencyMax(cpuFrequencyMax)
		{
		
		mFildesMeasure	= -1;
		
		init();
	}
	
	template <int SkipMs, int Variant>
	CMeasureMSR<SkipMs, Variant>::~CMeasureMSR() {
		destroy();
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::init(void) {
		if(Variant==FULL) {
			mrLog()
			<< ">>> 'msr' (full version)" << std::endl;
		} else {
			mrLog()
			<< ">>> 'msr' (light version)" << std::endl;
		}
		
		openMeasureDevice();
		
		int pseudo_thread_num	= -1;
		mEnergyUnit				= msrGetEnergyUnit(pseudo_thread_num);
		mMinTccActivationTemp	= msrGetMinumimTccActivation(pseudo_thread_num);
		mEnergyMax				= msrGetEnergyMax();
		
		mrLog() 
		<< ">>> 'msr' (thread main)" << std::endl
		<< "     one measured unit is   : " << mEnergyUnit*1000000 << " µWs" << std::endl
		<< "     min tcc activation temp: " << mMinTccActivationTemp << " \u00b0C" << std::endl
		<< "     max register energy val: " << mEnergyMax << " Ws" << std::endl;
		
		mrLog()
		<< ">>> 'msr' (thread main): set APERF counters to 0x0." << std::endl
		<< ">>> 'msr' (thread main): set MPERF counters to 0x0." << std::endl;
		resetMsrPerfCounters();
		
		showCurrentCpuFreqSettings();
		
		bool frequency_min_is_valid	= false;
		if (mCpuFrequencyMin == 0) {
			uint64_t temp;
			cpufreq_get_hardware_limits(0, &mCpuFrequencyMin, &temp);
		}
		
		bool frequency_max_is_valid	= false;
		if (mCpuFrequencyMax == 0) {
			uint64_t temp;
			cpufreq_get_hardware_limits(0, &temp, &mCpuFrequencyMax);
		}
		
		bool governor_is_valid		= false;
		std::string governor_string("");
		switch (mCpuGovernor) {
			case CPU_GOVERNOR_PERFORMANCE:
				governor_string.append("performance");
				break;
			case CPU_GOVERNOR_CONSERVATIVE:
				governor_string.append("conservative");
				break;
			case CPU_GOVERNOR_POWERSAVE:
				governor_string.append("powersave");
				break;
			case CPU_GOVERNOR_ONDEMAND:
			default:
				governor_string.append("ondemand");
				break;
		}
		
		struct cpufreq_available_frequencies *freqs	= NULL;
		freqs	= cpufreq_get_available_frequencies(0);
		if (NULL == freqs) {
			mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cannot obtain supported cpu frequencies. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog()
		<< ">>> 'msr' (thread main): supported frequencies" << std::endl
		<< "     ";
		struct cpufreq_available_frequencies *temp_freqs	= freqs;
		while (NULL != temp_freqs) {
			if (NULL != temp_freqs->next) {
				mrLog()
				<< temp_freqs->frequency/1000 << " MHz, ";
			} else {
				mrLog()
				<< temp_freqs->frequency/1000 << " MHz" << std::endl;
			}
			
			if (temp_freqs->frequency == mCpuFrequencyMin) {
				frequency_min_is_valid	= true;
			}
			
			if (temp_freqs->frequency == mCpuFrequencyMax) {
				frequency_max_is_valid	= true;
			}
			
			temp_freqs = temp_freqs->next;
		}
		
		cpufreq_put_available_frequencies(freqs);
		
		if (frequency_min_is_valid == false || frequency_max_is_valid == false) {
			mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cpu frequency settings aren't valid. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		if (mCpuFrequencyMax < mCpuFrequencyMin) {
			mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: max cpu frequency is smaller than min cpu frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		struct cpufreq_available_governors* govs	= NULL;
		govs	= cpufreq_get_available_governors(0);
		if (NULL == govs) {
			mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cannot obtain supported cpufreq governors. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog()
		<< ">>> 'msr' (thread main): supported governors" << std::endl
		<< "     ";
		struct cpufreq_available_governors* temp_govs	= govs;
		while (NULL != temp_govs) {
			if (NULL != temp_govs->next) {
				mrLog()
				<< "'" << temp_govs->governor << "', ";
			} else {
				mrLog()
				<< "'" << temp_govs->governor << "'" << std::endl;
			}
			
			if (governor_string.compare(temp_govs->governor) == 0) {
				governor_is_valid = true;
			}
			
			temp_govs = temp_govs->next;
		}
		
		cpufreq_put_available_governors(govs);
		
		if (governor_is_valid == false) {
			mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cpu governor is not available. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog()
		<< ">>> 'msr' (thread main): "
		<< "activate governor '" << governor_string << "' "
		<< "with min frequency " << mCpuFrequencyMin/1000 << " MHz "
		<< "and max frequency " << mCpuFrequencyMax/1000 << " MHz."
		<< std::endl;
		
		for (int i=0; i<CPUS*CORES; ++i) {
			cpufreq_modify_policy_min(i, mCpuFrequencyMin);
			cpufreq_modify_policy_max(i, mCpuFrequencyMax);
			cpufreq_modify_policy_governor(i, const_cast<char*>(governor_string.c_str()));
		}
		
		showCurrentCpuFreqSettings();
		
		mrLog()
		<< ">>> 'msr' (thread main): initialization done." << std::endl
		<< std::endl;
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::destroy(void) {
		closeMeasureDevice();
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::showCurrentCpuFreqSettings (void) {
		mrLog()
		<< ">>> 'msr' (thread main): current cpufreq settings and hardware limits" << std::endl
		<< "     | core | hw minimum | gov minimum |  current  | gov maximum | hw maximum |   governor   |" << std::endl
		<< "     |------|------------|-------------|-----------|-------------|------------|--------------|" << std::endl;
		for (int i=0; i<CPUS*CORES; ++i) {
			struct cpufreq_policy* policy	= NULL;
			uint64_t hw_min	= 0;
			uint64_t hw_max	= 0;
			
			cpufreq_get_hardware_limits(i, &hw_min, &hw_max);
			policy = cpufreq_get_policy(i);
			if (NULL == policy) {
				mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cannot obtain cpufreq policy. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
			}
			
			mrLog()
			<< "     "
			<< "| " << std::setw(4) << i << " "
			<< "| " << std::setw(6) << hw_min/1000 << " MHz "
			<< "| " << std::setw(7) << policy->min/1000 << " MHz "
			<< "| " << std::setw(5) << cpufreq_get_freq_kernel(i)/1000 << " MHz "
			<< "| " << std::setw(7) << policy->max/1000 << " MHz "
			<< "| " << std::setw(6) << hw_max/1000 << " MHz "
			<< "| " << std::setw(12) << policy->governor << " |"
			<< std::endl;
			
			cpufreq_put_policy(policy);
		}
	}
	
	template <int SkipMs, int Variant>
	int CMeasureMSR<SkipMs, Variant>::getVariant() {
		return Variant;
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::measure(MEASUREMENT* pMeasurement, int32_t& rThreadNum) {
		mEnergyUnit				= msrGetEnergyUnit(rThreadNum);
		mEnergyMax				= msrGetEnergyMax();
		if(Variant == FULL) {
			mMinTccActivationTemp	= msrGetMinumimTccActivation(rThreadNum);
		}
		
		for (int i=0; i<CPUS; ++i) {
			pMeasurement->msr_energy_cur[i][PKG]		= msrGetEnergy(rThreadNum, i, MSR_PKG_ENERGY_STATUS)*1000.0;
			pMeasurement->msr_energy_cur[i][PP0]		= msrGetEnergy(rThreadNum, i, MSR_PP0_ENERGY_STATUS)*1000.0;
			pMeasurement->msr_energy_cur[i][DRAM]		= msrGetEnergy(rThreadNum, i, MSR_DRAM_ENERGY_STATUS)*1000.0;
			if(Variant == FULL) {
				if(!(mMeasureCounter % SkipMs)) {
					pMeasurement->msr_temperature_pkg_cur[i]	= msrGetTemperature(rThreadNum, i, IA32_PACKAGE_THERM_STATUS);
				}
				
				for (int j=0; j<CORES; ++j) {
					if(!(mMeasureCounter % SkipMs)) {
						pMeasurement->msr_temperature_core_cur[i][j]		= msrGetTemperature(rThreadNum, j*CPUS+i, IA32_THERM_STATUS);
					}
					pMeasurement->internal.msr_timestamp_core_cur[i][j]	= msrGetTimeStamp(rThreadNum, j*CPUS+i, IA32_TIME_STAMP_COUNTER);
					msrGetPerfCounter(rThreadNum, j*CPUS+i,
									IA32_APERF, &(pMeasurement->internal.msr_aperf_core_cur[i][j]),
									IA32_MPERF, &(pMeasurement->internal.msr_mperf_core_cur[i][j]));
				}
			}
		}
		
		pMeasurement->internal.msr_energy_max	= mEnergyMax * 1000.0;
		
		measureGetUtilization(rThreadNum, pMeasurement->internal.measure_util_cur);
		
		if(Variant == FULL) {
			if(!(mMeasureCounter++ % SkipMs)) {
				measureGetMemInfo(rThreadNum, pMeasurement->measure_memory_cur);
			}
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::readMeasureDevice(int32_t& rThreadNum, int32_t dev, int32_t coreNumber, uint64_t address, uint64_t *pValues, int32_t size) {
		lseek(mFildesMeasure, ((uint64_t)coreNumber << CPU_NO_POS) | address, dev);
		
		if (read(mFildesMeasure, pValues, size) < 0) {
			mrLog.lock();
			mrLog(CLogger::scErr) << "!!! 'msr thread' (thread #" << rThreadNum << "): Error: cannot read from measure device driver. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::writeMeasureDevice(int32_t& rThreadNum, int32_t dev, int32_t coreNumber, uint64_t address, uint64_t valueToWrite) {
		lseek(mFildesMeasure, ((uint64_t)coreNumber << CPU_NO_POS)| address, dev);
		
		if (write(mFildesMeasure, &valueToWrite, sizeof(valueToWrite))<0) {
			mrLog.lock();
			if (rThreadNum < 0) {
				mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cannot write to measure device driver. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			} else {
				mrLog(CLogger::scErr) << "!!! 'msr thread' (thread #" << rThreadNum << "): Error: cannot write to measure device driver. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			}
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::openMeasureDevice(void) {
		std::ostringstream strstream;
		strstream << "/dev/measure";
		
		mFildesMeasure = open(strstream.str().c_str(), O_RDWR);
		if (mFildesMeasure < 0) {
			mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: cannot open measure device driver. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::closeMeasureDevice(void) {
		close(mFildesMeasure);
	}
	
	template <int SkipMs, int Variant>
	double CMeasureMSR<SkipMs, Variant>::msrGetEnergyUnit(int32_t& rThreadNum) {
		uint64_t	result0;
		uint64_t	result1;
		double		energy_unit;
		
		// get energy units [µWs]
		readMeasureDevice(rThreadNum, cpu_msr, 0, MSR_RAPL_POWER_UNIT, &result0, sizeof(result0));
		readMeasureDevice(rThreadNum, cpu_msr, 1, MSR_RAPL_POWER_UNIT, &result1, sizeof(result1));
		energy_unit	= pow(0.5, (result0>>8)&0x1f);

		if (result0 != result1) {
			mrLog.lock();
			if (rThreadNum >= 0) {
				mrLog(CLogger::scErr) << "!!! 'msr thread' (thread #" << rThreadNum << "): Error: energy unit values of file descriptor 0 and 1 have to be equal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			} else {
				mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: energy unit values of file descriptor 0 and 1 have to be equal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			}
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		return energy_unit;
	}
	
	template <int SkipMs, int Variant>
	double CMeasureMSR<SkipMs, Variant>::msrGetEnergyMax(void) {
		double energy_max;
		
		energy_max = UINT32_MAX * mEnergyUnit;
		
		return energy_max;
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::resetMsrPerfCounters(void) {
		int32_t threadNum = -1;
		
		for (int i=0; i<NUM_OF_CORES; ++i) {
			writeMeasureDevice(threadNum, cpu_msr, i, IA32_APERF, 0);
			writeMeasureDevice(threadNum, cpu_msr, i, IA32_MPERF, 0);
		}
	}
	
	template <int SkipMs, int Variant>
	uint64_t CMeasureMSR<SkipMs, Variant>::msrGetMinumimTccActivation(int32_t& rThreadNum) {
		uint64_t	result0;
		uint64_t	result1;
		uint64_t	min_tcc;
		
		readMeasureDevice(rThreadNum, cpu_msr, 0, MSR_TEMPERATURE_TARGET, &result0, sizeof(result0));
		readMeasureDevice(rThreadNum, cpu_msr, 1, MSR_TEMPERATURE_TARGET, &result1, sizeof(result1));
		min_tcc	= (result0>>16)&0xff;
		
		if (result0 != result1) {
			mrLog.lock();
			if (rThreadNum >= 0) {
				mrLog(CLogger::scErr) << "!!! 'msr thread' (thread #" << rThreadNum << "): Error: minimum tcc activation values of file descriptor 0 and 1 have to be equal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			} else {
				mrLog(CLogger::scErr) << "!!! 'msr' (thread main): Error: minimum tcc activation values of file descriptor 0 and 1 have to be equal. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			}
			mrLog.unlock();
			exit(EXIT_FAILURE);
		}
		
		return min_tcc;
	}
	
	template <int SkipMs, int Variant>
	uint32_t CMeasureMSR<SkipMs, Variant>::msrGetTemperature(int32_t& rThreadNum, int32_t coreNumber, int32_t msrRegisterAddr) {
		uint64_t result;
		uint32_t temperature;
		
		readMeasureDevice(rThreadNum, cpu_msr, coreNumber, (uint64_t)msrRegisterAddr, &result, sizeof(result));
		temperature	= mMinTccActivationTemp - ((result>>16)&0x7f);
		
		return temperature;
	}
	
	template <int SkipMs, int Variant>
	double CMeasureMSR<SkipMs, Variant>::msrGetEnergy(int32_t& rThreadNum, int32_t coreNumber, int32_t msrRegisterAddr) {
		uint64_t result;
		double energy;
		
		readMeasureDevice(rThreadNum, cpu_msr, coreNumber, (uint64_t)msrRegisterAddr, &result, sizeof(result));
		energy	= (result&0xffffffff) * mEnergyUnit;
		
		return energy;
	}
	
	template <int SkipMs, int Variant>
	uint64_t CMeasureMSR<SkipMs, Variant>::msrGetTimeStamp(int32_t &rThreadNum, int32_t coreNumber, int32_t msrRegisterAddr) {
		uint64_t result;
		
		readMeasureDevice(rThreadNum, cpu_msr, coreNumber, (uint64_t)msrRegisterAddr, &result, sizeof(result));
		
		return result;
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::msrGetPerfCounter(int32_t& rThreadNum, int32_t coreNumber,
										int32_t msrRegisterAddrAPerf, uint64_t* pResultAPerf,
										int32_t msrRegisterAddrMPerf, uint64_t* pResultMPerf) {
		readMeasureDevice(rThreadNum, cpu_msr, coreNumber, (uint64_t)msrRegisterAddrAPerf, pResultAPerf, sizeof(*pResultAPerf));
		readMeasureDevice(rThreadNum, cpu_msr, coreNumber, (uint64_t)msrRegisterAddrMPerf, pResultMPerf, sizeof(*pResultMPerf));
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::measureGetUtilization(int32_t& rThreadNum, uint64_t* pMeasureUtilization) {
		readMeasureDevice(rThreadNum, cpu_stats, NUM_OF_CORES, 0, pMeasureUtilization, sizeof(uint64_t)*CPUSTATS);
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMSR<SkipMs, Variant>::measureGetMemInfo(int32_t& rThreadNum, uint64_t* pMeasureMemInfo) {
#ifndef LSEEK_MAX_2
		readMeasureDevice(rThreadNum, cpu_meminfo, 0, 0, pMeasureMemInfo, sizeof(uint64_t)*MEMINFO_SIZE);
#else
		readMeasureDevice(rThreadNum, cpu_meminfo, MEMINFO >> CPU_NO_POS, 0, pMeasureMemInfo, sizeof(uint64_t)*MEMINFO_SIZE);
#endif
	}

}
