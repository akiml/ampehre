/**
 * @file    example.c
 * @author  Joachim Protze
 *          joachim.protze@zih.tu-dresden.de
 * @author  Vince Weaver
 *          vweaver1@eecs.utk.edu
 *
 * @ingroup papi_components
 *
 * @brief
 *	This is an example component, it demos the component interface
 *  and implements three example counters.
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
#include "maxdsd.h"
#include "cJSON.h"

/* Declare our vector in advance */
/* This allows us to modify the component info */
papi_vector_t _maxeler_vector;

/** Structure that stores private information for each event */
typedef struct maxeler_register
{
} maxeler_register_t;

/** This structure is used to build the table of events  */
/*   The contents of this structure will vary based on   */
/*   your component, however having name and description */
/*   fields are probably useful.                         */
typedef struct maxeler_native_event_entry
{
	maxeler_register_t resources;	    /**< Per counter resources       */
	char name[PAPI_MAX_STR_LEN];	    /**< Name of the counter         */
	char description[PAPI_MAX_STR_LEN]; /**< Description of the counter  */
	int writable;			    /**< Whether counter is writable */
	/* any other counter parameters go here */
	int event_id;
	int device_id;
} maxeler_native_event_entry_t;

/** This structure is used when doing register allocation 
    it possibly is not necessary when there are no 
    register constraints */

typedef struct maxeler_reg_alloc
{
	maxeler_register_t ra_bits;
} maxeler_reg_alloc_t;


/** Holds control flags.  
 *    There's one of these per event-set.
 *    Use this to hold data specific to the EventSet, either hardware
 *      counter settings or things like counter start values 
 */
typedef struct maxeler_control_state
{
  int num_events;
  int domain;
  int multiplexed;
  int overflow;
  int inherit;
  int max_counter; // defines size of which_counter / counter arrays
  int *which_counter; 
  long long *counter;   /**< Copy of counts, holds results when stopped */
  int event_types; // event types used in this event set
} maxeler_control_state_t;

/** Holds per-thread information */
typedef struct maxeler_context
{
     long long autoinc_value;
} maxeler_context_t;

/** This table contains the native events */
static maxeler_native_event_entry_t *maxeler_native_table;

int maxelerosd = 0;

char *jstr_power_request = "[{ \"request_type\": \"get_power_data\", \"args\": { \"acceptable_age\": \"0\" } }]";
char *jstr_compute_request = "[{ \"request_type\": \"get_computation_utilization_data\" }]";
//char *jstr_memory_request = "[{ \"request_type\": \"get_memory_utilization_data\" }]";

/** number of events in the table*/
static int num_events = 0;
static int num_devices = 0;
// array with current values per device
struct maxeler_raw_values *current_values = NULL;

/*
char **rail_names = {
	"vcc1v0_core",
	"vcc1v5_ddr",
	"vcc2v5_aux",
	"imgt_1v0",
	"imgt_1v2",
	"mgt_1v0",
	"mgt_1v2",
	NULL
};
*/

typedef struct maxeler_raw_values
{
	double fpga_power;
	double vcc1v0_c_power;
	double vcc1v5_ddr_power;
	double vcc2v5_aux_power;
	double imgt_1v0_power;
	double imgt_1v2_power;
	double mgt_1v0_power;
	double mgt_1v2_power;
	double imgt_temperature;
	double mmgt_temperature;
	double compute_utilization;

} maxeler_raw_power;

#define EVENT_TYPE_POWER 		1
#define EVENT_TYPE_TEMPERATURE 	2
#define EVENT_TYPE_UTIL 		4

#define EVENT_FPGA_POWER 			0
#define EVENT_VCC1V0_C_POWER 		1
#define EVENT_VCC1V5_DDR_POWER 		2
#define EVENT_VCC2V5_AUX_POWER 		3
#define EVENT_IMGT_1V0_POWER 		4
#define EVENT_IMGT_1V2_POWER 		5
#define EVENT_MGT_1V0_POWER 		6
#define EVENT_MGT_1V2_POWER 		7
#define EVENT_IMGT_TEMPERATURE 		8
#define EVENT_MMGT_TEMPERATURE 		9
#define EVENT_COMPUTE_UTILIZATION 	10

