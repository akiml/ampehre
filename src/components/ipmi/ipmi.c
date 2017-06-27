/**
 * @file    ipmi.c
 * @author  Alex Wiens
 *
 * @ingroup papi_components
 *
 * @brief
 *  This component uses the Ampehre libipmimeasure library to
 *  query system statistics.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ms_ipmiwrapper.h"

/* Headers required by PAPI */
#include "papi.h"
#include "papi_internal.h"
#include "papi_vector.h"
#include "papi_memory.h"    /* defines papi_malloc(), etc. */


#define BOARD_POWER 	0
#define SYSTEM_POWER 	1
#define SYSTEM_TEMP 	2
#define NUM_EVENTS		3

/* Declare our vector in advance */
/* This allows us to modify the component info */
papi_vector_t _ipmi_vector;

/** Structure that stores private information for each event */
typedef struct ipmi_register
{
} ipmi_register_t;

/** This structure is used to build the table of events  */
/*   The contents of this structure will vary based on   */
/*   your component, however having name and description */
/*   fields are probably useful.                         */
typedef struct ipmi_native_event_entry
{
	ipmi_register_t resources;	    /**< Per counter resources       */
	char name[PAPI_MAX_STR_LEN];	    /**< Name of the counter         */
	char description[PAPI_MAX_STR_LEN]; /**< Description of the counter  */
	int writable;			    /**< Whether counter is writable */
	/* any other counter parameters go here */
	int event_id;
} ipmi_native_event_entry_t;

/** This structure is used when doing register allocation 
    it possibly is not necessary when there are no 
    register constraints */
typedef struct ipmi_reg_alloc
{
	ipmi_register_t ra_bits;
} ipmi_reg_alloc_t;

/** Holds control flags.  
 *    There's one of these per event-set.
 *    Use this to hold data specific to the EventSet, either hardware
 *      counter settings or things like counter start values 
 */
typedef struct ipmi_control_state
{
  int num_events;
  int domain;
  int multiplexed;
  int overflow;
  int inherit;
  int which_counter[NUM_EVENTS];
  long long counter[NUM_EVENTS];   /**< Copy of counts, holds results when stopped */
} ipmi_control_state_t;

/** Holds per-thread information */
typedef struct ipmi_context
{
     long long autoinc_value;
} ipmi_context_t;

/** This table contains the native events */
static ipmi_native_event_entry_t *ipmi_native_table;

/** number of events in the table*/
static int num_events = 0;




/*************************************************************************/
/* Below is the actual "hardware implementation" of our ipmi counters */
/*************************************************************************/

long long ipmi_read_system_power(){
	uint16_t result_power_current;
	dellCurrentPower(&result_power_current);
	return result_power_current;
}
long long ipmi_read_board_power(){
	double value;
	value = getPower(90);
	return (long long)value;
}
long long ipmi_read_system_temp(){
	return getTemperature(18);
}



/********************************************************************/
/* Below are the functions required by the PAPI component interface */
/********************************************************************/


/** Initialize hardware counters, setup the function vector table
 * and get hardware information, this routine is called when the
 * PAPI process is initialized (IE PAPI_library_init)
 */
static int
_ipmi_init_component( int cidx )
{

	SUBDBG( "_ipmi_init_component..." );

   
	/* First, detect that our hardware is available */
	MS_VERSION version;
	version.major = MS_MAJOR_VERSION;
	version.minor = MS_MINOR_VERSION;
	version.revision = MS_REVISION_VERSION;
	init_ipmi_wrapper(&version);
	// in case of failure the program will exit at this point

	num_events = NUM_EVENTS;

	/* Allocate memory for the our native event table */
	ipmi_native_table =
		( ipmi_native_event_entry_t * )
		papi_calloc( sizeof(ipmi_native_event_entry_t),num_events);
	if ( ipmi_native_table == NULL ) {
		PAPIERROR( "malloc():Could not get memory for events table" );
		return PAPI_ENOMEM;
	}

	/* fill in the event table parameters */
	/* for complicated components this will be done dynamically */
	/* or by using an external library                          */

	strcpy( ipmi_native_table[0].name, "BOARD_POWER" );
	strcpy( ipmi_native_table[0].description,
			"" );
	ipmi_native_table[0].writable = 0;
	ipmi_native_table[0].event_id = BOARD_POWER;

	strcpy( ipmi_native_table[1].name, "SYSTEM_POWER" );
	strcpy( ipmi_native_table[1].description,
			"" );
	ipmi_native_table[1].writable = 0;
	ipmi_native_table[1].event_id = SYSTEM_POWER;

	strcpy( ipmi_native_table[2].name, "SYSTEM_TEMP" );
	strcpy( ipmi_native_table[2].description,
			"" );
	ipmi_native_table[2].writable = 0;
	ipmi_native_table[2].event_id = SYSTEM_TEMP;


	/* Export the total number of events available */
	_ipmi_vector.cmp_info.num_native_events = num_events;

	/* Export the component id */
	_ipmi_vector.cmp_info.CmpIdx = cidx;

    _ipmi_vector.cmp_info.num_cntrs = num_events;
    _ipmi_vector.cmp_info.num_mpx_cntrs = num_events;
	

	return PAPI_OK;
}

