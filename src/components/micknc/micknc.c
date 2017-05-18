/**
 * @file    micknc.c
 * @author  Joachim Protze
 *          joachim.protze@zih.tu-dresden.de
 * @author  Vince Weaver
 *          vweaver1@eecs.utk.edu
 *
 * @ingroup papi_components
 *
 * @brief
 *	This is an micknc component, it demos the component interface
 *  and implements three micknc counters.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

/* Headers required by PAPI */
#include "papi.h"
#include "papi_internal.h"
#include "papi_vector.h"
#include "papi_memory.h"    /* defines papi_malloc(), etc. */
#include "miclib.h"

#define S_PER_JIFFY 0.01
#define MIC_CORES 57
#define MIC_THREADS 4

#define TYPE_POWER	1
#define TYPE_TEMP	2
#define TYPE_FREQ	4
#define TYPE_TIME	8
#define TYPE_MEM	16

#define EVENT_POWER_PCIE	0
#define EVENT_POWER_C2X3	1
#define EVENT_POWER_C2X4	2
#define EVENT_POWER_VCCP	3
#define EVENT_POWER_VDDG	4
#define EVENT_POWER_VDDQ	5
#define EVENT_POWER_MIC		6
#define EVENT_TEMP_DIE		7
#define EVENT_TEMP_GDDR		8
#define EVENT_TEMP_FANIN	9
#define EVENT_TEMP_FANOUT	10
#define EVENT_TEMP_VCCP		11
#define EVENT_TEMP_VDDG		12
#define EVENT_TEMP_VDDQ		13
#define EVENT_FREQ_CORE		14
#define EVENT_FREQ_MEM		15
#define EVENT_TIME_ACTIVE	16
#define EVENT_TIME_IDLE		17
#define EVENT_UTIL			18
#define EVENT_MEM_TOTAL		19
#define EVENT_MEM_USED		20
#define EVENT_MEM_FREE		21
#define NUM_EVENTS			22

struct event_def {
	char* name;
	char* desc;
	int event_id;
	int event_type;
};

struct event_def event_definitions[] = {
	{"POWER_PCIE:DEVICE%d", 	"pcie power for device %d", EVENT_POWER_PCIE, TYPE_POWER},
	{"POWER_C2X3:DEVICE%d", 	"%d", EVENT_POWER_C2X3, TYPE_POWER},
	{"POWER_C2X4:DEVICE%d", 	"%d", EVENT_POWER_C2X4, TYPE_POWER},
	{"POWER_VCCP:DEVICE%d", 	"%d", EVENT_POWER_VCCP, TYPE_POWER},
	{"POWER_VDDG:DEVICE%d", 	"%d", EVENT_POWER_VDDG, TYPE_POWER},
	{"POWER_VDDQ:DEVICE%d", 	"%d", EVENT_POWER_VDDQ, TYPE_POWER},
	{"POWER_MIC:DEVICE%d", 		"%d", EVENT_POWER_MIC, TYPE_POWER},
	{"TEMP_DIE:DEVICE%d", 		"%d", EVENT_TEMP_DIE, TYPE_TEMP},
	{"TEMP_GDDR:DEVICE%d", 		"%d", EVENT_TEMP_GDDR, TYPE_TEMP},
	{"TEMP_FANIN:DEVICE%d", 	"%d", EVENT_TEMP_FANIN, TYPE_TEMP},
	{"TEMP_FANOUT:DEVICE%d", 	"%d", EVENT_TEMP_FANOUT, TYPE_TEMP},
	{"TEMP_VCCP:DEVICE%d", 		"%d", EVENT_TEMP_VCCP, TYPE_TEMP},
	{"TEMP_VDDG:DEVICE%d", 		"%d", EVENT_TEMP_VDDG, TYPE_TEMP},
	{"TEMP_VDDQ:DEVICE%d", 		"%d", EVENT_TEMP_VDDQ, TYPE_TEMP},
	{"FREQ_CORE:DEVICE%d", 		"%d", EVENT_FREQ_CORE, TYPE_FREQ},
	{"FREQ_MEM:DEVICE%d", 		"%d", EVENT_FREQ_MEM, TYPE_FREQ},
	{"TIME_ACTIVE:DEVICE%d", 	"%d", EVENT_TIME_ACTIVE, TYPE_TIME},
	{"TIME_IDLE:DEVICE%d", 		"%d", EVENT_TIME_IDLE, TYPE_TIME},
	{"UTIL:DEVICE%d", 			"%d", EVENT_UTIL, TYPE_TIME},
	{"MEM_TOTAL:DEVICE%d", 		"%d", EVENT_MEM_TOTAL, TYPE_MEM},
	{"MEM_USED:DEVICE%d", 		"%d", EVENT_MEM_USED, TYPE_MEM},
	{"MEM_FREE:DEVICE%d", 		"%d", EVENT_MEM_FREE, TYPE_MEM},
	NULL
};