/*************************************************************************/
/* Below is the actual "hardware implementation" of our example counters */
/*************************************************************************/

static int
detect_maxeler_devices(int *out_num_devices) {

	*out_num_devices = max_daemon_device_count(maxelerosd);
	return PAPI_OK;
}

int
read_maxeler_utilization() {
	cJSON *json_response        = NULL;
	cJSON *json_measurements    = NULL;
	cJSON *temp 				= NULL;

	char *jstr_response = NULL;

	int ret = max_daemon_exchange_json_string(maxelerosd, &jstr_response, jstr_compute_request);
	if (ret || NULL == jstr_response) {
		return -1;
	}

	json_response = cJSON_Parse(jstr_response);
	if (NULL == json_response) {
		free(jstr_response);
		return -1;
	}

	temp = cJSON_GetArrayItem(json_response, 0);
	if (NULL == temp) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}
	temp = cJSON_GetObjectItem(temp, "success");
	if (NULL == temp) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}
	int success = temp->type;
	if (!success) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}

	int deviceIx;
	char engineStr[10];
	for(deviceIx = 0; deviceIx < num_devices; ++deviceIx) {

		snprintf(engineStr, 10, "engine%d", deviceIx);
		temp = cJSON_GetArrayItem(json_response, 0);
		if (NULL == temp) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		temp = cJSON_GetObjectItem(temp, "data");
		if (NULL == temp) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		json_measurements = cJSON_GetObjectItem(temp, engineStr);
		if (NULL == json_measurements) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}

		temp = cJSON_GetObjectItem(json_measurements, "usage");
		if (NULL == temp) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		current_values[deviceIx].compute_utilization = temp->valuedouble;
	}

	cJSON_Delete(json_response);

	free(jstr_response);
	return PAPI_OK;
}

