/*
 * CMeasureOdroid.cpp
 * 
 * Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author : Achim Lösch (achim.loesch@upb.de)
 * created: 2/24/16
 * version: 0.7.4 - add support for Odroid XU4 systems
 */

#include "CMeasureOdroid.hpp"

namespace NLibMeasure {
	CMeasureOdroid::CMeasureOdroid(NLibMeasure::CLogger& rLogger):
		CMeasureAbstractResource(rLogger),
		mpSensorDevFSARM(0),
		mpSensorDevFSMEM(0),
		mpSensorDevFSG3D(0),
		mpSensorDevFSKFC(0),
		mpSensorSysFSTemp(0),
		mpSensorSysFSFreqA7(0),
		mpSensorSysFSFreqA15(0),
		mpSensorSysFSFreqMali(0),
		mpFileSysFSUtilMali(0),
		mpFileProcFSUtilARM(0)
		{
		
		init();
	}
	
	CMeasureOdroid::~CMeasureOdroid() {
		destroy();
	}
	
	void CMeasureOdroid::init(void) {
		mrLog()
		<< ">>> 'odroid' (full version)" << std::endl
		<< ">>> 'odroid' (thread main):" << std::endl;
		
		/* Initialize power sensors */
		mpSensorDevFSARM		= initSensorPower(DEVFS_SENSOR_ARM);
		mrLog()
		<< "     sensor power arm a15  eagle      initialized" << std::endl;
		mpSensorDevFSKFC		= initSensorPower(DEVFS_SENSOR_KFC);
		mrLog()
		<< "     sensor power arm a7   kingfisher initialized" << std::endl;
		mpSensorDevFSG3D		= initSensorPower(DEVFS_SENSOR_G3D);
		mrLog()
		<< "     sensor power arm mali t628       initialized" << std::endl;
		mpSensorDevFSMEM		= initSensorPower(DEVFS_SENSOR_MEM);
		mrLog()
		<< "     sensor power mem                 initialized" << std::endl;
		
		/* Initialize temperature sensors */
		mpSensorSysFSTemp		= initSensorTemp(SYSFS_SENSOR_TEMP);
		mrLog()
		<< "     sensor temp  arm a15  eagle      initialized" << std::endl
		<< "     sensor temp  arm mali t628       initialized" << std::endl;
		
		/* Initialize clock sensors */
		mpSensorSysFSFreqA15	= initSensorFreq(SYSFS_SENSOR_FREQ_A15);
		mrLog()
		<< "     sensor clock arm a15  eagle      initialized" << std::endl;
		mpSensorSysFSFreqA7		= initSensorFreq(SYSFS_SENSOR_FREQ_A7);
		mrLog()
		<< "     sensor clock arm a7   kingfisher initialized" << std::endl;
		mpSensorSysFSFreqMali	= initSensorFreq(SYSFS_SENSOR_FREQ_MALI);
		mrLog()
		<< "     sensor clock arm mali t628       initialized" << std::endl;
		
		/* Initialize utilization info files */
		mpFileProcFSUtilARM		= initFileUtilizationProcFS(PROCFS_FILE_UTIL_A15_A7);
		mrLog()
		<< "     file   util  arm a15  eagle      opened" << std::endl
		<< "     file   util  arm a7   kingfisher opened" << std::endl;
		mpFileSysFSUtilMali		= initFileUtilizationSysFS(SYSFS_FILE_UTIL_MALI);
		mrLog()
		<< "     file   util  arm mali t628       opened" << std::endl;
	}
	
	void CMeasureOdroid::destroy(void) {
		destroySensorPower(mpSensorDevFSARM);
		destroySensorPower(mpSensorDevFSKFC);
		destroySensorPower(mpSensorDevFSG3D);
		destroySensorPower(mpSensorDevFSMEM);
		
		destroySensorTemp(mpSensorSysFSTemp);
		
		destroySensorFreq(mpSensorSysFSFreqA15);
		destroySensorFreq(mpSensorSysFSFreqA7);
		destroySensorFreq(mpSensorSysFSFreqMali);
		
		destroyFileUtilizationProcFS(mpFileProcFSUtilARM);
		destroyFileUtilizationSysFS(mpFileSysFSUtilMali);
	}
	