/*
status = mic_get_devices(&mic_devices);
status = mic_get_ndevices(mic_devices, &num_of_mic_devices);
// Get sysfs device number of first MIC device in sysfs.
status = mic_get_device_at_index(mic_devices, 0, &sysfs_num_mic_device);
// Open MIC device according to sysfs MIC device number.
status = mic_open_device(&mpMicDevice, (uint32_t)sysfs_num_mic_device);
// Get MIC device type.
status = mic_get_device_type(mpMicDevice, &device_type);
// Free the device list.
status = mic_free_devices(mic_devices);
// Allocate MIC utilization struct.
status = mic_alloc_core_util(&mpMicUtilization);
// Obtain MIC's PCIe configuration.
status = mic_get_pci_config(mpMicDevice, &pcie);
// Get PCIe domain number.
status = mic_get_pci_domain_id(pcie, &pcie_domain_number);
// Get PCIe bus number.
status = mic_get_bus_number(pcie, &pcie_bus_number);
// Get PCIe device number.
status = mic_get_device_number(pcie, &pcie_device_number);
// Free MIC's PCIe configuration struct.
status = mic_free_pci_config(pcie);
// Get MIC version information.
status = mic_get_version_info(mpMicDevice, &version);
// Obtain OS version running on MIC.
status = mic_get_uos_version(version, uos, &size_string);
// Get MIC flash version.
status = mic_get_flash_version(version, flash, &size_string);
// Free version info struct.
status = mic_free_version_info(version);
// Obtain stock keeping unit (product id) of Xeon Phi board.
status = mic_get_silicon_sku(mpMicDevice, board_sku, &size_string);
// Returns SMC (System Mangement Controller) info as well as thermal info.
status = mic_get_thermal_info(mpMicDevice, &thermal);
// Get HW revision ID string.
status = mic_get_smc_hwrevision(thermal, revision, &size_string);
// Get firmware version.
status = mic_get_smc_fwversion(thermal, firmware, &size_string);
// Get boot loader version off MIC.
status = mic_get_smc_boot_loader_ver(thermal, boot_loader, &size_string);
// Free SKU/thermal info struct.
status = mic_free_thermal_info(thermal);
// Update core utilization struct.
status = mic_update_core_util(mpMicDevice, mpMicUtilization);
// Get number of cores on MIC.
status = mic_get_num_cores(mpMicUtilization, &num_of_cores);
// Get number of threads per core on MIC.
status = mic_get_threads_core(mpMicUtilization, &threads_per_core);
// Get information about the MIC memory.
status = mic_get_memory_info(mpMicDevice, &memory_info);
// Get MIC memory type.
status = mic_get_memory_type(memory_info, memory_type, &size_string);
// Get total size of MIC memory.
status = mic_get_memory_size(memory_info, &memory_size);
// Free struct storing the MIC memory info.
status = mic_free_memory_info(memory_info);

// Close MIC device.
status = mic_close_device(mpMicDevice);
// Free MIC utilization struct.
status = mic_free_core_util(mpMicUtilization);


// Get information about the MIC memory utilization.
status = mic_get_memory_utilization_info(mpMicDevice, &memory_info);
// Get total memory size according to MIC OS.
status = mic_get_total_memory_size(memory_info, &(pMsMeasurementMic->mic_memory_total));
// Free information struct about the MIC memory utilization.
status = mic_free_memory_utilization_info(memory_info);

// measure

// get power
// Returns power info.
status = mic_get_power_utilization_info(mpMicDevice, &power);
// Get power consumed via PCIe slot (max. 75W).
status = mic_get_pcie_power_readings(power, &temp);
// Get power consumed 2x3 (max. 75W) power connector.
status = mic_get_c2x3_power_readings(power,  &temp);
// Get power consumed 2x4 (max. 100W) power connector.
status = mic_get_c2x4_power_readings(power, &temp);
// Get core power rail power consumption.
status = mic_get_vccp_power_readings(power, &temp);
// Get uncore power rail power consumption.
status = mic_get_vddg_power_readings(power, &temp);
// Get memory subsystem power rail power consumption.
status = mic_get_vddq_power_readings(power, &temp);
// Calculate total power consumption
pMsMeasurementMic->mic_power_cur[MIC_POWER] = pMsMeasurementMic->mic_power_cur[MIC_PCIE] + pMsMeasurementMic->mic_power_cur[MIC_C2X3] +     pMsMeasurementMic->mic_power_cur[MIC_C2X4];
// Free power info struct.
status = mic_free_power_utilization_info(power);

// get util
// Update core utilization struct.
status = mic_update_core_util(mpMicDevice, mpMicUtilization);
// Get sum of idle time (in jiffies) of all cores.
status = mic_get_idle_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[IDLE_SUM]));
// Get sum of system time (in jiffies) of all cores.
status = mic_get_sys_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[SYS_SUM]));
// Get sum of nice time (in jiffies) of all cores.
status = mic_get_nice_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[NICE_SUM]));
// Get sum of user time (in jiffies) of all cores.
status = mic_get_user_sum(mpMicUtilization, &(pMsMeasurementMic->internal.mic_util_cur[USER_SUM]));

// get memory
// Get information about the MIC memory utilization.
status = mic_get_memory_utilization_info(mpMicDevice, &memory_info);
// Get size of free memory according to MIC OS.
status = mic_get_available_memory_size(memory_info, &(pMsMeasurementMic->mic_memory_free_cur));
pMsMeasurementMic->mic_memory_used_cur = pMsMeasurementMic->mic_memory_total - pMsMeasurementMic->mic_memory_free_cur;
// Free information struct about the MIC memory utilization.
status = mic_free_memory_utilization_info(memory_info);

// get frequency
// Get information about the MIC on-die cores.
status = mic_get_cores_info(mpMicDevice, &cores_info);
// Get current frequency of the MIC cores.
status = mic_get_cores_frequency(cores_info, &temp);
// Free struct storing the MIC on-die core info.
status = mic_free_cores_info(cores_info);

if(!(mMeasureCounter % TSkipMs)) {
// Get information about the MIC memory.
status = mic_get_memory_info(mpMicDevice, &memory_info);
// Get current frequency of the MIC memory.
status = mic_get_memory_frequency(memory_info, &temp);
// Free struct storing the MIC memory info.
status = mic_free_memory_info(memory_info);


// get temperature
// Returns thermal info.
status = mic_get_thermal_info(mpMicDevice, &thermal);
// Get die temperature.
status = mic_get_die_temp(thermal, &(pMsMeasurementMic->mic_temperature_cur[MIC_DIE_TEMP]));
// Get GDDR RAM temperature.
status = mic_get_gddr_temp(thermal, &temp);
// Get fan-in temperature.
status = mic_get_fanin_temp(thermal, &temp);
// Get fan-out temperature.
status = mic_get_fanout_temp(thermal, &temp);
// Get core power rail (vccp) temperature.
status = mic_get_vccp_temp(thermal, &temp);
// Get uncore power rail (vddg) temperature.
status = mic_get_vddg_temp(thermal, &temp);
// Get memory subsystem power rail (vddq) temperature.
status = mic_get_vddq_temp(thermal, &temp);
// Free thermal info struct.
status = mic_free_thermal_info(thermal);

*/



