static int
read_maxeler_power() {

	cJSON *json_response 		= NULL;
	cJSON *json_measurements    = NULL;
	cJSON *json_single          = NULL;
	cJSON *temp					= NULL;
	
	char* jstr_response = NULL;

	int ret = max_daemon_exchange_json_string(maxelerosd, &jstr_response, jstr_power_request);
	if (ret || NULL == jstr_response) {
		return -1;
	}

	json_response = cJSON_Parse(jstr_response);
	if (NULL == json_response) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}

	//TODO: Add additional checks, in case the interediate pointers are NULL (some structural item do not exist)
	temp = cJSON_GetArrayItem(json_response, 0);
	if (NULL == temp) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}
	temp = cJSON_GetObjectItem(temp, "success");
	if (NULL == temp) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}
	
	int success = temp->type;
	if (!success) {
		cJSON_Delete(json_response);
		free(jstr_response);
		return -1;
	}

	int deviceIx;
	int itemIx;
	double voltage = 0.0;
	double current = 0.0;
	char engineStr[10];
	for(deviceIx = 0; deviceIx < num_devices; ++deviceIx) {

		snprintf(engineStr, 10, "engine%d", deviceIx);
		temp = cJSON_GetArrayItem(json_response, 0);
		if (NULL == temp) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		temp = cJSON_GetObjectItem(temp, "data");
		if (NULL == temp) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		json_measurements = cJSON_GetObjectItem(temp, engineStr);
		if (NULL == json_measurements) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		
		temp = cJSON_GetObjectItem(json_measurements, "power_usage");
		if (NULL == temp) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		
		current_values[deviceIx].fpga_power = temp->valuedouble*1000;		

		json_measurements = cJSON_GetObjectItem(json_measurements, "measurements");
		if (NULL == json_measurements) {
			cJSON_Delete(json_response);
			free(jstr_response);
			return -1;
		}
		
		int size = cJSON_GetArraySize(json_measurements);
		for (itemIx = 0; itemIx < size; ++itemIx) {
			json_single = cJSON_GetArrayItem(json_measurements, itemIx);
			if (NULL == json_single) {
				continue;
			}
			temp = cJSON_GetObjectItem(json_single, "voltage");
			if (NULL == temp) {
				continue;
			}
			voltage = temp->valuedouble;
			temp = cJSON_GetObjectItem(json_single, "current");
			if (NULL == temp) {
				continue;
			}
			current = temp->valuedouble;
			temp = cJSON_GetObjectItem(json_single, "rail_name");
			if (NULL == temp) {
				continue;
			}

			if (strcmp(temp->valuestring, "vcc1v0_core") == 0) {
				current_values[deviceIx].vcc1v0_c_power = voltage * current*1000.0 / 0.85;
			} else
			if (strcmp(temp->valuestring, "vcc1v5_ddr") == 0) {
				current_values[deviceIx].vcc1v5_ddr_power = voltage * current*1000.0 / 0.85;
			} else
			if (strcmp(temp->valuestring, "vcc2v5_aux") == 0) {
				current_values[deviceIx].vcc2v5_aux_power = voltage * current*1000.0 / 0.85;
			} else
			if (strcmp(temp->valuestring, "imgt_1v0") == 0) {
				current_values[deviceIx].imgt_1v0_power = voltage * current*1000.0 / 0.65;
			} else
			if (strcmp(temp->valuestring, "imgt_1v2") == 0) {
				current_values[deviceIx].imgt_1v2_power = voltage * current*1000.0 / 0.65;
			} else
			if (strcmp(temp->valuestring, "mgt_1v0") == 0) {
				current_values[deviceIx].mgt_1v0_power = voltage * current*1000.0 / 0.65;
			} else
			if (strcmp(temp->valuestring, "mgt_1v2") == 0) {
				current_values[deviceIx].mgt_1v2_power = voltage * current*1000.0 / 0.65;
			}
		}

	}
	cJSON_Delete(json_response);
	free(jstr_response);


	return PAPI_OK;
}

int extractTemperature(char* response, double *mtemp, double *itemp) {
    char *mfpga = strstr(response, "MAIN_FPGA_TEMPERATURE=");
	if (NULL == mfpga) {
		return -1;
	}
    char *meq   = strchr(mfpga, '=');
	if (NULL == meq) {
		return -1;
	}
    char *mend  = strchr(meq++, ',');
	if (NULL == mend) {
		return -1;
	}

    char *ifpga = strstr(response, "MAX3_IFCEFPGA_TEMPERATURE=");
	if (NULL == ifpga) {
		return -1;
	}
    char *ieq   = strchr(ifpga, '=');
	if (NULL == ieq) {
		return -1;
	}
    char *iend  = strchr(ieq++, ',');
	if (NULL == iend) {
		return -1;
	}

    *mend       = '\0';
    *iend       = '\0';

	int ret;
    ret = sscanf(meq, "%lf", mtemp);
	if (ret < 0) {
		return -1;
	}
    ret = sscanf(ieq, "%lf", itemp);
	if (ret < 0) {
		return -1;
	}

	return PAPI_OK;
}

static int
read_maxeler_temperature() {
	char *response;

	int deviceIx;
	for(deviceIx = 0; deviceIx < num_devices; ++deviceIx) {

		max_daemon_device_hw_monitor(maxelerosd, 0, &response);

		extractTemperature(response, &(current_values[deviceIx].mmgt_temperature), &(current_values[deviceIx].imgt_temperature));

		free(response);
	}

	return PAPI_OK;
}

/********************************************************************/
/* Below are the functions required by the PAPI component interface */
/********************************************************************/


