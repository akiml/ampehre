/*                                                                                                                                              
 * atime_papi.h
 * 
 * Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Alex Wiens <awiens@mail.uni-paderborn.de>  * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Alex Wiens (awiens@mail.uni-paderborn.de)
 * created: 6/01/17
 * version: 0.8.0 - initial implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "papi.h"
#include "apapi.h"

// known components
/*
char *known_components[] = {
	"rapl",
	"nvml",
	"maxeler",
	"micknc",
	"ipmi",
	NULL
};
*/

//char *known_cmplist = "rapl nvml maxeler micknc ipmi";

// available components
char **components;
int cmp_count = 0;

// sets and timers
struct apapi_eventset **sets = NULL;
struct apapi_timer **timers = NULL;

// user event list
char ***user_eventlist_sorted = NULL;
char *user_eventlist_file = NULL;
char **user_eventlist_components = NULL;

// user event defaults
struct apapi_event_ops *user_eventops = NULL;
char *user_eventops_file = NULL;
int user_eventops_num = 0;

// user component list
char *user_cmplist_buffer = NULL;
char **user_cmplist = NULL;

void measure_custom(void** arg, int last_measurement){

	if (arg == NULL) {
		return;
	}
	struct apapi_eventset *set;
	set = (struct apapi_eventset *) *arg;
	int eventIx;
	for (eventIx = 0; eventIx < set->num_events; eventIx++) {
		printf("%lld ", set->current_time);
		printf("%s ", set->event_ops[eventIx].event_name);
		printf("%lld ", set->current_samples[eventIx]);
		printf("%f\n", set->values0[eventIx*4 + 3]);
	}

}

void quit(unsigned int line, char* msg){
	printf("%d %s\n", line, msg);
	exit(EXIT_FAILURE);
}

long gettime() {
	struct timespec result;
	clock_gettime(CLOCK_MONOTONIC_RAW, &result);
	return result.tv_sec* 1000000000 + result.tv_nsec;
}

/**
 *	Expected format:
 *	name:number:name:number:name:number
 */
void get_interval_option(const char *cmp, char *option_intervalstr, time_t *sec, long *nsec) {

	*sec = 0;
	*nsec = 100000000;

	if (NULL == option_intervalstr) {
		return;
	}

	// search for component name
	char *cmp_pos;
	cmp_pos = strstr(option_intervalstr, cmp);
	if (NULL == cmp_pos) {
		return;
	}
	size_t cmp_len = strlen(cmp);
	// step forward to delimiter
	cmp_pos += cmp_len; // whole component name was found, this should be ok
	if (*cmp_pos != ':' || *cmp_pos == 0) {
		// delimiter not found
		printf("delimiter not found\n");
		return;
	}
	cmp_pos += 1;
	if (*cmp_pos == 0) {
		printf("missing number");
		return;
	}
	char *endptr = NULL;
	long long int number;
	printf("%s %d\n", cmp_pos, errno);
	number = strtoll(cmp_pos, &endptr, 10);
	if (NULL == endptr || (':' != *endptr && 0 != *endptr)) {
		// invalid number
		printf("invalid number %lld\n", number);
		return;
	}
	if (number < 10000000) {
		printf("invalid number %lld\n", number);
		return;
	}
	if (number < 1000000000L) {
		*nsec = number;
	} else {
		*sec = number / 1000000000L;
		*nsec = number - (*sec * 1000000000L);
	}
}