/* Declare our vector in advance */
/* This allows us to modify the component info */
papi_vector_t _micknc_vector;

/** Structure that stores private information for each event */
typedef struct micknc_register
{
		           /**< Signifies which counter slot is being used */
			   /**< Indexed from 1 as 0 has a special meaning  */
} micknc_register_t;

/** This structure is used to build the table of events  */
/*   The contents of this structure will vary based on   */
/*   your component, however having name and description */
/*   fields are probably useful.                         */
typedef struct micknc_native_event_entry
{
	micknc_register_t resources;	    /**< Per counter resources       */
	char name[PAPI_MAX_STR_LEN];	    /**< Name of the counter         */
	char description[PAPI_MAX_STR_LEN]; /**< Description of the counter  */
	int writable;			    /**< Whether counter is writable */
	/* any other counter parameters go here */
	int event_id;
	int event_type;
	int device_id;
} micknc_native_event_entry_t;

/** This structure is used when doing register allocation 
    it possibly is not necessary when there are no 
    register constraints */
typedef struct micknc_reg_alloc
{
	micknc_register_t ra_bits;
} micknc_reg_alloc_t;

/** Holds control flags.  
 *    There's one of these per event-set.
 *    Use this to hold data specific to the EventSet, either hardware
 *      counter settings or things like counter start values 
 */
typedef struct micknc_control_state
{
  int num_events;
  int domain;
  int multiplexed;
  int overflow;
  int inherit;
  int max_counter; // defines size of which_counter / counter arrays
  int *which_counter;   
  long long *counter;   /**< Copy of counts, holds results when stopped */
  int event_types;
} micknc_control_state_t;

/** Holds per-thread information */
typedef struct micknc_context
{
     long long autoinc_value;
} micknc_context_t;

/** This table contains the native events */
static micknc_native_event_entry_t *micknc_native_table;

/** number of events in the table*/
static int num_events = 0;
static int num_devices = 0;
struct mic_device **mic_devices = NULL;

/** mic information structs **/
struct mic_core_util_counters {
	uint64_t idle_sum;
	uint64_t sys_sum;
	uint64_t nice_sum;
	uint64_t user_sum;
};
struct mic_memory_counters {
	uint32_t total;
	uint32_t used;
	uint32_t free;
};
struct mic_memory_counters *mic_info_memory_counters = NULL;
struct mic_core_util_counters *mic_info_core_util_values = NULL;
struct mic_core_util_counters *mic_info_core_util_counters = NULL;
struct mic_core_util_counters *mic_info_core_util_counters_prev = NULL;

struct mic_core_util 		**mic_info_core_util	= NULL;

struct mic_power_util_info 	**mic_info_power		= NULL;
struct mic_thermal_info    	**mic_info_thermal		= NULL;
struct mic_cores_info 		**mic_info_cores		= NULL;
struct mic_device_mem		**mic_info_memory		= NULL;
//struct mic_memory_util_info **mic_info_memory_util	= NULL;


/*************************************************************************/
/* Below is the actual "hardware implementation" of our micknc counters */
/*************************************************************************/

static int
micknc_read_total_memory() {
	int m = 0;
	struct mic_memory_util_info *memory_info    = NULL;
	for (m = 0; m < num_devices; ++m) {
		mic_get_memory_utilization_info(mic_devices[m], &memory_info);
		mic_get_total_memory_size(memory_info, &(mic_info_memory_counters[m].total));
		mic_free_memory_utilization_info(memory_info);
	}
	return 0;
}

static int
micknc_read_memory() {
	int m = 0;
	struct mic_memory_util_info *memory_info    = NULL;
	for (m = 0; m < num_devices; ++m) {
		mic_get_memory_utilization_info(mic_devices[m], &memory_info);
		mic_get_available_memory_size(memory_info, &(mic_info_memory_counters[m].free));
		mic_info_memory_counters[m].used = mic_info_memory_counters[m].total - mic_info_memory_counters[m].free;
		mic_free_memory_utilization_info(memory_info);
	}
	return 0;
}
/*
static int
micknc_read_memory_util() {
	int status, m = 0;
	for (m = 0; m < num_devices; ++m) {
		status = mic_get_memory_utilization_info(mic_devices[m], &(mic_info_memory_util[m]));
	}
	return 0;
}
*/