/** Initialize hardware counters, setup the function vector table
 * and get hardware information, this routine is called when the
 * PAPI process is initialized (IE PAPI_library_init)
 */
static int
_maxeler_init_component( int cidx )
{ 

	SUBDBG( "_maxeler_init_component..." );

	/* Try to open socket, on failure the component is not available*/
	maxelerosd = max_daemon_connect("maxelerosd.sock");
	if (maxelerosd < 0) {
		return PAPI_ECMP;
	}

	if ( PAPI_OK != detect_maxeler_devices(&num_devices) ) {
		// something went terribly wrong
		//TODO: release socket
		return PAPI_ECMP;
	}
	 

	current_values = (struct maxeler_raw_values*) papi_calloc(sizeof(struct maxeler_raw_values), num_devices);
	if (current_values == NULL) {
		PAPIERROR( "malloc():Could not get memory for maxeler raw values structs" );
		//TODO: release socket
		return PAPI_ENOMEM;
	}

	/* we know in advance how many events we want                       */
	/* for actual hardware this might have to be determined dynamically */
	num_events = num_devices * 11;

	/* Allocate memory for the our native event table */
	maxeler_native_table =
		( maxeler_native_event_entry_t * )
		papi_calloc( sizeof(maxeler_native_event_entry_t),num_events);
	if ( maxeler_native_table == NULL ) {
		PAPIERROR( "malloc():Could not get memory for events table" );
		return PAPI_ENOMEM;
	}

	/* fill in the event table parameters */
	/* for complicated components this will be done dynamically */
	/* or by using an external library                          */

	int deviceIx;
	int eventIx = 0;
	for(deviceIx = 0; deviceIx < num_devices; ++deviceIx) {
		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"fpga_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"fpga power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_FPGA_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"vcc1v0_c_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"vcc1v0_c power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_VCC1V0_C_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"vcc1v5_ddr_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"vcc1v5_ddr power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_VCC1V5_DDR_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"vcc2v5_aux_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"vcc2v5_aux power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_VCC2V5_AUX_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"imgt_1v0_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"imgt_1v0 power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_IMGT_1V0_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"imgt_1v2_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"imgt_1v2 power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_IMGT_1V2_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"mgt_1v0_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"mgt_1v0 power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_MGT_1V0_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"mgt_1v2_POWER:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"mgt_1v2 power");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_MGT_1V2_POWER;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"imgt_TEMPERATURE:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"imgt temperature");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_IMGT_TEMPERATURE;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"mmgt_TEMPERATURE:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"mmgt temperature");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_MMGT_TEMPERATURE;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;

		snprintf( maxeler_native_table[eventIx].name, PAPI_MAX_STR_LEN,
			"COMPUTE_UTIL:DEVICE%d", deviceIx);
		snprintf( maxeler_native_table[eventIx].description, PAPI_MAX_STR_LEN,
			"compute utilization");
		maxeler_native_table[eventIx].writable = 0;
		maxeler_native_table[eventIx].event_id = EVENT_COMPUTE_UTILIZATION;
		maxeler_native_table[eventIx].device_id = deviceIx;
		eventIx++;
	}

	/* Export the total number of events available */
	_maxeler_vector.cmp_info.num_native_events = num_events;

	_maxeler_vector.cmp_info.num_cntrs = num_events;
	_maxeler_vector.cmp_info.num_mpx_cntrs = num_events;


	/* Export the component id */
	_maxeler_vector.cmp_info.CmpIdx = cidx;

	

	return PAPI_OK;
}

/** This is called whenever a thread is initialized */
static int
_maxeler_init_thread( hwd_context_t *ctx )
{

	maxeler_context_t *maxeler_context = (maxeler_context_t *)ctx;

	maxeler_context->autoinc_value=0;

	SUBDBG( "_maxeler_init_thread %p...", ctx );

	return PAPI_OK;
}



/** Setup a counter control state.
 *   In general a control state holds the hardware info for an
 *   EventSet.
 */