	ODROID_SENSOR *CMeasureOdroid::initSensorPower(const char* pSensorDevFSFileName) {
		ODROID_SENSOR *mpSensorDevFS	= new ODROID_SENSOR;
		mpSensorDevFS->fd				= 0;
		mpSensorDevFS->data.enable		= 0;
		mpSensorDevFS->data.cur_uA		= 0;
		mpSensorDevFS->data.cur_uV		= 0;
		mpSensorDevFS->data.cur_uW		= 0;
		memset(mpSensorDevFS->data.name, 0, INA231_IOCREG_NAME_LENGTH+1);
		
		if ((mpSensorDevFS->fd = open(pSensorDevFSFileName, O_RDWR)) >= 0) {
			mpSensorDevFS->data.enable = 1;
			ioctl(mpSensorDevFS->fd, INA231_IOCSSTATUS, &mpSensorDevFS->data);
		} else {
			mrLog(CLogger::scErr) << "!!! 'odroid' (thread main): Error: cannot open sensor device. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		return mpSensorDevFS;
	}
	
	void CMeasureOdroid::destroySensorPower(ODROID_SENSOR *pSensorDevFSStruct) {
		pSensorDevFSStruct->data.enable	= 0;
		ioctl(pSensorDevFSStruct->fd, INA231_IOCSSTATUS, &pSensorDevFSStruct->data);
		
		delete pSensorDevFSStruct;
	}
	
	double CMeasureOdroid::readSensorPower(ODROID_SENSOR* pSensorDevFSStruct) {
		ioctl(pSensorDevFSStruct->fd, INA231_IOCGREG, &pSensorDevFSStruct->data);
		
		// Devision by 1000.0 to return mW instead of uW.
		return pSensorDevFSStruct->data.cur_uW / 1000.0;
	}
	
	void CMeasureOdroid::measurePower(MS_MEASUREMENT_ODROID* pMsMeasurementOdroid, int32_t& rThreadNum) {
		/* Read power sensor of ARM A15 cores */
		pMsMeasurementOdroid->odroid_power_cur[ODROID_POWER_A15]	= readSensorPower(mpSensorDevFSARM);
		/* Read power sensor of ARM A7 cores */
		pMsMeasurementOdroid->odroid_power_cur[ODROID_POWER_A7]		= readSensorPower(mpSensorDevFSKFC);
		/* Read power sensor of GPU */
		pMsMeasurementOdroid->odroid_power_cur[ODROID_POWER_MALI]	= readSensorPower(mpSensorDevFSG3D);
		/* Read power sensor of Memory */
		pMsMeasurementOdroid->odroid_power_cur[ODROID_POWER_MEM]	= readSensorPower(mpSensorDevFSMEM);
	}
	
	FILE *CMeasureOdroid::initSensorTemp(const char* pSensorSysFSFileName) {
		FILE *sensor_sys_fs_file = fopen(pSensorSysFSFileName, "r");
		if (0 == sensor_sys_fs_file) {
			mrLog(CLogger::scErr) << "!!! 'odroid' (thread main): Error: cannot open sensor file in sys fs. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		return sensor_sys_fs_file;
	}
	
	void CMeasureOdroid::destroySensorTemp(FILE *pSensorSysFSFile) {
		fclose(pSensorSysFSFile);
	}
	
	uint32_t CMeasureOdroid::readSensorTemp(FILE *pSensorSysFSFile, odroid_temp sensorTempType, int32_t& rThreadNum) {
		uint32_t line_length	= 17;
		uint32_t temperature	= 0;
		int status				= 0;
		
		fseek(pSensorSysFSFile, sensorTempType*line_length, SEEK_SET);
		
		status = fscanf(pSensorSysFSFile, "%*s : %u", &temperature);
		if (status != 1) {
			mrLog(CLogger::scErr)
			<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Cannot read temperature (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		temperature /= 1000;
		
		return temperature;
	}
	
	void CMeasureOdroid::measureTemperature(MS_MEASUREMENT_ODROID* pMsMeasurementOdroid, int32_t& rThreadNum) {
		mpSensorSysFSTemp = freopen(SYSFS_SENSOR_TEMP, "r", mpSensorSysFSTemp);
		
		/* Read temperature sensors of ARM A15 cores */
		pMsMeasurementOdroid->odroid_temperature_cur[ODROID_TEMP_A15_CORE_0]	= readSensorTemp(mpSensorSysFSTemp, ODROID_TEMP_A15_CORE_0, rThreadNum);
		pMsMeasurementOdroid->odroid_temperature_cur[ODROID_TEMP_A15_CORE_1]	= readSensorTemp(mpSensorSysFSTemp, ODROID_TEMP_A15_CORE_1, rThreadNum);
		pMsMeasurementOdroid->odroid_temperature_cur[ODROID_TEMP_A15_CORE_2]	= readSensorTemp(mpSensorSysFSTemp, ODROID_TEMP_A15_CORE_2, rThreadNum);
		pMsMeasurementOdroid->odroid_temperature_cur[ODROID_TEMP_A15_CORE_3]	= readSensorTemp(mpSensorSysFSTemp, ODROID_TEMP_A15_CORE_3, rThreadNum);
		/* Read temperature sensor of Mali GPU */
		pMsMeasurementOdroid->odroid_temperature_cur[ODROID_TEMP_MALI]			= readSensorTemp(mpSensorSysFSTemp, ODROID_TEMP_MALI      , rThreadNum);
	}
	
	FILE* CMeasureOdroid::initSensorFreq(const char* pSensorSysFSFileName) {
		return initSensorTemp(pSensorSysFSFileName);
	}
	
	void CMeasureOdroid::destroySensorFreq(FILE* pSensorSysFSFile) {
		destroySensorTemp(pSensorSysFSFile);
	}
	
	uint32_t CMeasureOdroid::readSensorFreq(FILE* pSensorSysFSFile, odroid_freq sensorFreqType, int32_t& rThreadNum) {
		uint32_t frequency	= 0;
		int status			= 0;
		
		fseek(pSensorSysFSFile, 0, SEEK_SET);
		
		status = fscanf(pSensorSysFSFile, "%u", &frequency);
		if (status != 1) {
			mrLog(CLogger::scErr)
			<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Cannot read frequency (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		switch (sensorFreqType) {
			case ODROID_FREQ_A15:
			case ODROID_FREQ_A7:
				frequency /= 1000;
				break;
			case ODROID_FREQ_MALI:
			default:
				break;
		}
		
		return frequency;
	}
	
	void CMeasureOdroid::measureFrequency(MS_MEASUREMENT_ODROID* pMsMeasurementOdroid, int32_t& rThreadNum) {
		mpSensorSysFSFreqA15	= freopen(SYSFS_SENSOR_FREQ_A15 , "r", mpSensorSysFSFreqA15 );
		mpSensorSysFSFreqA7		= freopen(SYSFS_SENSOR_FREQ_A7  , "r", mpSensorSysFSFreqA7  );
		mpSensorSysFSFreqMali	= freopen(SYSFS_SENSOR_FREQ_MALI, "r", mpSensorSysFSFreqMali);
		
		/* Read clock sensor of ARM A15 cores */
		pMsMeasurementOdroid->odroid_clock_cur[ODROID_FREQ_A15]		= readSensorFreq(mpSensorSysFSFreqA15 , ODROID_FREQ_A15 , rThreadNum);
		/* Read clock sensor of ARM A7 cores */
		pMsMeasurementOdroid->odroid_clock_cur[ODROID_FREQ_A7]		= readSensorFreq(mpSensorSysFSFreqA7  , ODROID_FREQ_A7  , rThreadNum);
		/* Read clock sensor of Mali GPU */
		pMsMeasurementOdroid->odroid_clock_cur[ODROID_FREQ_MALI]	= readSensorFreq(mpSensorSysFSFreqMali, ODROID_FREQ_MALI, rThreadNum);
	}
	
	FILE* CMeasureOdroid::initFileUtilizationSysFS(const char* pFileSysFSFileName) {
		return initSensorTemp(pFileSysFSFileName);
	}
	
	FILE* CMeasureOdroid::initFileUtilizationProcFS(const char* pFileProcFSFileName) {
		FILE *file_proc_fs_file = fopen(pFileProcFSFileName, "r");
		if (0 == file_proc_fs_file) {
			mrLog(CLogger::scErr) << "!!! 'odroid' (thread main): Error: cannot open file in proc fs. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		return file_proc_fs_file;
	}
	
	void CMeasureOdroid::destroyFileUtilizationSysFS(FILE* pFileSysFSFileName) {
		destroySensorTemp(pFileSysFSFileName);
	}
	
	void CMeasureOdroid::destroyFileUtilizationProcFS(FILE* pFileProcFSFileName) {
		fclose(pFileProcFSFileName);
	}
	
	void CMeasureOdroid::readFileUtilizationSysFS(FILE* pFileSysFSFileName, odroid_util deviceUtilType, int32_t& rThreadNum, uint64_t *active, uint64_t *inactive) {
		uint64_t utilization	= 0;
		int status				= 0;
		
		fseek(pFileSysFSFileName, 0, SEEK_SET);
		
		status = fscanf(pFileSysFSFileName, "%llu", &utilization);
		if (status != 1) {
			mrLog(CLogger::scErr)
			<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Cannot read utilization (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		switch (deviceUtilType) {
			case ODROID_UTIL_MALI:
				break;
			case ODROID_UTIL_A7:
			case ODROID_UTIL_A15:
			default:
				mrLog(CLogger::scErr)
				<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Wrong utilization type (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
		}
		
		*active		= utilization;
		*inactive	= 100 - utilization;
	}
	
	void CMeasureOdroid::readFileUtilizationProcFS(FILE* pFileProcFSFileName, odroid_util deviceUtilType, int32_t& rThreadNum,
												   uint64_t *active, uint64_t *inactive) {
		int status			= 0;
		char *line			= NULL;
		size_t line_size	= 0;
		ssize_t line_length	= 0;
		
		fseek(pFileProcFSFileName, 0, SEEK_SET);
		
		int first_line		= 0;
		switch (deviceUtilType) {
			case ODROID_UTIL_A15:
				first_line	= 5;
				break;
			case ODROID_UTIL_A7:
				first_line	= 1;
				break;
			case ODROID_UTIL_MALI:
			default:
				mrLog(CLogger::scErr)
				<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Wrong utilization type (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
		}
		
		uint64_t nice, user, system, idle, iowait;
		
		*active		= 0;
		*inactive	= 0;
		
		int i;
		for (i=0; i<9; ++i) {
			line_length = getline(&line, &line_size, pFileProcFSFileName);
			if (line_length <= 0) {
				mrLog(CLogger::scErr)
				<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Cannot read utilization file in proc fs (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
			}
			
			if (i>=first_line && i<first_line+4) {
				status		= sscanf(line, "%*s %llu %llu %llu %llu %llu %*s", &nice, &user, &system, &idle, &iowait);
				if (status != 5) {
					mrLog(CLogger::scErr)
					<< "!!! 'odroid thread' (thread #" << rThreadNum << "): Error: Cannot read utilization (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
					exit(EXIT_FAILURE);
				}
				
				*active		+= nice + user + system;
				*inactive	+= idle + iowait;
			}
		}
		
		free(line);
	}
	
	void CMeasureOdroid::measureUtilization(MS_MEASUREMENT_ODROID* pMsMeasurementOdroid, int32_t& rThreadNum) {
		mpFileProcFSUtilARM	= freopen(PROCFS_FILE_UTIL_A15_A7, "r", mpFileProcFSUtilARM);
		mpFileSysFSUtilMali	= freopen(SYSFS_FILE_UTIL_MALI   , "r", mpFileSysFSUtilMali);
		
		/* Read utilization of Mali GPU */
		readFileUtilizationSysFS(mpFileSysFSUtilMali, ODROID_UTIL_MALI, rThreadNum,
								 &(pMsMeasurementOdroid->internal.odroid_util_cur[ODROID_UTIL_MALI][ODROID_UTIL_ACTIVE]),
								 &(pMsMeasurementOdroid->internal.odroid_util_cur[ODROID_UTIL_MALI][ODROID_UTIL_IDLE]));
		
		/* Read utilization of ARM Cortex A7/A15 CPU */
		readFileUtilizationProcFS(mpFileProcFSUtilARM, ODROID_UTIL_A15, rThreadNum,
								  &(pMsMeasurementOdroid->internal.odroid_util_cur[ODROID_UTIL_A15][ODROID_UTIL_ACTIVE]),
								  &(pMsMeasurementOdroid->internal.odroid_util_cur[ODROID_UTIL_A15][ODROID_UTIL_IDLE]));
		readFileUtilizationProcFS(mpFileProcFSUtilARM, ODROID_UTIL_A7 , rThreadNum,
								  &(pMsMeasurementOdroid->internal.odroid_util_cur[ODROID_UTIL_A7][ODROID_UTIL_ACTIVE]),
								  &(pMsMeasurementOdroid->internal.odroid_util_cur[ODROID_UTIL_A7][ODROID_UTIL_IDLE]));
	}
	
	void CMeasureOdroid::measure(void* pMsMeasurement, int32_t& rThreadNum) {
		MS_MEASUREMENT_ODROID *pMsMeasurementOdroid = (MS_MEASUREMENT_ODROID *)pMsMeasurement;
		
		measurePower(pMsMeasurementOdroid, rThreadNum);
		measureTemperature(pMsMeasurementOdroid, rThreadNum);
		measureFrequency(pMsMeasurementOdroid, rThreadNum);
		measureUtilization(pMsMeasurementOdroid, rThreadNum);
	}
	
	int CMeasureOdroid::getVariant() {
		return VARIANT_FULL;
	}
}