/** This is called whenever a thread is initialized */
static int
_ipmi_init_thread( hwd_context_t *ctx )
{

        ipmi_context_t *ipmi_context = (ipmi_context_t *)ctx;

        ipmi_context->autoinc_value=0;
   
	SUBDBG( "_ipmi_init_thread %p...", ctx );

	return PAPI_OK;
}



/** Setup a counter control state.
 *   In general a control state holds the hardware info for an
 *   EventSet.
 */

static int
_ipmi_init_control_state( hwd_control_state_t * ctl )
{
   SUBDBG( "ipmi_init_control_state... %p\n", ctl );

   ipmi_control_state_t *ipmi_ctl = ( ipmi_control_state_t * ) ctl;
   memset( ipmi_ctl, 0, sizeof ( ipmi_control_state_t ) );

   return PAPI_OK;
}


/** Triggered by eventset operations like add or remove */
static int
_ipmi_update_control_state( hwd_control_state_t *ctl, 
				    NativeInfo_t *native,
				    int count, 
				    hwd_context_t *ctx )
{
   
   (void) ctx;
   int i, index;

   ipmi_control_state_t *ipmi_ctl = ( ipmi_control_state_t * ) ctl;   

   SUBDBG( "_ipmi_update_control_state %p %p...", ctl, ctx );

   /* if no events, return */
   if (count==0) return PAPI_OK;


	for( i = 0; i < count; i++ ) {
		index = native[i].ni_event;

		native[i].ni_position = i;

		ipmi_ctl->which_counter[i] = index;
	}


   ipmi_ctl->num_events=count;

   return PAPI_OK;
}

/** Triggered by PAPI_start() */
static int
_ipmi_start( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

        (void) ctx;
        (void) ctl;

	SUBDBG( "ipmi_start %p %p...", ctx, ctl );

	/* anything that would need to be set at counter start time */

	/* reset counters? */
        /* For hardware that cannot reset counters, store initial        */
        /*     counter state to the ctl and subtract it off at read time */
	 
	/* start the counting ?*/

	dellResetEnergyCounter();
/*
	// ?
	int i;
	for(i=0; i<5; i++){
		getTemperature(18);
		getPower(90);
		getTemperature(153);
		getTemperature(154);
	}
*/

	return PAPI_OK;
}


/** Triggered by PAPI_stop() */
static int
_ipmi_stop( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

        (void) ctx;
        (void) ctl;

	SUBDBG( "ipmi_stop %p %p...", ctx, ctl );

	/* anything that would need to be done at counter stop time */

	

	return PAPI_OK;
}


/** Triggered by PAPI_read()     */
/*     flags field is never set? */
static int
_ipmi_read( hwd_context_t *ctx, hwd_control_state_t *ctl,
			  long long **events, int flags )
{

	(void) ctx;
	(void) flags;

	ipmi_control_state_t *ipmi_ctl = ( ipmi_control_state_t *) ctl;

	SUBDBG( "ipmi_read...%d", flags );

	int i;
	int event_id;

	/* Read counters into expected slot */
	for(i=0;i<ipmi_ctl->num_events;i++) {
	
		event_id = ipmi_ctl->which_counter[i];

		switch(event_id) {
			case BOARD_POWER:
				ipmi_ctl->counter[i] = ipmi_read_board_power();
			break;
			case SYSTEM_POWER:
				ipmi_ctl->counter[i] = ipmi_read_system_power();
			break;
			case SYSTEM_TEMP:
				ipmi_ctl->counter[i] = ipmi_read_system_temp();
			break;
		}
	}

	/* return pointer to the values we read */
	*events = ipmi_ctl->counter;
	return PAPI_OK;
}