static int
_maxeler_init_control_state( hwd_control_state_t * ctl )
{
   SUBDBG( "maxeler_init_control_state... %p\n", ctl );

   maxeler_control_state_t *maxeler_ctl = ( maxeler_control_state_t * ) ctl;
   memset( maxeler_ctl, 0, sizeof ( maxeler_control_state_t ) );

   return PAPI_OK;
}


/** Triggered by eventset operations like add or remove */
static int
_maxeler_update_control_state( hwd_control_state_t *ctl, 
				    NativeInfo_t *native,
				    int count, 
				    hwd_context_t *ctx )
{
   
	(void) ctx;
	int i, index;

	maxeler_control_state_t *maxeler_ctl = ( maxeler_control_state_t * ) ctl;

	SUBDBG( "_maxeler_update_control_state %p %p...", ctl, ctx );

	// resize which_counter / counter arrays
	if (count > maxeler_ctl->max_counter || count*2 < maxeler_ctl->max_counter) {
		if (maxeler_ctl->which_counter != NULL) {
			papi_free(maxeler_ctl->which_counter);
		}
		if (maxeler_ctl->counter != NULL) {
			papi_free(maxeler_ctl->counter);
		}
		if (count == 0) {
			maxeler_ctl->max_counter = 0;
		} else {
			if (maxeler_ctl->max_counter == 0) {
				maxeler_ctl->max_counter = 16;
			}
			while (maxeler_ctl->max_counter < count) maxeler_ctl->max_counter *= 2;
			while (maxeler_ctl->max_counter > 2*count) maxeler_ctl->max_counter /= 2;
			maxeler_ctl->which_counter = (int*) papi_calloc( sizeof(int), maxeler_ctl->max_counter);
			maxeler_ctl->counter = (long long*) papi_calloc( sizeof(long long), maxeler_ctl->max_counter);
		}
	}

	/* if no events, return */
	if (count==0) return PAPI_OK;

	maxeler_ctl->event_types = 0;
	for( i = 0; i < count; i++ ) {
	
		index = native[i].ni_event;
      
		// Map counter #i to Measure Event "index"
		maxeler_ctl->which_counter[i]=index;

		// We have no constraints on event position, so any event
		// can be in any slot.                                  
		native[i].ni_position = i;
		switch (index) {
			case EVENT_COMPUTE_UTILIZATION:					
					maxeler_ctl->event_types = maxeler_ctl->event_types | EVENT_TYPE_UTIL;
				break;
			case EVENT_IMGT_TEMPERATURE:
			case EVENT_MMGT_TEMPERATURE:
					maxeler_ctl->event_types = maxeler_ctl->event_types | EVENT_TYPE_TEMPERATURE;
				break;
			default:
					maxeler_ctl->event_types = maxeler_ctl->event_types | EVENT_TYPE_POWER;
				break;
		}
	
	}

	maxeler_ctl->num_events=count;

	return PAPI_OK;
}

/** Triggered by PAPI_start() */
static int
_maxeler_start( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

        (void) ctx;
        (void) ctl;

	SUBDBG( "maxeler_start %p %p...", ctx, ctl );

	/* anything that would need to be set at counter start time */

	/* reset counters? */
        /* For hardware that cannot reset counters, store initial        */
        /*     counter state to the ctl and subtract it off at read time */
	 
	/* start the counting ?*/

	return PAPI_OK;
}


/** Triggered by PAPI_stop() */
static int
_maxeler_stop( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

        (void) ctx;
        (void) ctl;

	SUBDBG( "maxeler_stop %p %p...", ctx, ctl );

	/* anything that would need to be done at counter stop time */

	

	return PAPI_OK;
}


