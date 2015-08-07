/*
 * mic.c
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
 * created: 3/18/15
 * version: 0.3.4 - add a tool for first intergration tests of mic
 */

#include "mic.h"

#include <unistd.h>

#include <miclib.h>

/*
 * FUNCTION DECLARATIONS (PROTOTYPES)
 */
static void mic_init(struct mic_device **mic_device, struct mic_core_util **mic_utilization);
static void mic_fini(struct mic_device **mic_device, struct mic_core_util **mic_utilization);

static void mic_info(struct mic_device *mic_device, struct mic_core_util *mic_utilization);

static void mic_read(struct mic_device *mic_device, struct mic_core_util *mic_utilization);
static void mic_read_power(struct mic_device *mic_device);
static void mic_read_temp(struct mic_device *mic_device);
static void mic_read_freq(struct mic_device *mic_device);
static void mic_read_util(struct mic_device *mic_device, struct mic_core_util *mic_utilization);
static void mic_read_mem(struct mic_device *mic_device);

/*
 * FUNCTION DEFINITIONS
 */
static void mic_init(struct mic_device **mic_device, struct mic_core_util **mic_utilization) {
	int32_t status							= E_MIC_SUCCESS;
	struct mic_devices_list *mic_devices	= NULL;
	int32_t num_of_mic_devices				= 0;
	int32_t sysfs_num_mic_device			= -1;
	uint32_t device_type					= 0;
	
	*mic_device			= NULL;
	*mic_utilization	= NULL;
	
	// Get a list of all MIC devices.
	status = mic_get_devices(&mic_devices);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC device list.");
	
	// Get number of MIC devices.
	status = mic_get_ndevices(mic_devices, &num_of_mic_devices);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain number of MIC devices.");
	
	if (num_of_mic_devices == 0) {
		LOG_ERROR("Could not find any Xeon Phi cards. Please compile libmeasure without MIC support.");
		exit(EXIT_FAILURE);
	} else if (num_of_mic_devices > 1) {
		LOG_WARN("Detect more than one MIC device. Note that only one device will be considered in measurements.");
	}
	
	// Get sysfs device number of first MIC device in sysfs.
	status = mic_get_device_at_index(mic_devices, 0, &sysfs_num_mic_device);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain Linux sysfs MIC device number.");
	
	// Open MIC device according to sysfs MIC device number.
	status = mic_open_device(mic_device, (uint32_t)sysfs_num_mic_device);
	CHECK_MIC_STATUS_ERROR(status, "Cannot open MIC device.");
	
	// Get MIC device type.
	status = mic_get_device_type(*mic_device, &device_type);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC device type.");
	
	if (device_type != KNC_ID) {
		LOG_ERROR("Unknown MIC device type. Currently, libmeasure does only support KNC ('Knights Corner') MIC devices.");
		exit(EXIT_FAILURE);
	}
	
	// Free the device list.
	status = mic_free_devices(mic_devices);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC device list.");
	
	fprintf(stdout,
		">>> 'mic' (thread main):\n"
		"     sysfs index  : /sys/class/mic/mic%i\n"
		"     device       : Knights Corner (KNC)\n",
		sysfs_num_mic_device
	);
	
	// Allocate MIC utilization struct.
	status = mic_alloc_core_util(mic_utilization);
	CHECK_MIC_STATUS_ERROR(status, "Cannot allocate MIC utilization structure.");
}

static void mic_fini(struct mic_device **mic_device, struct mic_core_util **mic_utilization) {
	int32_t status	= E_MIC_SUCCESS;
	
	// Close MIC device.
	status = mic_close_device(*mic_device);
	CHECK_MIC_STATUS_WARN(status, "Cannot close MIC device.");
	
	// Free MIC utilization struct.
	status = mic_free_core_util(*mic_utilization);
	CHECK_MIC_STATUS_ERROR(status, "Cannot free MIC utilization structure.");
	
	*mic_device			= NULL;
	*mic_utilization	= NULL;
	
	fprintf(stdout,
		"<<< 'mic' (thread main)\n"
	);
}