void papi_init(char *option_intervalstr){

	int retv;

	APAPI_read_env_eventops(&user_eventops_file, &user_eventops, &user_eventops_num);

	retv = APAPI_init();
	if (retv != PAPI_VER_CURRENT) {
		quit(__LINE__, "Failed to initialize PAPI library");
	}

	retv = APAPI_read_env_cmplist(&user_cmplist_buffer, &user_cmplist);

	char **cmplist;
	if (user_cmplist != NULL) {
		cmplist = user_cmplist;
	} else {
		cmplist = APAPI_default_components;
	}

	int untested_cmp_count = 0;
	for(untested_cmp_count = 0; cmplist[untested_cmp_count] != NULL; ++untested_cmp_count);
	components = calloc(untested_cmp_count+1, sizeof(char*)); // +1 for NULL pointer at the end

	// check if cmplist components are not compiled into PAPI
	int i;
	for(i=0; i<untested_cmp_count; ++i) {
		if (PAPI_get_component_index(cmplist[i]) == PAPI_ENOCMP) {
			printf("Component %s is not available in PAPI. Check your PAPI lib.\n", cmplist[i]);
			exit(1);
		}
	}

	// check available papi components
	int num_components = PAPI_num_components();
	const PAPI_component_info_t *component_infos[num_components];
	int in_cmplist = 0;
	cmp_count = 0;
	int cmplist_cmpIx = 0;
	for(i=0; i<num_components; i++) {
		component_infos[i] = PAPI_get_component_info(i);

        in_cmplist = 1;
		cmplist_cmpIx = APAPI_cmp_cmplist_index(component_infos[i]->short_name, cmplist);
		if (cmplist_cmpIx == -1) {
			in_cmplist = 0;
		}

		if (component_infos[i]->disabled != 0) {
			printf("PAPI component %s disabled. Reason: %s\n", component_infos[i]->short_name, component_infos[i]->disabled_reason);
			if (in_cmplist == 1) {
				printf("PAPI component %s should have been available\n", component_infos[i]->short_name);
				exit(1);
			}
			continue;
		}

		if (in_cmplist == 0) {
			continue;
		}

		//printf("%d: %s %s %d\n", i, component_infos[i]->short_name, component_infos[i]->name, component_infos[i]->num_native_events);
		// add component to list of usable components
		components[cmp_count] = cmplist[cmplist_cmpIx];
		cmp_count++;
	}


	// check user selected events from eventlist
	APAPI_read_env_eventlist(components, &user_eventlist_file, &user_eventlist_sorted, &user_eventlist_components);

	// create apapi_event_set array
	// one set per component
	sets = calloc(sizeof(struct apapi_eventset *), cmp_count);
	timers = calloc(sizeof(struct apapi_timer *), cmp_count);
	uint32_t setIx = 0;
	int cidx;
	char ** cmp_events = NULL;
	int eventlist_cmpIx = 0;
	time_t sec;
	long nsec;
	for(setIx = 0; setIx < cmp_count; ++setIx) {
		cidx = PAPI_get_component_index(components[setIx]);
		if (NULL != user_eventlist_sorted) {
			eventlist_cmpIx = APAPI_cmp_cmplist_index(components[setIx], user_eventlist_components);
			if (eventlist_cmpIx < 0) {
				cmp_events = NULL;
			} else {
				cmp_events = user_eventlist_sorted[eventlist_cmpIx];
			}
		}

		printf("Use PAPI component %s\n", components[setIx]);
		retv = APAPI_init_apapi_eventset_cmp(&(sets[setIx]), cidx, cmp_events, user_eventops);
		if (PAPI_OK != retv) {
			printf("Failed to create eventset for component %s\n", components[setIx]);
			exit(1);
		}
		#ifdef DEBUG
		printf("atime %d %d\n", __LINE__, retv);
		#endif

		get_interval_option(components[setIx], option_intervalstr, &sec, &nsec);

		#ifdef DEBUG
		retv = APAPI_create_timer(&(timers[setIx]), sec, nsec, measure_custom, (void**) &(sets[setIx]), sets[setIx]);
		#else
		retv = APAPI_create_timer(&(timers[setIx]), sec, nsec, NULL, NULL, sets[setIx]);
		#endif
		#ifdef DEBUG
		printf("atime %d %d\n", __LINE__, retv);
		#endif
		printf("atime cmp %s event_num %d interval %ld.%ld\n", sets[setIx]->cmp_name, sets[setIx]->num_events, sec, nsec);
	}
	if (cmp_count == 0) {
		printf("No component active.\n");
		exit();
	}
}

void papi_start(){
	printf("papi_start\n");
	int retv;
	int setIx = 0;
	for(setIx = 0; setIx < cmp_count; setIx++){
		retv = APAPI_initstart_timer(timers[setIx]);
	}
	for(setIx = 0; setIx < cmp_count; setIx++){
		retv = APAPI_start_timer(timers[setIx]);
		#ifdef DEBUG
		printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
		#endif
	}
	(void) retv;
}

void papi_stop(){
	int retv;
	int setIx = 0;
	for(setIx = 0; setIx < cmp_count; setIx++){
		retv = APAPI_stop_timer(timers[setIx]);
		#ifdef DEBUG
		printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
		#endif
	}

	for(setIx = 0; setIx < cmp_count; setIx++){
		retv = APAPI_destroy_timer(&(timers[setIx]));
		#ifdef DEBUG
		printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
		#endif
	}

	for(setIx = 0; setIx < cmp_count; setIx++){
		APAPI_print_apapi_eventset(sets[setIx]);

		retv = APAPI_destroy_apapi_eventset(&(sets[setIx]));
		#ifdef DEBUG
		printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
		#endif
	}
 
	PAPI_shutdown();
	printf("papi stop\n");
	(void) retv;
}