/** Triggered by PAPI_read()     */
/*     flags field is never set? */
static int
_maxeler_read( hwd_context_t *ctx, hwd_control_state_t *ctl,
			  long long **events, int flags )
{

	(void) flags;

	maxeler_control_state_t *maxeler_ctl = ( maxeler_control_state_t *) ctl;

	SUBDBG( "maxeler_read...%d"flags );

	int i;

	if ( EVENT_TYPE_POWER == (maxeler_ctl->event_types & EVENT_TYPE_POWER) ) {
		read_maxeler_power();
	}
	if ( EVENT_TYPE_TEMPERATURE == (maxeler_ctl->event_types & EVENT_TYPE_TEMPERATURE) ) {
		//TODO: Maybe only read temperatures of relevant devices
		read_maxeler_temperature();
	}
	if ( EVENT_TYPE_UTIL == (maxeler_ctl->event_types & EVENT_TYPE_UTIL) ) {
		read_maxeler_utilization();
	}

	int event_id;
	int event_device_id;
	/* Read counters into expected slot */
	for(i=0;i<maxeler_ctl->num_events;i++) {

		event_id = maxeler_ctl->which_counter[i];

		event_device_id = maxeler_native_table[event_id].device_id;

		switch ( maxeler_native_table[event_id].event_id ) {
			case EVENT_FPGA_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].fpga_power * 1000.0);
				break;
			case EVENT_VCC1V0_C_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].vcc1v0_c_power * 1000.0);
				break;
			case EVENT_VCC1V5_DDR_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].vcc1v5_ddr_power * 1000.0);
				break;
			case EVENT_VCC2V5_AUX_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].vcc2v5_aux_power * 1000.0);
				break;
			case EVENT_IMGT_1V0_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].imgt_1v0_power * 1000.0);
				break;
			case EVENT_IMGT_1V2_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].imgt_1v2_power * 1000.0);
				break;
			case EVENT_MGT_1V0_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].mgt_1v0_power * 1000.0);
				break;
			case EVENT_MGT_1V2_POWER:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].mgt_1v2_power * 1000.0);
				break;
			case EVENT_IMGT_TEMPERATURE:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].imgt_temperature * 1000.0);
				break;
			case EVENT_MMGT_TEMPERATURE:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].mmgt_temperature * 1000.0);
				break;
			case EVENT_COMPUTE_UTILIZATION:
					maxeler_ctl->counter[i] = (long long) (current_values[event_device_id].compute_utilization * 1000.0);
				break;	
		}
   }

   /* return pointer to the values we read */
   *events = maxeler_ctl->counter;

   return PAPI_OK;
}

/** Triggered by PAPI_write(), but only if the counters are running */
/*    otherwise, the updated state is written to ESI->hw_start      */
static int
_maxeler_write( hwd_context_t *ctx, hwd_control_state_t *ctl,
			   long long *events )
{

/*
        example_context_t *example_ctx = (example_context_t *) ctx;
        example_control_state_t *example_ctl = ( example_control_state_t *) ctl;   
   
        int i;
   
	SUBDBG( "example_write... %p %p", ctx, ctl );

        // Write counters into expected slot
        for(i=0;i<example_ctl->num_events;i++) {
	   example_hardware_write( example_ctl->which_counter[i],
				   example_ctx,
				   events[i] );
	}
*/ 
	return PAPI_OK;
}


/** Triggered by PAPI_reset() but only if the EventSet is currently running */
/*  If the eventset is not currently running, then the saved value in the   */
/*  EventSet is set to zero without calling this routine.                   */
static int
_maxeler_reset( hwd_context_t *ctx, hwd_control_state_t *ctl )
{
/* 
       example_context_t *event_ctx = (example_context_t *)ctx;
	(void) ctl;

	SUBDBG( "example_reset ctx=%p ctrl=%p...", ctx, ctl );

	// Reset the hardware
	example_hardware_reset( event_ctx );
*/
	return PAPI_OK;
}