static void mic_info(struct mic_device *mic_device, struct mic_core_util *mic_utilization) {
	int32_t status						= E_MIC_SUCCESS;
	
	struct mic_pci_config *pcie			= NULL;
	uint16_t pcie_domain_number			= 0; // TODO: Currently, it's a static value!
	uint16_t pcie_bus_number			= 0;
	uint16_t pcie_device_number			= 0;
	uint16_t pcie_func_number			= 0; // TODO: Currently, it's a static value!
	
	struct mic_version_info *version	= NULL;
	size_t size_string					= 255;
	char uos[size_string+1];
	memset(uos, 0, size_string+1);
	char flash[size_string+1];
	memset(flash, 0, size_string+1);
	
	char board_sku[size_string+1];
	memset(board_sku, 0, size_string+1);
	
	struct mic_thermal_info *thermal	= NULL;
	char revision[size_string+1];
	memset(revision, 0, size_string+1);
	char firmware[size_string+1];
	memset(firmware, 0, size_string+1);
	char boot_loader[size_string+1];
	memset(boot_loader, 0, size_string+1);
	
	uint16_t num_of_cores				= 0;
	uint16_t threads_per_core			= 0;
	
	struct mic_device_mem *memory_info	= NULL;
	uint32_t memory_size				= 0;
	char memory_type[size_string+1];
	memset(memory_type, 0, size_string+1);
	
	// Obtain MIC's PCIe configuration.
	status = mic_get_pci_config(mic_device, &pcie);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC PCIe configuration.");
	
	// Get PCIe bus number.
	status = mic_get_bus_number(pcie, &pcie_bus_number);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC PCIe bus number.");
	
	// Get PCIe device number.
	status = mic_get_device_number(pcie, &pcie_device_number);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC PCIe device number.");
	
	fprintf(stdout,
		"     pcie         : %04X:%02X:%02X.%01X\n",
		pcie_domain_number, pcie_bus_number, pcie_device_number, pcie_func_number
	);
	
	// Free MIC's PCIe configuration struct.
	status = mic_free_pci_config(pcie);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC PCIe configuration struct.");
	
	// Get MIC version information.
	status = mic_get_version_info(mic_device, &version);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC version information.");
	
	// Obtain OS version running on MIC.
	status = mic_get_uos_version(version, uos, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain uOS version.");
	
	// Get MIC flash version.
	status = mic_get_flash_version(version, flash, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain flash version.");
	
	fprintf(stdout,
		"     mic os       : Linux kernel %s\n"
		"     flash        : v%s\n",
		uos, flash
	);
	
	// Free version info struct.
	status = mic_free_version_info(version);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC version information struct.");
	
	// Obtain stock keeping unit (product id) of Xeon Phi board.
	status = mic_get_silicon_sku(mic_device, board_sku, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC SKU.");
	
	fprintf(stdout,
		"     board sku    : %s\n",
		board_sku
	);
	
	// Returns SMC (System Mangement Controller) info as well as thermal info.
	status = mic_get_thermal_info(mic_device, &thermal);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC SMC and thermal information.");
	
	// Get HW revision ID string.
	status = mic_get_smc_hwrevision(thermal, revision, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC hardware revision string.");
	
	// Get firmware version.
	status = mic_get_smc_fwversion(thermal, firmware, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC firmware version.");
	
	// Get boot loader version off MIC.
	status = mic_get_smc_boot_loader_ver(thermal, boot_loader, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC boot loader version.");
	
	fprintf(stdout,
		"     revision     : %s\n"
		"     firmware     : v%s\n"
		"     boot loader  : v%s\n",
		revision, firmware, boot_loader
	);
	
	// Free SKU/thermal info struct.
	status = mic_free_thermal_info(thermal);
	CHECK_MIC_STATUS_WARN(status, "Cannot free SKU and thermal information.");
	
	// Update core utilization struct.
	status = mic_update_core_util(mic_device, mic_utilization);
	CHECK_MIC_STATUS_ERROR(status, "Cannot update MIC utilization struct.");
	
	// Get number of cores on MIC.
	status = mic_get_num_cores(mic_utilization, &num_of_cores);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain number of core on MIC die.");
	
	// Get number of threads per core on MIC.
	status = mic_get_threads_core(mic_utilization, &threads_per_core);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain number of HW threds per core on MIC die.");
	
	fprintf(stdout,
		"     cores/board  : %u\n"
		"     threads/core : %u\n"
		"     threads total: %u\n",
		num_of_cores, threads_per_core, num_of_cores*threads_per_core
	);
	
	// Get information about the MIC memory.
	status = mic_get_memory_info(mic_device, &memory_info);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory information structure.");
	
	// Get MIC memory type.
	status = mic_get_memory_type(memory_info, memory_type, &size_string);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory type.");
	
	// Get total size of MIC memory.
	status = mic_get_memory_size(memory_info, &memory_size);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC total memory size.");
	
	fprintf(stdout,
		"     memory type  : %s\n"
		"     memory size  : %u MiB\n",
		memory_type, memory_size/1024
	);
	
	// Free struct storing the MIC memory info.
	status = mic_free_memory_info(memory_info);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC memory information structure.");
}

static void mic_read(struct mic_device *mic_device, struct mic_core_util *mic_utilization) {
	mic_read_power(mic_device);
	mic_read_temp(mic_device);
	mic_read_freq(mic_device);
	mic_read_util(mic_device, mic_utilization);
	mic_read_mem(mic_device);
}

static void mic_read_power(struct mic_device *mic_device) {
	int32_t status						= E_MIC_SUCCESS;
	struct mic_power_util_info *power	= NULL;
	uint32_t power_w0					= 0;
	uint32_t power_w1					= 0;
	uint32_t power_pcie					= 0;
	uint32_t power_c2x3					= 0;
	uint32_t power_c2x4					= 0;
	uint32_t power_vccp					= 0;
	uint32_t power_vddg					= 0;
	uint32_t power_vddq					= 0;
	
	// Returns power info.
	status = mic_get_power_utilization_info(mic_device, &power);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC power information.");
	
	// Get power for time window 0.
	status = mic_get_total_power_readings_w0(power, &power_w0);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC power for time window 0.");
	
	// Get power for time window 1.
	status = mic_get_total_power_readings_w1(power, &power_w1);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC power for time window 1.");
	
	// Get power consumed via PCIe slot (max. 75W).
	status = mic_get_pcie_power_readings(power, &power_pcie);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC power consumption via PCIe slot.");
	
	// Get power consumed 2x3 (max. 75W) power connector.
	status = mic_get_c2x3_power_readings(power, &power_c2x3);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC power consumption via 2x3 power connector.");
	
	// Get power consumed 2x4 (max. 100W) power connector.
	status = mic_get_c2x4_power_readings(power, &power_c2x4);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC power consumption via 2x4 power connector.");
	
	// Get core power rail power consumption.
	status = mic_get_vccp_power_readings(power, &power_vccp);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC core power rail power consumption.");
	
	// Get uncore power rail power consumption.
	status = mic_get_vddg_power_readings(power, &power_vddg);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC uncore power rail power consumption.");
	
	// Get memory subsystem power rail power consumption.
	status = mic_get_vddq_power_readings(power, &power_vddq);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory subsystem power rail power consumption.");
	
	fprintf(stdout,
		":::> power total time window 0      [ W ]: %u\n"
		":::> power total time window 1      [ W ]: %u\n"
		":::> power pcie                     [ W ]: %u\n"
		":::> power 2x3 power connector      [ W ]: %u\n"
		":::> power 2x4 power connector      [ W ]: %u\n"
		":::> power core   power rail (vccp) [ W ]: %u\n"
		":::> power uncore power rail (vddg) [ W ]: %u\n"
		":::> power memory power rail (vddq) [ W ]: %u\n"
		":::> power total board              [ W ]: %u\n",
		power_w0/1000000, power_w1/1000000,
		power_pcie/1000000, power_c2x3/1000000, power_c2x4/1000000,
		power_vccp/1000000, power_vddg/1000000, power_vddq/1000000,
		(power_pcie+power_c2x3+power_c2x4)/1000000
	);
	
	// Free power info struct.
	status = mic_free_power_utilization_info(power);
	CHECK_MIC_STATUS_WARN(status, "Cannot free power information struct.");
}

static void mic_read_temp(struct mic_device *mic_device) {
	int32_t status						= E_MIC_SUCCESS;
	struct mic_thermal_info *thermal	= NULL;
	uint32_t temp_die					= 0;
	uint16_t temp_gddr					= 0;
	uint16_t temp_fan_in				= 0;
	uint16_t temp_fan_out				= 0;
	uint16_t temp_vccp					= 0;
	uint16_t temp_vddg					= 0;
	uint16_t temp_vddq					= 0;
	
	// Returns thermal info.
	status = mic_get_thermal_info(mic_device, &thermal);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC thermal information.");
	
	// Get die temperature.
	status = mic_get_die_temp(thermal, &temp_die);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC die temperature.");
	
	// Get GDDR RAM temperature.
	status = mic_get_gddr_temp(thermal, &temp_gddr);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC GDDR RAM temperature.");
	
	// Get fan-in temperature.
	status = mic_get_fanin_temp(thermal, &temp_fan_in);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC fan-in temperature.");
	
	// Get fan-out temperature.
	status = mic_get_fanout_temp(thermal, &temp_fan_out);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC fan-out temperature.");
	
	// Get core power rail (vccp) temperature.
	status = mic_get_vccp_temp(thermal, &temp_vccp);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC core power rail temperature.");
	
	// Get uncore power rail (vddg) temperature.
	status = mic_get_vddg_temp(thermal, &temp_vddg);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC uncore power rail temperature.");
	
	// Get memory subsystem power rail (vddq) temperature.
	status = mic_get_vddq_temp(thermal, &temp_vddq);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory power rail temperature.");
	
	fprintf(stdout,
		":::> temp  die                      [\u00b0C ]: %u\n"
		":::> temp  gddr                     [\u00b0C ]: %u\n"
		":::> temp  fan-in                   [\u00b0C ]: %u\n"
		":::> temp  fan-out                  [\u00b0C ]: %u\n"
		":::> temp  core   power rail (vccp) [\u00b0C ]: %u\n"
		":::> temp  uncore power rail (vddg) [\u00b0C ]: %u\n"
		":::> temp  memory power rail (vddq) [\u00b0C ]: %u\n",
		temp_die, temp_gddr, temp_fan_in, temp_fan_out, temp_vccp, temp_vddg, temp_vddq
	);
	
	// Free thermal info struct.
	status = mic_free_thermal_info(thermal);
	CHECK_MIC_STATUS_WARN(status, "Cannot free thermal information struct.");
}

static void mic_read_freq(struct mic_device *mic_device) {
	int32_t status						= E_MIC_SUCCESS;
	struct mic_cores_info *cores_info	= NULL;
	uint32_t cores_freq					= 0;
	struct mic_device_mem *memory_info	= NULL;
	uint32_t memory_freq				= 0;
	
	// Get information about the MIC on-die cores.
	status = mic_get_cores_info(mic_device, &cores_info);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC core information structure.");
	
	// Get current frequency of the MIC cores.
	status = mic_get_cores_frequency(cores_info, &cores_freq);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC core frequency.");
	
	fprintf(stdout,
		":::> freq  cores                    [MHz]: %u\n",
		cores_freq/1000
	);
	
	// Free struct storing the MIC on-die core info.
	status = mic_free_cores_info(cores_info);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC core information structure.");
	
	// Get information about the MIC memory.
	status = mic_get_memory_info(mic_device, &memory_info);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory information structure.");
	
	// Get current frequency of the MIC memory.
	status = mic_get_memory_frequency(memory_info, &memory_freq);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory frequency.");
	
	fprintf(stdout,
		":::> freq  memory                   [MHz]: %u\n",
		memory_freq/1000
	);
	
	// Free struct storing the MIC memory info.
	status = mic_free_memory_info(memory_info);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC memory information structure.");
}

static void mic_read_util(struct mic_device *mic_device, struct mic_core_util *mic_utilization) {
	int32_t status					= E_MIC_SUCCESS;
	uint64_t idle_sum				= 0;
	uint64_t sys_sum				= 0;
	uint64_t nice_sum				= 0;
	uint64_t user_sum				= 0;
	double util_since_mic_os_boot	= 0.0;
	
	// Update core utilization struct.
	status = mic_update_core_util(mic_device, mic_utilization);
	CHECK_MIC_STATUS_ERROR(status, "Cannot update MIC utilization struct.");
	
	// Get sum of idle time (in jiffies) of all cores.
	status = mic_get_idle_sum(mic_utilization, &idle_sum);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain sum of idle time of all cores.");
	
	// Get sum of system time (in jiffies) of all cores.
	status = mic_get_sys_sum(mic_utilization, &sys_sum);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain sum of system time of all cores.");
	
	// Get sum of nice time (in jiffies) of all cores.
	status = mic_get_nice_sum(mic_utilization, &nice_sum);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain sum of nice time of all cores.");
	
	// Get sum of user time (in jiffies) of all cores.
	status = mic_get_user_sum(mic_utilization, &user_sum);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain sum of user time of all cores.");
	
	util_since_mic_os_boot = (100.0*(sys_sum+nice_sum+user_sum)/(idle_sum+sys_sum+nice_sum+user_sum));
	
	fprintf(stdout,
		":::> util  time idle sum all cores  [jif]: %lu\n"
		":::> util  time sys  sum all cores  [jif]: %lu\n"
		":::> util  time nice sum all cores  [jif]: %lu\n"
		":::> util  time user sum all cores  [jif]: %lu\n"
		":::> util  since mic os boot        [ %% ]: %lf\n",
		idle_sum, sys_sum, nice_sum, user_sum, util_since_mic_os_boot
	);
}

static void mic_read_mem(struct mic_device *mic_device) {
	int32_t status								= E_MIC_SUCCESS;
	struct mic_memory_util_info *memory_info	= NULL;
	uint32_t memory_free						= 0;
	uint32_t memory_total						= 0;
	uint32_t memory_alloc						= 0;
	double memory_alloc_relative				= 0.0;
	
	// Get information about the MIC memory utilization.
	status = mic_get_memory_utilization_info(mic_device, &memory_info);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC memory utilization structure.");
	
	// Get total memory size according to MIC OS.
	status = mic_get_total_memory_size(memory_info, &memory_total);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain MIC total memory size.");
	
	// Get size of free memory according to MIC OS.
	status = mic_get_available_memory_size(memory_info, &memory_free);
	CHECK_MIC_STATUS_ERROR(status, "Cannot obtain size of free MIC memory.");
	
	memory_alloc			= memory_total - memory_free;
	memory_alloc_relative	= 100.0*memory_alloc/memory_total;
	
	fprintf(stdout,
		":::> mem   total     (acc. mic os)  [MiB]: %u\n"
		":::> mem   free      (acc. mic os)  [MiB]: %u\n"
		":::> mem   allocated (acc. mic os)  [MiB]: %u\n"
		":::> mem   utilized  (acc. mic os)  [ %% ]: %lf\n",
		memory_total/1024, memory_free/1024, memory_alloc/1024, memory_alloc_relative
	);
	
	// Free information struct about the MIC memory utilization.
	status = mic_free_memory_utilization_info(memory_info);
	CHECK_MIC_STATUS_WARN(status, "Cannot free MIC memory utilization structure.");
}

void mic_call(int32_t num_of_measurements) {
	struct mic_device *mic_device			= NULL;
	struct mic_core_util *mic_utilization	= NULL;
	int i;
	
	mic_init(&mic_device, &mic_utilization);
	
	mic_info(mic_device, mic_utilization);
	
	for (i=0; i<num_of_measurements; ++i) {
		fprintf(stdout, "\n");
		
		mic_read(mic_device, mic_utilization);
		
		sleep(1);
		
		if (i==(num_of_measurements-1)) {
			fprintf(stdout, "\n");
		}
	}
	
	mic_fini(&mic_device, &mic_utilization);
}
