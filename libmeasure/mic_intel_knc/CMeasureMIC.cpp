/*
 * CMeasureMIC.cpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 4/2/15
 * version: 0.4.0 - MIC integration into libmeasure
 *          0.5.0 - add cpu, gpu and mic memory information
 *          0.5.3 - add abstract measure and abstract measure thread
 *          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                   and to select between the full or light library.
 *          0.7.0 - modularised measurement struct 
 */

namespace NLibMeasure {
	template <int SkipMs, int Variant>
	CMeasureMIC<SkipMs, Variant>::CMeasureMIC(CLogger& rLogger):
		CMeasureAbstractResource(rLogger),
		mpMicDevice(0),
		mpMicUtilization(0)
		{
		
		init();
	}
	
	template <int SkipMs, int Variant>
	CMeasureMIC<SkipMs, Variant>::~CMeasureMIC() {
		destroy();
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::init(void) {	
		if(Variant == FULL) {
			mrLog()
			<< ">>> 'mic' (full version)" << std::endl;
		} else {
			mrLog()
			<< ">>> 'mic' (light version)" << std::endl;
		}
		
		int32_t status							= E_MIC_SUCCESS;
		int32_t num_of_mic_devices				= 0;
		int32_t sysfs_num_mic_device			= -1;
		uint32_t device_type					= 0;
		uint16_t pcie_domain_number				= 0;
		uint16_t pcie_bus_number				= 0;
		uint16_t pcie_device_number				= 0;
		uint16_t pcie_func_number				= 0; // TODO: Currently, it's a static value!
		uint16_t num_of_cores					= 0;
		uint16_t threads_per_core				= 0;
		uint32_t memory_size					= 0;
		size_t size_string						= 255;
		
		char uos[size_string+1];
		char flash[size_string+1];
		char board_sku[size_string+1];
		char revision[size_string+1];
		char firmware[size_string+1];
		char boot_loader[size_string+1];
		char memory_type[size_string+1];
		
		struct mic_devices_list *mic_devices	= NULL;
		struct mic_pci_config *pcie				= NULL;
		struct mic_version_info *version		= NULL;
		struct mic_thermal_info *thermal		= NULL;
		struct mic_device_mem *memory_info		= NULL;
		
		memset(uos, 0, size_string+1);
		memset(flash, 0, size_string+1);
		memset(board_sku, 0, size_string+1);
		memset(revision, 0, size_string+1);
		memset(firmware, 0, size_string+1);
		memset(boot_loader, 0, size_string+1);
		memset(memory_type, 0, size_string+1);
		
		// Get a list of all MIC devices.
		status = mic_get_devices(&mic_devices);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC device list. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get number of MIC devices.
		status = mic_get_ndevices(mic_devices, &num_of_mic_devices);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain number of MIC devices. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	
		if (num_of_mic_devices == 0) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: could not find any Xeon Phi cards. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		} else if (num_of_mic_devices > 1) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: this software has to be rewritten if you want to support more than 1 device. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get sysfs device number of first MIC device in sysfs.
		status = mic_get_device_at_index(mic_devices, 0, &sysfs_num_mic_device);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain Linux sysfs MIC device number. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Open MIC device according to sysfs MIC device number.
		status = mic_open_device(&mpMicDevice, (uint32_t)sysfs_num_mic_device);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot open MIC device. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get MIC device type.
		status = mic_get_device_type(mpMicDevice, &device_type);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC device type. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		if (device_type != KNC_ID) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: Unknown MIC device type. Currently, libmeasure does only support KNC ('Knights Corner') MIC devices. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Free the device list.
		status = mic_free_devices(mic_devices);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot free MIC device list. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog()
		<< ">>> 'mic' (thread main):" << std::endl
		<< "     sysfs index  : /sys/class/mic/mic" << sysfs_num_mic_device << std::endl
		<< "     device       : Knights Corner (KNC)" << std::endl;
		
		// Allocate MIC utilization struct.
		status = mic_alloc_core_util(&mpMicUtilization);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot allocate MIC utilization structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Obtain MIC's PCIe configuration.
		status = mic_get_pci_config(mpMicDevice, &pcie);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC PCIe configuration. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get PCIe domain number.
		status = mic_get_pci_domain_id(pcie, &pcie_domain_number);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC PCIe domain number. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get PCIe bus number.
		status = mic_get_bus_number(pcie, &pcie_bus_number);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC PCIe bus number. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get PCIe device number.
		status = mic_get_device_number(pcie, &pcie_device_number);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC PCIe device number. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Free MIC's PCIe configuration struct.
		status = mic_free_pci_config(pcie);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot free MIC PCIe configuration struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get MIC version information.
		status = mic_get_version_info(mpMicDevice, &version);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC version information. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Obtain OS version running on MIC.
		status = mic_get_uos_version(version, uos, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain uOS version. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get MIC flash version.
		status = mic_get_flash_version(version, flash, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain flash version. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
			
		// Free version info struct.
		status = mic_free_version_info(version);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot free MIC version information struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		
		// Obtain stock keeping unit (product id) of Xeon Phi board.
		status = mic_get_silicon_sku(mpMicDevice, board_sku, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC SKU. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Returns SMC (System Mangement Controller) info as well as thermal info.
		status = mic_get_thermal_info(mpMicDevice, &thermal);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC SMC and thermal information. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get HW revision ID string.
		status = mic_get_smc_hwrevision(thermal, revision, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC hardware revision string. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get firmware version.
		status = mic_get_smc_fwversion(thermal, firmware, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC firmware version. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get boot loader version off MIC.
		status = mic_get_smc_boot_loader_ver(thermal, boot_loader, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC boot loader version. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Free SKU/thermal info struct.
		status = mic_free_thermal_info(thermal);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot free SKU and thermal information. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Update core utilization struct.
		status = mic_update_core_util(mpMicDevice, mpMicUtilization);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot update MIC utilization struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get number of cores on MIC.
		status = mic_get_num_cores(mpMicUtilization, &num_of_cores);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain number of core on MIC die. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get number of threads per core on MIC.
		status = mic_get_threads_core(mpMicUtilization, &threads_per_core);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain number of HW threds per core on MIC die. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get information about the MIC memory.
		status = mic_get_memory_info(mpMicDevice, &memory_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC memory information structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get MIC memory type.
		status = mic_get_memory_type(memory_info, memory_type, &size_string);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC memory type. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get total size of MIC memory.
		status = mic_get_memory_size(memory_info, &memory_size);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot obtain MIC total memory size. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Free struct storing the MIC memory info.
		status = mic_free_memory_info(memory_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot free MIC memory information structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mrLog()
		<< ">>> 'mic' (thread main):" << std::endl
		<< "     pcie         : " << std::setfill('0') << std::hex << std::setw(4) << pcie_domain_number << ":" << std::setw(2) << pcie_bus_number << ":" << std::setw(2) << pcie_device_number << "." << pcie_func_number << std::dec << std::endl
		<< "     mic os       : Linux kernel " << uos <<  std::endl
		<< "     flash        : v" << flash << std::endl
		<< "     board sku	  : " << board_sku << std::endl
		<< "     revision     : " << revision << std::endl
		<< "     firmware     : v" << firmware << std::endl
		<< "     boot loader  : v" << boot_loader << std::endl
		<< "     cores/board  : " << num_of_cores << std::endl
		<< "     threads/core : "  << threads_per_core << std::endl
		<< "     threads total: "  << num_of_cores*threads_per_core << std::endl
		<< "     memory type  : " << memory_type << std::endl
		<< "     memory size  : " <<  memory_size/1024 << " MiB" << std::endl;
		
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::destroy(void) {
		int32_t status	= E_MIC_SUCCESS;
		
		// Close MIC device.
		status = mic_close_device(mpMicDevice);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot close MIC device. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Free MIC utilization struct.
		status = mic_free_core_util(mpMicUtilization);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread main): Error: cannot free MIC utilization structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		mpMicDevice			= NULL;
		mpMicUtilization	= NULL;
		
	}
	
	template <int SkipMs, int Variant>
	int CMeasureMIC<SkipMs, Variant>::getVariant() {
		return Variant;
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::read_memory_total(void *pMsMeasurement, int32_t& rThreadNum) {
		MS_MEASUREMENT_MIC *pMsMeasurementMic = (MS_MEASUREMENT_MIC *) pMsMeasurement;
		int32_t status								= E_MIC_SUCCESS;
		struct mic_memory_util_info *memory_info	= NULL;
				
		// Get information about the MIC memory utilization.
		status = mic_get_memory_utilization_info(mpMicDevice, &memory_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC memory utilization structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get total memory size according to MIC OS.
		status = mic_get_total_memory_size(memory_info, &(pMsMeasurementMic->mic_memory_total));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC total memory size. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
				
		// Free information struct about the MIC memory utilization.
		status = mic_free_memory_utilization_info(memory_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot free MIC memory utilization structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::measure(void* pMsMeasurement, int32_t& rThreadNum) {
		MS_MEASUREMENT_MIC *pMsMeasurementMic = (MS_MEASUREMENT_MIC *) pMsMeasurement;
		micGetPower(pMsMeasurementMic, rThreadNum);
		micGetUtil(pMsMeasurementMic, rThreadNum);
		
		if(Variant == FULL) {
			micGetFrequency(pMsMeasurementMic, rThreadNum);
			if(!(mMeasureCounter++ % SkipMs)) {
				micGetMemory(pMsMeasurementMic, rThreadNum);
				micGetTemperature(pMsMeasurementMic, rThreadNum);
			}
		}
		
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::micGetFrequency(MS_MEASUREMENT_MIC *pMsMeasurementMic, int32_t& rThreadNum) {
		int32_t status						= E_MIC_SUCCESS;
		uint32_t temp						= 0;
		struct mic_cores_info *cores_info	= NULL;
		struct mic_device_mem *memory_info	= NULL;
		
		// Get information about the MIC on-die cores.
		status = mic_get_cores_info(mpMicDevice, &cores_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC core information structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get current frequency of the MIC cores.
		status = mic_get_cores_frequency(cores_info, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC core frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_freq_core_cur = temp / 1000;
		
		// Free struct storing the MIC on-die core info.
		status = mic_free_cores_info(cores_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot free MIC core information structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		if(!(mMeasureCounter % SkipMs)) {
			// Get information about the MIC memory.
			status = mic_get_memory_info(mpMicDevice, &memory_info);
			if (E_MIC_SUCCESS != status) {
				mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC memory information structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
			}
			
			// Get current frequency of the MIC memory.
			status = mic_get_memory_frequency(memory_info, &temp);
			if (E_MIC_SUCCESS != status) {
				mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC memory frequency. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
			}
			pMsMeasurementMic->mic_freq_mem_cur = temp / 1000;
			
			// Free struct storing the MIC memory info.
			status = mic_free_memory_info(memory_info);
			if (E_MIC_SUCCESS != status) {
				mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot free MIC memory information structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::micGetPower(MS_MEASUREMENT_MIC *pMsMeasurementMic, int32_t& rThreadNum) {
		int32_t status						= E_MIC_SUCCESS;
		uint32_t temp						= 0;
		struct mic_power_util_info *power	= NULL;
		
		// Returns power info.
		status = mic_get_power_utilization_info(mpMicDevice, &power);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC power information. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get power consumed via PCIe slot (max. 75W).
		status = mic_get_pcie_power_readings(power, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC power consumption via PCIe slot. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_power_cur[MIC_PCIE] = temp / 1000;
				
		// Get power consumed 2x3 (max. 75W) power connector.
		status = mic_get_c2x3_power_readings(power,  &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC power consumption via 2x3 power connector. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_power_cur[MIC_C2X3] = temp / 1000;
		
		// Get power consumed 2x4 (max. 100W) power connector.
		status = mic_get_c2x4_power_readings(power, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC power consumption via 2x4 power connector. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_power_cur[MIC_C2X4] = temp / 1000;
		
		// Get core power rail power consumption.
		status = mic_get_vccp_power_readings(power, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC core power rail power consumption. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_power_cur[MIC_VCCP] = temp / 1000;
		
		// Get uncore power rail power consumption.
		status = mic_get_vddg_power_readings(power, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC uncore power rail power consumption. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_power_cur[MIC_VDDG] = temp / 1000;
		
		// Get memory subsystem power rail power consumption.
		status = mic_get_vddq_power_readings(power, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC memory subsystem power rail power consumption. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_power_cur[MIC_VDDQ] = temp / 1000;
		
		// Calculate total power consumption
		pMsMeasurementMic->mic_power_cur[MIC_POWER] = pMsMeasurementMic->mic_power_cur[MIC_PCIE] + pMsMeasurementMic->mic_power_cur[MIC_C2X3] + pMsMeasurementMic->mic_power_cur[MIC_C2X4];
		
		// Free power info struct.
		status = mic_free_power_utilization_info(power);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot free power information struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::micGetTemperature(MS_MEASUREMENT_MIC *pMsMeasurementMic, int32_t& rThreadNum) {
		int32_t status						= E_MIC_SUCCESS;
		struct mic_thermal_info *thermal	= NULL;
		uint16_t temp						= 0;
		
		// Returns thermal info.
		status = mic_get_thermal_info(mpMicDevice, &thermal);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC thermal information. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get die temperature.
		status = mic_get_die_temp(thermal, &(pMsMeasurementMic->mic_temperature_cur[MIC_DIE_TEMP]));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC die temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get GDDR RAM temperature.
		status = mic_get_gddr_temp(thermal, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC GDDR RAM temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_temperature_cur[MIC_GDDR_TEMP] = temp;
		
		// Get fan-in temperature.
		status = mic_get_fanin_temp(thermal, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC fan-in temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_temperature_cur[MIC_FAN_IN_TEMP] = temp;
		
		// Get fan-out temperature.
		status = mic_get_fanout_temp(thermal, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC fan-out temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_temperature_cur[MIC_FAN_OUT_TEMP] = temp;
		
		// Get core power rail (vccp) temperature.
		status = mic_get_vccp_temp(thermal, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC core power rail temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_temperature_cur[MIC_VCCP_TEMP] = temp;
		
		// Get uncore power rail (vddg) temperature.
		status = mic_get_vddg_temp(thermal, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC uncore power rail temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_temperature_cur[MIC_VDDG_TEMP] = temp;
		
		// Get memory subsystem power rail (vddq) temperature.
		status = mic_get_vddq_temp(thermal, &temp);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC memory power rail temperature. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		pMsMeasurementMic->mic_temperature_cur[MIC_VDDQ_TEMP] = temp;
		
		// Free thermal info struct.
		status = mic_free_thermal_info(thermal);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot free thermal information struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	}	
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::micGetUtil(MS_MEASUREMENT_MIC *pMsMeasurementMic, int32_t& rThreadNum) {  
		int32_t status					= E_MIC_SUCCESS;
		
		// Update core utilization struct.
		status = mic_update_core_util(mpMicDevice, mpMicUtilization);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot update MIC utilization struct. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get sum of idle time (in jiffies) of all cores.
		status = mic_get_idle_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[IDLE_SUM]));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain sum of idle time of all cores. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get sum of system time (in jiffies) of all cores.
		status = mic_get_sys_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[SYS_SUM]));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain sum of system time of all cores. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get sum of nice time (in jiffies) of all cores.
		status = mic_get_nice_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[NICE_SUM]));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain sum of nice time of all cores. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get sum of user time (in jiffies) of all cores.
		status = mic_get_user_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[USER_SUM]));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain sum of user time of all cores. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	template <int SkipMs, int Variant>
	void CMeasureMIC<SkipMs, Variant>::micGetMemory(MS_MEASUREMENT_MIC *pMsMeasurementMic, int32_t& rThreadNum) {
		int32_t status								= E_MIC_SUCCESS;
		struct mic_memory_util_info *memory_info	= NULL;
		
		// Get information about the MIC memory utilization.
		status = mic_get_memory_utilization_info(mpMicDevice, &memory_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain MIC memory utilization structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		// Get size of free memory according to MIC OS.
		status = mic_get_available_memory_size(memory_info, &(pMsMeasurementMic->mic_memory_free_cur));
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot obtain size of free MIC memory. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		pMsMeasurementMic->mic_memory_used_cur = pMsMeasurementMic->mic_memory_total - pMsMeasurementMic->mic_memory_free_cur;
		
		// Free information struct about the MIC memory utilization.
		status = mic_free_memory_utilization_info(memory_info);
		if (E_MIC_SUCCESS != status) {
			mrLog(CLogger::scErr) << "!!! 'mic' (thread #" << rThreadNum << "): Error: cannot free MIC memory utilization structure. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

}