/** Triggered by PAPI_write(), but only if the counters are running */
/*    otherwise, the updated state is written to ESI->hw_start      */
static int
_ipmi_write( hwd_context_t *ctx, hwd_control_state_t *ctl,
			   long long *events )
{

	(void) ctx;
	(void) ctl;
	(void) events;
	SUBDBG( "ipmi_write...");

	return PAPI_OK;
}


/** Triggered by PAPI_reset() but only if the EventSet is currently running */
/*  If the eventset is not currently running, then the saved value in the   */
/*  EventSet is set to zero without calling this routine.                   */
static int
_ipmi_reset( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

	(void) ctx;
	(void) ctl;
	SUBDBG( "ipmi_reset...");

	return PAPI_OK;
}

/** Triggered by PAPI_shutdown() */
static int
_ipmi_shutdown_component(void)
{

	SUBDBG( "ipmi_shutdown_component..." );

        /* Free anything we allocated */
   
	close_ipmi_wrapper();

	papi_free(ipmi_native_table);

	return PAPI_OK;
}

/** Called at thread shutdown */
static int
_ipmi_shutdown_thread( hwd_context_t *ctx )
{

        (void) ctx;

	SUBDBG( "ipmi_shutdown_thread... %p", ctx );

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
_ipmi_ctl( hwd_context_t *ctx, int code, _papi_int_option_t *option )
{

        (void) ctx;
	(void) code;
	(void) option;

	SUBDBG( "ipmi_ctl..." );

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
_ipmi_set_domain( hwd_control_state_t * cntrl, int domain )
{
        (void) cntrl;

	int found = 0;
	SUBDBG( "ipmi_set_domain..." );

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
_ipmi_ntv_enum_events( unsigned int *EventCode, int modifier )
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
_ipmi_ntv_code_to_name( unsigned int EventCode, char *name, int len )
{
  int index;

  index = EventCode;

  /* Make sure we are in range */
  if (index >= 0 && index < num_events) {
     strncpy( name, ipmi_native_table[index].name, len );  
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
_ipmi_ntv_code_to_descr( unsigned int EventCode, char *descr, int len )
{
  int index;
  index = EventCode;

  /* make sure event is in range */
  if (index >= 0 && index < num_events) {
     strncpy( descr, ipmi_native_table[index].description, len );
     return PAPI_OK;
  }
  
  return PAPI_ENOEVNT;
}

/** Vector that points to entry points for our component */
papi_vector_t _ipmi_vector = {
	.cmp_info = {
		/* default component information */
		/* (unspecified values are initialized to 0) */
                /* we explicitly set them to zero in this ipmi */
                /* to show what settings are available            */

		.name = "ipmi",
		.short_name = "ipmi",
		.description = "A simple ipmi component",
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
		.context = sizeof ( ipmi_context_t ),
	        /* once per eventset */
		.control_state = sizeof ( ipmi_control_state_t ),
	        /* ?? */
		.reg_value = sizeof ( ipmi_register_t ),
	        /* ?? */
		.reg_alloc = sizeof ( ipmi_reg_alloc_t ),
	},

	/* function pointers */
        /* by default they are set to NULL */
   
	/* Used for general PAPI interactions */
	.start =                _ipmi_start,
	.stop =                 _ipmi_stop,
	.read =                 _ipmi_read,
	.reset =                _ipmi_reset,	
	.write =                _ipmi_write,
	.init_component =       _ipmi_init_component,	
	.init_thread =          _ipmi_init_thread,
	.init_control_state =   _ipmi_init_control_state,
	.update_control_state = _ipmi_update_control_state,	
	.ctl =                  _ipmi_ctl,	
	.shutdown_thread =      _ipmi_shutdown_thread,
	.shutdown_component =   _ipmi_shutdown_component,
	.set_domain =           _ipmi_set_domain,
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
	.ntv_enum_events =   _ipmi_ntv_enum_events,
	.ntv_code_to_name =  _ipmi_ntv_code_to_name,
	.ntv_code_to_descr = _ipmi_ntv_code_to_descr,
        /* if .ntv_name_to_code not available, PAPI emulates  */
        /* it by enumerating all events and looking manually  */
   	.ntv_name_to_code  = NULL,

   
	/* These are only used by _papi_hwi_get_native_event_info() */
	/* Which currently only uses the info for printing native   */
	/* event info, not for any sort of internal use.            */
	/* .ntv_code_to_bits =  NULL, */

};