static int
micknc_read_core_util() {
	int m = 0;
	for (m = 0; m < num_devices; ++m) {
		mic_alloc_core_util(&mic_info_core_util[m]);
	}
	return 0;
}

static int
micknc_update_core_util() {
	int m = 0;
	struct mic_core_util_counters *temp_counters;

	// swap counters
	temp_counters = mic_info_core_util_counters;
	mic_info_core_util_counters = mic_info_core_util_counters_prev;
	mic_info_core_util_counters_prev = temp_counters;

	// read values
	for (m = 0; m < num_devices; ++m) {
		mic_update_core_util(mic_devices[m], mic_info_core_util[m]);
		mic_get_idle_sum(mic_info_core_util[m], &(mic_info_core_util_counters[m].idle_sum));
		mic_get_sys_sum(mic_info_core_util[m], &(mic_info_core_util_counters[m].sys_sum));
		mic_get_nice_sum(mic_info_core_util[m], &(mic_info_core_util_counters[m].nice_sum));
		mic_get_user_sum(mic_info_core_util[m], &(mic_info_core_util_counters[m].user_sum));

		mic_info_core_util_values[m].idle_sum = mic_info_core_util_counters[m].idle_sum - mic_info_core_util_counters_prev[m].idle_sum;
		mic_info_core_util_values[m].sys_sum = mic_info_core_util_counters[m].sys_sum - mic_info_core_util_counters_prev[m].sys_sum;
		mic_info_core_util_values[m].nice_sum = mic_info_core_util_counters[m].nice_sum - mic_info_core_util_counters_prev[m].nice_sum;
		mic_info_core_util_values[m].user_sum = mic_info_core_util_counters[m].user_sum - mic_info_core_util_counters_prev[m].user_sum;
	}
	return 0;
}

static int
micknc_free_core_util() {
	int m = 0;
	for (m = 0; m < num_devices; ++m) {
		mic_free_core_util(mic_info_core_util[m]);
		mic_info_core_util[m] = NULL;
	}
	return 0;
}

static int
micknc_read_power() {
	int m = 0;
	for (m = 0; m < num_devices; ++m) {
		mic_get_power_utilization_info(mic_devices[m], &(mic_info_power[m]));
	}
	return 0;
}

static int
micknc_read_temp() {
	int m = 0;
	for (m = 0; m < num_devices; ++m) {
		mic_get_thermal_info(mic_devices[m], &(mic_info_thermal[m]));
	}
	return 0;
}

static int
micknc_read_cores() {
	int m = 0;
	for (m = 0; m < num_devices; ++m) {
		mic_get_cores_info(mic_devices[m], &(mic_info_cores[m]));
	}
	return 0;
}

static int
micknc_read_memory_freq() {
	int m = 0;
	for (m = 0; m < num_devices; ++m) {
		mic_get_memory_info(mic_devices[m], &(mic_info_memory[m]));
	}
	return 0;
}

static int
micknc_free_necessary_counters() {
	int m = 0;
	if (mic_info_power[0] != NULL) {
		for (m = 0; m < num_devices; ++m) {
			mic_free_power_utilization_info(mic_info_power[m]);
			mic_info_power[m] = NULL;
		}
	}
	if (mic_info_thermal[0] != NULL) {
		for (m = 0; m < num_devices; ++m) {
			mic_free_thermal_info(mic_info_thermal[m]);
			mic_info_thermal[m] = NULL;
		}
	}
	if (mic_info_cores[0] != NULL) {
		for (m = 0; m < num_devices; ++m) {
			mic_free_cores_info(mic_info_cores[m]);
			mic_info_cores[m] = NULL;
		}
	}
	if (mic_info_memory[0] != NULL) {
		for (m = 0; m < num_devices; ++m) {
			mic_free_memory_info(mic_info_memory[m]);
			mic_info_memory[m] = NULL;
		}
	}
	return 0;
}

static int
micknc_read_necessary_counters(int event_types) {
	if ( (event_types & TYPE_POWER) == TYPE_POWER ) {
		micknc_read_power();
	}
	if ( (event_types & TYPE_TEMP) == TYPE_TEMP ) {
		micknc_read_temp();
	}
	if ( (event_types & TYPE_FREQ) == TYPE_FREQ ) {
		micknc_read_cores();
		micknc_read_memory_freq();
	}
	if ( (event_types & TYPE_TIME) == TYPE_TIME ) {
		micknc_update_core_util();
	}
	if ( (event_types & TYPE_MEM) == TYPE_MEM ) {
		micknc_read_memory();
	}
	return 0;
}