/** Triggered by PAPI_shutdown() */
static int
_maxeler_shutdown_component(void)
{

	SUBDBG( "maxeler_shutdown_component..." );

    /* Free anything we allocated */
   
	papi_free(maxeler_native_table);

	papi_free(current_values);

	max_daemon_disconnect(maxelerosd);

	return PAPI_OK;
}

/** Called at thread shutdown */
static int
_maxeler_shutdown_thread( hwd_context_t *ctx )
{

        (void) ctx;

	SUBDBG( "maxeler_shutdown_thread... %p", ctx );

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
_maxeler_ctl( hwd_context_t *ctx, int code, _papi_int_option_t *option )
{

        (void) ctx;
	(void) code;
	(void) option;

	SUBDBG( "maxeler_ctl..." );

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
_maxeler_set_domain( hwd_control_state_t * cntrl, int domain )
{
        (void) cntrl;

	int found = 0;
	SUBDBG( "maxeler_set_domain..." );

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
_maxeler_ntv_enum_events( unsigned int *EventCode, int modifier )
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
_maxeler_ntv_code_to_name( unsigned int EventCode, char *name, int len )
{
  int index;

  index = EventCode;

  /* Make sure we are in range */
  if (index >= 0 && index < num_events) {
     strncpy( name, maxeler_native_table[index].name, len );  
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
_maxeler_ntv_code_to_descr( unsigned int EventCode, char *descr, int len )
{
  int index;
  index = EventCode;

  /* make sure event is in range */
  if (index >= 0 && index < num_events) {
     strncpy( descr, maxeler_native_table[index].description, len );
     return PAPI_OK;
  }
  
  return PAPI_ENOEVNT;
}

/** Vector that points to entry points for our component */
papi_vector_t _maxeler_vector = {
	.cmp_info = {
		/* default component information */
		/* (unspecified values are initialized to 0) */
                /* we explicitly set them to zero in this example */
                /* to show what settings are available            */

		.name = "maxeler",
		.short_name = "maxeler",
		.description = "Maxeler MAX3a component",
		.version = "0.5",
		.support_version = "n/a",
		.kernel_version = "n/a",
//		.num_cntrs =               EXAMPLE_MAX_SIMULTANEOUS_COUNTERS, 
//		.num_mpx_cntrs =           EXAMPLE_MAX_SIMULTANEOUS_COUNTERS,
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
		.context = sizeof ( maxeler_context_t ),
	        /* once per eventset */
		.control_state = sizeof ( maxeler_control_state_t ),
	        /* ?? */
		.reg_value = sizeof ( maxeler_register_t ),
	        /* ?? */
		.reg_alloc = sizeof ( maxeler_reg_alloc_t ),
	},

	/* function pointers */
        /* by default they are set to NULL */
   
	/* Used for general PAPI interactions */
	.start =                _maxeler_start,
	.stop =                 _maxeler_stop,
	.read =                 _maxeler_read,
	.reset =                _maxeler_reset,	
	.write =                _maxeler_write,
	.init_component =       _maxeler_init_component,	
	.init_thread =          _maxeler_init_thread,
	.init_control_state =   _maxeler_init_control_state,
	.update_control_state = _maxeler_update_control_state,	
	.ctl =                  _maxeler_ctl,	
	.shutdown_thread =      _maxeler_shutdown_thread,
	.shutdown_component =   _maxeler_shutdown_component,
	.set_domain =           _maxeler_set_domain,
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
	.ntv_enum_events =   _maxeler_ntv_enum_events,
	.ntv_code_to_name =  _maxeler_ntv_code_to_name,
	.ntv_code_to_descr = _maxeler_ntv_code_to_descr,
        /* if .ntv_name_to_code not available, PAPI emulates  */
        /* it by enumerating all events and looking manually  */
   	.ntv_name_to_code  = NULL,

   
	/* These are only used by _papi_hwi_get_native_event_info() */
	/* Which currently only uses the info for printing native   */
	/* event info, not for any sort of internal use.            */
	/* .ntv_code_to_bits =  NULL, */

};

