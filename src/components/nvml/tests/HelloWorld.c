/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/** 
 * @file    HelloWorld.c
 * CVS:     $Id$
 * @author  Heike Jagode
 *          jagode@eecs.utk.edu
 * Mods:	<your name here>
 *			<your email address>
 * test case for Example component 
 * 
 *
 * @brief
 *  This file is a very simple HelloWorld C example which serves (together
 *	with its Makefile) as a guideline on how to add tests to components.
 *  The papi configure and papi Makefile will take care of the compilation
 *	of the component tests (if all tests are added to a directory named
 *	'tests' in the specific component dir).
 *	See components/README for more details.
 *
 *	The string "Hello World!" is mangled and then restored.
 */

#include <stdio.h>
#include "papi_test.h"

#define NUM_EVENTS 6
#define PAPI


// Host function
int main(int argc, char** argv)
{
#ifdef PAPI
	int retval, i;
	int EventSet = PAPI_NULL;
	long long values[NUM_EVENTS];
	/* REPLACE THE EVENT NAME 'PAPI_FP_OPS' WITH A CUDA EVENT 
	   FOR THE CUDA DEVICE YOU ARE RUNNING ON.
	   RUN papi_native_avail to get a list of CUDA events that are 
	   supported on your machine */
    char *EventName[] = { "nvml:::Tesla_K20c:power", "nvml:::Tesla_K20c:fan_speed", "nvml:::Tesla_K20c:total_memory", "nvml:::Tesla_K20c:unallocated_memory", "nvml:::Tesla_K20c:allocated_memory", "nvml:::Tesla_K20c:temperature" };
	int events[NUM_EVENTS];
	int eventCount = 0;
	
	/* PAPI Initialization */
	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if( retval != PAPI_VER_CURRENT )
		fprintf( stderr, "PAPI_library_init failed\n" );
	
	printf( "PAPI_VERSION     : %4d %6d %7d\n",
			PAPI_VERSION_MAJOR( PAPI_VERSION ),
			PAPI_VERSION_MINOR( PAPI_VERSION ),
			PAPI_VERSION_REVISION( PAPI_VERSION ) );
	
	/* convert PAPI native events to PAPI code */
	for( i = 0; i < NUM_EVENTS; i++ ){
		retval = PAPI_event_name_to_code( EventName[i], &events[i] );
		if( retval != PAPI_OK ) {
			fprintf( stderr, "PAPI_event_name_to_code failed\n" );
			continue;
		}
		eventCount++;
			printf( "Name %s --- Code: %#x\n", EventName[i], events[i] );
	}

	/* if we did not find any valid events, just report test failed. */
	if (eventCount == 0) {
		printf( "Test FAILED: no valid events found.\n");
		return 1;
	}
	
	retval = PAPI_create_eventset( &EventSet );
	if( retval != PAPI_OK )
		fprintf( stderr, "PAPI_create_eventset failed\n" );
	
	retval = PAPI_add_events( EventSet, events, eventCount );
	if( retval != PAPI_OK )
		fprintf( stderr, "PAPI_add_events failed\n" );
#endif


	int j;
	int count;
	int cuda_device;

	for ( cuda_device = 0; cuda_device < 1; cuda_device++ ) {
#ifdef PAPI	
			retval = PAPI_start( EventSet );
			if( retval != PAPI_OK )
					fprintf( stderr, "PAPI_start failed\n" );
#endif

			// desired output
			char str[] = "Hello World!";

			// mangle contents of output
			// the null character is left intact for simplicity
			for(j = 0; j < 12; j++) {
					str[j] -= j;
					//printf("str=%s\n", str);
			}

            sleep(10);

			// free up the allocated memory on the device

			printf("END: %s\n", str);


#ifdef PAPI
			retval = PAPI_stop( EventSet, values );
			if( retval != PAPI_OK )
					fprintf( stderr, "PAPI_stop failed\n" );

			for( i = 0; i < eventCount; i++ )
					printf( "On device %d: %12lld \t\t --> %s \n", cuda_device, values[i], EventName[i] );
#endif
	}

	return 0;
}