static int
micknc_set_counter(int native_id, int device_id, long long *result) {

	uint32_t temp = 0;
	uint16_t temp16 = 0;
	uint32_t pcie, c2x3, c2x4;

	switch (native_id) {
		case EVENT_POWER_PCIE:
			mic_get_pcie_power_readings(mic_info_power[device_id], &temp);
		break;
		case EVENT_POWER_C2X3:
			mic_get_c2x3_power_readings(mic_info_power[device_id],  &temp);
		break;
		case EVENT_POWER_C2X4:
			mic_get_c2x4_power_readings(mic_info_power[device_id], &temp);
		break;
		case EVENT_POWER_VCCP:
			mic_get_vccp_power_readings(mic_info_power[device_id], &temp);
		break;
		case EVENT_POWER_VDDG:
			mic_get_vddg_power_readings(mic_info_power[device_id], &temp);
		break;
		case EVENT_POWER_VDDQ:
			mic_get_vddq_power_readings(mic_info_power[device_id], &temp);
		break;
		case EVENT_POWER_MIC:
			mic_get_pcie_power_readings(mic_info_power[device_id], &pcie);
			mic_get_c2x3_power_readings(mic_info_power[device_id],  &c2x3);
			mic_get_c2x4_power_readings(mic_info_power[device_id], &c2x4);
			temp = pcie + c2x3 + c2x4;
		break;
		case EVENT_TEMP_DIE:
			mic_get_die_temp(mic_info_thermal[device_id], &temp);
		break;
		case EVENT_TEMP_GDDR:
			mic_get_gddr_temp(mic_info_thermal[device_id], &temp16);
		break;
		case EVENT_TEMP_FANIN:
			mic_get_fanin_temp(mic_info_thermal[device_id], &temp16);
		break;
		case EVENT_TEMP_FANOUT:
			mic_get_fanout_temp(mic_info_thermal[device_id], &temp16);
		break;
		case EVENT_TEMP_VCCP:
			mic_get_vccp_temp(mic_info_thermal[device_id], &temp16);
		break;
		case EVENT_TEMP_VDDG:
			mic_get_vddg_temp(mic_info_thermal[device_id], &temp16);
		break;
		case EVENT_TEMP_VDDQ:
			mic_get_vddq_temp(mic_info_thermal[device_id], &temp16);
		break;
		case EVENT_FREQ_CORE:
			mic_get_cores_frequency(mic_info_cores[device_id], &temp);
		break;
		case EVENT_FREQ_MEM:
			mic_get_memory_frequency(mic_info_memory[device_id], &temp);
		break;
		case EVENT_TIME_ACTIVE:
			temp = 	(uint64_t) (1000.0 * (double)(mic_info_core_util_values[device_id].sys_sum +
					mic_info_core_util_values[device_id].nice_sum +
					mic_info_core_util_values[device_id].user_sum) * S_PER_JIFFY / MIC_CORES / MIC_THREADS) ;
		break;
		case EVENT_TIME_IDLE:
			temp = (uint64_t) (1000.0 * (double) mic_info_core_util_values[device_id].idle_sum * S_PER_JIFFY / MIC_CORES / MIC_THREADS);
		break;
		case EVENT_UTIL:
			temp = 	mic_info_core_util_values[device_id].sys_sum +
					mic_info_core_util_values[device_id].nice_sum +
					mic_info_core_util_values[device_id].user_sum;
			temp = (uint64_t) ( (double)temp / (double)(temp + mic_info_core_util_values[device_id].idle_sum));
		break;
		case EVENT_MEM_TOTAL:
			temp = mic_info_memory_counters[device_id].total;
		break;
		case EVENT_MEM_USED:
			temp = mic_info_memory_counters[device_id].used;
		break;
		case EVENT_MEM_FREE:
			temp = mic_info_memory_counters[device_id].free;
		break;
		default:
			return -1;
		break;
	}
	*result = temp + temp16;

	return 0;
}


/********************************************************************/
/* Below are the functions required by the PAPI component interface */
/********************************************************************/


/** Initialize hardware counters, setup the function vector table
 * and get hardware information, this routine is called when the
 * PAPI process is initialized (IE PAPI_library_init)
 */
static int
_micknc_init_component( int cidx )
{

	SUBDBG( "_micknc_init_component..." );

	int status;
   
    /* First, detect that our hardware is available */
	struct mic_devices_list *mic_list;
	status = mic_get_devices(&mic_list);
	if (status != E_MIC_SUCCESS) {
	   return PAPI_ECMP;
	}
	status = mic_get_ndevices(mic_list, &num_devices);
	if (status != E_MIC_SUCCESS) {
	   return PAPI_ECMP;
	}
   
	/* we know in advance how many events we want                       */
	/* for actual hardware this might have to be determined dynamically */
	num_events = NUM_EVENTS * num_devices;

	mic_devices = (struct mic_device**) papi_calloc(sizeof(struct mic_device*), num_devices);
	mic_info_power = (struct mic_power_util_info**) papi_calloc(sizeof(struct mic_power_util_info*), num_devices);
	mic_info_thermal = (struct mic_thermal_info**) papi_calloc(sizeof(struct mic_thermal_info*), num_devices);
	mic_info_cores = (struct mic_cores_info**) papi_calloc(sizeof(struct mic_cores_info*), num_devices);
	mic_info_memory = (struct mic_device_mem**) papi_calloc(sizeof(struct mic_device_mem*), num_devices);
	//mic_info_memory_util = (struct mic_memory_util_info**) papi_calloc(sizeof(struct mic_memory_util_info*), num_devices);
	mic_info_core_util_values = (struct mic_core_util_counters*) papi_calloc(sizeof(struct mic_core_util_counters), num_devices);
	mic_info_core_util_counters = (struct mic_core_util_counters*) papi_calloc(sizeof(struct mic_core_util_counters), num_devices);
	mic_info_core_util_counters_prev = (struct mic_core_util_counters*) papi_calloc(sizeof(struct mic_core_util_counters), num_devices);
	mic_info_core_util = (struct mic_core_util**) papi_calloc(sizeof(struct mic_core_util*), num_devices);
	mic_info_memory_counters = (struct mic_memory_counters*) papi_calloc(sizeof(struct mic_memory_counters), num_devices);

 
	int m = 0;
	int sysfs_device;
	for (m = 0; m<num_devices; ++m) {
		// Get sysfs device number of first MIC device in sysfs.
		status = mic_get_device_at_index(mic_list, m, &sysfs_device);
		// Open MIC device according to sysfs MIC device number.
		status = mic_open_device(&(mic_devices[m]), (uint32_t)sysfs_device);
	}

	status = mic_free_devices(mic_list);

	status = micknc_read_core_util();
	status = micknc_read_total_memory();

	/* Allocate memory for the our native event table */
	micknc_native_table =
		( micknc_native_event_entry_t * )
		papi_calloc( sizeof(micknc_native_event_entry_t),num_events);
	if ( micknc_native_table == NULL ) {
		PAPIERROR( "malloc():Could not get memory for events table" );
		return PAPI_ENOMEM;
	}

	/* fill in the event table parameters */
	/* for complicated components this will be done dynamically */
	/* or by using an external library                          */
	int deviceIx = 0;
	int j = 0;
	int eventIx = 0;
	for (deviceIx = 0; deviceIx<num_devices; ++deviceIx) {
		for (j = 0; j<NUM_EVENTS; ++j) {

        	snprintf( micknc_native_table[eventIx].name, PAPI_MAX_STR_LEN,
            	event_definitions[j].name, deviceIx);
        	snprintf( micknc_native_table[eventIx].description, PAPI_MAX_STR_LEN,
            	event_definitions[j].desc, deviceIx);
        	micknc_native_table[eventIx].writable = 0; 
        	micknc_native_table[eventIx].event_id = event_definitions[j].event_id;
        	micknc_native_table[eventIx].event_type = event_definitions[j].event_type;
        	micknc_native_table[eventIx].device_id = deviceIx;
        	eventIx++;

		}
	}

	/* Export the total number of events available */
	_micknc_vector.cmp_info.num_native_events = num_events;

    _micknc_vector.cmp_info.num_cntrs = num_events;
    _micknc_vector.cmp_info.num_mpx_cntrs = num_events;


	/* Export the component id */
	_micknc_vector.cmp_info.CmpIdx = cidx;

	return PAPI_OK;
}

/** This is called whenever a thread is initialized */
static int
_micknc_init_thread( hwd_context_t *ctx )
{

        micknc_context_t *micknc_context = (micknc_context_t *)ctx;

        micknc_context->autoinc_value=0;
   
	SUBDBG( "_micknc_init_thread %p...", ctx );

	return PAPI_OK;
}



/** Setup a counter control state.
 *   In general a control state holds the hardware info for an
 *   EventSet.
 */

static int
_micknc_init_control_state( hwd_control_state_t * ctl )
{
   SUBDBG( "micknc_init_control_state... %p\n", ctl );

   micknc_control_state_t *micknc_ctl = ( micknc_control_state_t * ) ctl;
   memset( micknc_ctl, 0, sizeof ( micknc_control_state_t ) );

   return PAPI_OK;
}


/** Triggered by eventset operations like add or remove */
static int
_micknc_update_control_state( hwd_control_state_t *ctl, 
				    NativeInfo_t *native,
				    int count, 
				    hwd_context_t *ctx )
{
   
   (void) ctx;
   int i, index;

   micknc_control_state_t *micknc_ctl = ( micknc_control_state_t * ) ctl;   

   SUBDBG( "_micknc_update_control_state %p %p...", ctl, ctx );

    // resize which_counter / counter arrays
    if (count > micknc_ctl->max_counter || count*2 < micknc_ctl->max_counter) {
        if (micknc_ctl->which_counter != NULL) {
            papi_free(micknc_ctl->which_counter);
        }
        if (micknc_ctl->counter != NULL) {
            papi_free(micknc_ctl->counter);
        }
        if (count == 0) {
            micknc_ctl->max_counter = 0;
        } else {
            if (micknc_ctl->max_counter == 0) {
                micknc_ctl->max_counter = 16;
            }
            while (micknc_ctl->max_counter < count) micknc_ctl->max_counter *= 2;
            while (micknc_ctl->max_counter > 2*count) micknc_ctl->max_counter /= 2;
            micknc_ctl->which_counter = (int*) papi_calloc( sizeof(int), micknc_ctl->max_counter);
            micknc_ctl->counter = (long long*) papi_calloc( sizeof(long long), micknc_ctl->max_counter);
        }
    }

	/* if no events, return */
	if (count==0) return PAPI_OK;

    micknc_ctl->event_types = 0;
	for( i = 0; i < count; i++ ) {

        index = native[i].ni_event;

        // Map counter #i to Measure Event "index"
        micknc_ctl->which_counter[i]=index;

        // We have no constraints on event position, so any event
        // can be in any slot.                                  
        native[i].ni_position = i;
		micknc_ctl->event_types = micknc_ctl->event_types | micknc_native_table[index].event_type;

	}


   micknc_ctl->num_events=count;

   return PAPI_OK;
}

/** Triggered by PAPI_start() */
static int
_micknc_start( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

        (void) ctx;
        (void) ctl;

	SUBDBG( "micknc_start %p %p...", ctx, ctl );

	/* anything that would need to be set at counter start time */

	/* reset counters? */
        /* For hardware that cannot reset counters, store initial        */
        /*     counter state to the ctl and subtract it off at read time */
	 
	/* start the counting ?*/

	return PAPI_OK;
}


/** Triggered by PAPI_stop() */
static int
_micknc_stop( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

        (void) ctx;
        (void) ctl;

	SUBDBG( "micknc_stop %p %p...", ctx, ctl );

	/* anything that would need to be done at counter stop time */

	

	return PAPI_OK;
}


/** Triggered by PAPI_read()     */
/*     flags field is never set? */
static int
_micknc_read( hwd_context_t *ctx, hwd_control_state_t *ctl,
			  long long **events, int flags )
{

	(void) flags;

	micknc_control_state_t *micknc_ctl = ( micknc_control_state_t *) ctl;   

	SUBDBG( "micknc_read... %p %d", ctx, flags );

	int i;

	micknc_read_necessary_counters(micknc_ctl->event_types);

	int event_id;
	int event_device_id;
	/* Read counters into expected slot */
	for(i=0;i<micknc_ctl->num_events;i++) {

        event_id = micknc_ctl->which_counter[i];
        event_device_id = micknc_native_table[event_id].device_id;

		micknc_set_counter(event_id, event_device_id, &(micknc_ctl->counter[i]));
	}
	
	micknc_free_necessary_counters();

	/* return pointer to the values we read */
	*events = micknc_ctl->counter;

	return PAPI_OK;
}

/** Triggered by PAPI_write(), but only if the counters are running */
/*    otherwise, the updated state is written to ESI->hw_start      */
static int
_micknc_write( hwd_context_t *ctx, hwd_control_state_t *ctl,
			   long long *events )
{

   
	SUBDBG( "micknc_write... ");

	return PAPI_OK;
}


/** Triggered by PAPI_reset() but only if the EventSet is currently running */
/*  If the eventset is not currently running, then the saved value in the   */
/*  EventSet is set to zero without calling this routine.                   */
static int
_micknc_reset( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

	SUBDBG( "micknc_reset...");

	/* Reset the hardware */
	return PAPI_OK;
}

/** Triggered by PAPI_shutdown() */
static int
_micknc_shutdown_component(void)
{

	SUBDBG( "micknc_shutdown_component..." );

        /* Free anything we allocated */

	int m = 0;
	for (m = 0; m<num_devices; ++m) {
		mic_close_device(mic_devices[m]);
		mic_devices[m] = NULL;
	}
	micknc_free_core_util();

	papi_free(mic_devices);
	papi_free(mic_info_power);
	papi_free(mic_info_thermal);
	papi_free(mic_info_cores);
	papi_free(mic_info_memory);
	papi_free(mic_info_core_util_values);
	papi_free(mic_info_core_util_counters);
	papi_free(mic_info_core_util_counters_prev);
	papi_free(mic_info_core_util);
	papi_free(mic_info_memory_counters);


	papi_free(micknc_native_table);

	return PAPI_OK;
}

/** Called at thread shutdown */
static int
_micknc_shutdown_thread( hwd_context_t *ctx )
{

        (void) ctx;

	SUBDBG( "micknc_shutdown_thread... %p", ctx );

	/* Last chance to clean up thread */

	return PAPI_OK;
}



/** This function sets various options in the component
  @param[in] ctx -- hardware context
  @param[in] code valid are PAPI_SET_DEFDOM, PAPI_SET_DOMAIN, 
                        PAPI_SETDEFGRN, PAPI_SET_GRANUL and PAPI_SET_INHERIT
  @param[in] option -- options to be set
 */
static int
_micknc_ctl( hwd_context_t *ctx, int code, _papi_int_option_t *option )
{

        (void) ctx;
	(void) code;
	(void) option;

	SUBDBG( "micknc_ctl..." );

	return PAPI_OK;
}

/** This function has to set the bits needed to count different domains
    In particular: PAPI_DOM_USER, PAPI_DOM_KERNEL PAPI_DOM_OTHER
    By default return PAPI_EINVAL if none of those are specified
    and PAPI_OK with success
    PAPI_DOM_USER is only user context is counted
    PAPI_DOM_KERNEL is only the Kernel/OS context is counted
    PAPI_DOM_OTHER  is Exception/transient mode (like user TLB misses)
    PAPI_DOM_ALL   is all of the domains
 */
static int
_micknc_set_domain( hwd_control_state_t * cntrl, int domain )
{
        (void) cntrl;

	int found = 0;
	SUBDBG( "micknc_set_domain..." );

	if ( PAPI_DOM_USER & domain ) {
		SUBDBG( " PAPI_DOM_USER " );
		found = 1;
	}
	if ( PAPI_DOM_KERNEL & domain ) {
		SUBDBG( " PAPI_DOM_KERNEL " );
		found = 1;
	}
	if ( PAPI_DOM_OTHER & domain ) {
		SUBDBG( " PAPI_DOM_OTHER " );
		found = 1;
	}
	if ( PAPI_DOM_ALL & domain ) {
		SUBDBG( " PAPI_DOM_ALL " );
		found = 1;
	}
	if ( !found )
		return ( PAPI_EINVAL );

	return PAPI_OK;
}


/**************************************************************/
/* Naming functions, used to translate event numbers to names */
/**************************************************************/


/** Enumerate Native Events
 *   @param EventCode is the event of interest
 *   @param modifier is one of PAPI_ENUM_FIRST, PAPI_ENUM_EVENTS
 *  If your component has attribute masks then these need to
 *   be handled here as well.
 */
static int
_micknc_ntv_enum_events( unsigned int *EventCode, int modifier )
{
  int index;


  switch ( modifier ) {

		/* return EventCode of first event */
	case PAPI_ENUM_FIRST:
	   /* return the first event that we support */

	   *EventCode = 0;
	   return PAPI_OK;

		/* return EventCode of next available event */
	case PAPI_ENUM_EVENTS:
	   index = *EventCode;

	   /* Make sure we have at least 1 more event after us */
	   if ( index < num_events - 1 ) {

	      /* This assumes a non-sparse mapping of the events */
	      *EventCode = *EventCode + 1;
	      return PAPI_OK;
	   } else {
	      return PAPI_ENOEVNT;
	   }
	   break;
	
	default:
	   return PAPI_EINVAL;
  }

  return PAPI_EINVAL;
}

/** Takes a native event code and passes back the name 
 * @param EventCode is the native event code
 * @param name is a pointer for the name to be copied to
 * @param len is the size of the name string
 */
static int
_micknc_ntv_code_to_name( unsigned int EventCode, char *name, int len )
{
  int index;

  index = EventCode;

  /* Make sure we are in range */
  if (index >= 0 && index < num_events) {
     strncpy( name, micknc_native_table[index].name, len );  
     return PAPI_OK;
  }
   
  return PAPI_ENOEVNT;
}

/** Takes a native event code and passes back the event description
 * @param EventCode is the native event code
 * @param descr is a pointer for the description to be copied to
 * @param len is the size of the descr string
 */
static int
_micknc_ntv_code_to_descr( unsigned int EventCode, char *descr, int len )
{
  int index;
  index = EventCode;

  /* make sure event is in range */
  if (index >= 0 && index < num_events) {
     strncpy( descr, micknc_native_table[index].description, len );
     return PAPI_OK;
  }
  
  return PAPI_ENOEVNT;
}

/** Vector that points to entry points for our component */
papi_vector_t _micknc_vector = {
	.cmp_info = {
		/* default component information */
		/* (unspecified values are initialized to 0) */
                /* we explicitly set them to zero in this micknc */
                /* to show what settings are available            */

		.name = "micknc",
		.short_name = "micknc",
		.description = "A simple micknc component",
		.version = "1.15",
		.support_version = "n/a",
		.kernel_version = "n/a",
		.num_cntrs =               0, 
		.num_mpx_cntrs =           0,
		.default_domain =          PAPI_DOM_USER,
		.available_domains =       PAPI_DOM_USER,
		.default_granularity =     PAPI_GRN_THR,
		.available_granularities = PAPI_GRN_THR,
		.hardware_intr_sig =       PAPI_INT_SIGNAL,

		/* component specific cmp_info initializations */
	},

	/* sizes of framework-opaque component-private structures */
	.size = {
	        /* once per thread */
		.context = sizeof ( micknc_context_t ),
	        /* once per eventset */
		.control_state = sizeof ( micknc_control_state_t ),
	        /* ?? */
		.reg_value = sizeof ( micknc_register_t ),
	        /* ?? */
		.reg_alloc = sizeof ( micknc_reg_alloc_t ),
	},

	/* function pointers */
        /* by default they are set to NULL */
   
	/* Used for general PAPI interactions */
	.start =                _micknc_start,
	.stop =                 _micknc_stop,
	.read =                 _micknc_read,
	.reset =                _micknc_reset,	
	.write =                _micknc_write,
	.init_component =       _micknc_init_component,	
	.init_thread =          _micknc_init_thread,
	.init_control_state =   _micknc_init_control_state,
	.update_control_state = _micknc_update_control_state,	
	.ctl =                  _micknc_ctl,	
	.shutdown_thread =      _micknc_shutdown_thread,
	.shutdown_component =   _micknc_shutdown_component,
	.set_domain =           _micknc_set_domain,
	/* .cleanup_eventset =     NULL, */
	/* called in add_native_events() */
	/* .allocate_registers =   NULL, */

	/* Used for overflow/profiling */
	/* .dispatch_timer =       NULL, */
	/* .get_overflow_address = NULL, */
	/* .stop_profiling =       NULL, */
	/* .set_overflow =         NULL, */
	/* .set_profile =          NULL, */

	/* ??? */
	/* .user =                 NULL, */

	/* Name Mapping Functions */
	.ntv_enum_events =   _micknc_ntv_enum_events,
	.ntv_code_to_name =  _micknc_ntv_code_to_name,
	.ntv_code_to_descr = _micknc_ntv_code_to_descr,
        /* if .ntv_name_to_code not available, PAPI emulates  */
        /* it by enumerating all events and looking manually  */
   	.ntv_name_to_code  = NULL,

   
	/* These are only used by _papi_hwi_get_native_event_info() */
	/* Which currently only uses the info for printing native   */
	/* event info, not for any sort of internal use.            */
	/* .ntv_code_to_bits =  NULL, */

};

