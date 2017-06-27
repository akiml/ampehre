/*                                                                                                                                              
 * apapi.c
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
#include <stdlib.h> //calloc
#include <errno.h> //ETIMEDOUT
#include <string.h> //memset
#include <float.h> //DBL_MAX
#include <unistd.h>
#include "apapi.h"
#include "papi.h"
//#define DEBUG
#define APAPI_PRINTERR(...) fprintf(stderr, "APAPI %s:%d ", __FILE__, __LINE__);fprintf(stderr, __VA_ARGS__);
#define APAPI_PRINT(...) fprintf(stdout, "APAPI: ");fprintf(stdout, __VA_ARGS__);


int _apapi_initialized = 0;

// set with APAPI_VERBOSE env variable
// 0 = default, only errors
// 1 = additional prints
int _apapi_verbose = 0;

// set with APAPI_SOFTFAIL env variable
// 0 = default, exit on every error
// 1 = soft, print error messages but carry on
int _apapi_softfail = 0;

// default op to use if no event definition found
struct apapi_event_ops _apapi_default_op = {"default", APAPI_OP1_NOP, APAPI_STAT_ALL, APAPI_STAT_NO, 0, "unknown", "unknown", 1, NULL, NULL, 1};
// default PAPI components
char *APAPI_default_components[] = {
	"rapl",
	"nvml",
	"maxeler",
	"ipmi",
	"micknc",
	NULL
};

// default event operations
struct apapi_event_ops *_apapi_default_eventops = NULL;
char *_apapi_default_eventops_file_buffer;
int _apapi_default_eventops_num_events;
// default event list
char *_apapi_default_eventlist_file_buffer;
char ***_apapi_default_eventlist_sorted;
char **_apapi_default_eventlist_cmp;

#include "apapi_defaults.c"
#include "apapi_op1.c"


int APAPI_init() {

	int retval;

	if (0 == _apapi_initialized) {
		// search for APAPI_VERBOSE variable
		int envIx;
		char *env_variable = NULL;
		for (envIx = 0; environ[envIx] != NULL; ++envIx) {
			if (strncmp(environ[envIx], "APAPI_VERBOSE=", 14) == 0) {
				env_variable = environ[envIx];
				break;
			}
		}

		// APAPI_VERBOSE variable found
		if (env_variable != NULL) {
			long int v;
			char *endptr;
			v = strtol(&(env_variable[14]), &endptr, 0);
			if (endptr != &(env_variable[14]) && *endptr == 0) {
				_apapi_verbose = v;
			}
		}

		// search for APAPI_SOFTFAIL variable
		env_variable = NULL;
		for (envIx = 0; environ[envIx] != NULL; ++envIx) {
			if (strncmp(environ[envIx], "APAPI_SOFTFAIL=", 15) == 0) {
				env_variable = environ[envIx];
				break;
			}
		}

		// APAPI_SOFTFAIL variable found
		if (env_variable != NULL) {
			long int v;
			char *endptr;
			v = strtol(&(env_variable[15]), &endptr, 0);
			if (endptr != &(env_variable[15]) && *endptr == 0) {
				_apapi_softfail = v;
			}
		}


		// load default files
		char filename_buffer[50];
		memset(filename_buffer, 0, 50);
		int pathsize;

		// load default event definitions
		pathsize = snprintf(filename_buffer, 50, "%s/apapi/default_eventops.csv", DATADIR);
		if (pathsize <= 50) {
			// constructing pathname successfull
			retval = _apapi_read_eventops_file(filename_buffer, &_apapi_default_eventops, &_apapi_default_eventops_file_buffer, &_apapi_default_eventops_num_events);
			if (PAPI_OK != retval) {
				APAPI_PRINTERR("default event operations not found\n")
				if (0 == _apapi_softfail) {
					return -1;
				}
			} else {
				if (1 == _apapi_verbose) {
					APAPI_PRINT("Read default event operations from %s\n", filename_buffer)
				}
			}
		}

		// load default event list
		pathsize = snprintf(filename_buffer, 50, "%s/apapi/default_eventlist.txt", DATADIR);
		if (pathsize <= 50) {
			retval = _apapi_read_eventlist_file(filename_buffer, APAPI_default_components, &_apapi_default_eventlist_file_buffer, &_apapi_default_eventlist_sorted, &_apapi_default_eventlist_cmp);
			if (PAPI_OK != retval) {
				APAPI_PRINTERR("default event list not found\n")
				if (0 == _apapi_softfail) {
					return -1;
				}
			} else {
				if (1 == _apapi_verbose) {
					APAPI_PRINT("Read default event list from %s\n", filename_buffer)
				}
			}
		}
		_apapi_initialized = 1;
	}
	
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	return retval;
};

void APAPI_destroy() {

	if (0 != _apapi_initialized) {
		// free default event definitions
		if (NULL != _apapi_default_eventops) {
			free(_apapi_default_eventops);
			_apapi_default_eventops = NULL;
			free(_apapi_default_eventops_file_buffer);
			_apapi_default_eventops_file_buffer = NULL;
			_apapi_default_eventops_num_events = 0;
		}

		// free default event list
		if (NULL != _apapi_default_eventlist_file_buffer) {
			free(_apapi_default_eventlist_file_buffer);
			_apapi_default_eventlist_file_buffer = NULL;
			_apapi_free_sorted_eventlist(&_apapi_default_eventlist_cmp, &_apapi_default_eventlist_sorted);
		}
		_apapi_initialized = 0;
	}
	
	PAPI_shutdown();
}

/** Internal
 *	@class _apapi_getcurrenttime
 *	@brief Returns time from clock_gettime with seconds and nanoseconds combined as long long
 *
 *	@retval linux current time as long long
 */
long long _apapi_getcurrenttime() {
	struct timespec result;
	clock_gettime(CLOCK_MONOTONIC_RAW, &result);
	return (long long)result.tv_sec* 1000000000LL + (long long)result.tv_nsec;
}

int APAPI_create_eventset_list(char **events, int cidx, int *EventSet, int *num_events){

	int retv;

	// create new PAPI EventSet
	retv = PAPI_create_eventset(EventSet);
	if (PAPI_OK != retv) {
		return retv;
	}

	// assign PAPI EventSet to component
	retv = PAPI_assign_eventset_component(*EventSet, cidx);
	if (PAPI_OK != retv) {
		APAPI_PRINTERR("Failed to assign event set to component %d\n", cidx)
		if (0 == _apapi_softfail) {
			PAPI_destroy_eventset(EventSet);
			return -1;
		}
	}

	// add events to PAPI EventSet
	int eventIx;
	*num_events = 0;
	for(eventIx = 0; events[eventIx] != NULL; ++eventIx) {
		retv = PAPI_add_named_event(*EventSet, events[eventIx]);
		if (PAPI_OK == retv) {
			(*num_events)++;
		} else {
			APAPI_PRINTERR("Failed to add event \"%s\" to EventSet.\n", events[eventIx])
			if (0 == _apapi_softfail) {
				PAPI_destroy_eventset(EventSet);
				return -1;
			}
		}
	}

	return PAPI_OK;
}

// last_measurement: true = 1, false = 0
// last_measurement: true = 1, false = 0
int APAPI_create_eventset_cmp_all(int cidx, int *EventSet, int *num_events) {

	int retv;

	// create new event set
	retv = PAPI_create_eventset(EventSet);
	#ifdef DEBUG
		APAPI_PRINT("%d %d %x\n", __LINE__, retv, EventSet)
	#endif
	if (PAPI_OK != retv) {
		return retv;
	}

	// assign PAPI EventSet to component
	retv = PAPI_assign_eventset_component(*EventSet, cidx);
	if (PAPI_OK != retv) {
		APAPI_PRINTERR("Failed to assign event set to component %d\n", cidx)
		if (0 == _apapi_softfail) {
			PAPI_destroy_eventset(EventSet);
			return -1;
		}
	}

	// iterate over component's events and add event to event set
	int EventCode = 0 | PAPI_NATIVE_MASK;
	*num_events = 0;
	// get first event code
	retv = PAPI_enum_cmp_event(&EventCode, PAPI_ENUM_FIRST, cidx);
	#ifdef DEBUG
		APAPI_PRINT("%d %d %d\n", __LINE__, retv, cidx)
	#endif
	while (PAPI_OK == retv) {
		(*num_events)++;
		retv = PAPI_add_event(*EventSet, EventCode);
		#ifdef DEBUG
			APAPI_PRINT("%d %d\n", __LINE__, retv)
		#endif
		// error on adding
		if (retv != PAPI_OK) {
			APAPI_PRINTERR("Failed to add event %d to event set for component %d, errorcode %d\n", EventCode, cidx, retv)
			if (0 == _apapi_softfail) {
				PAPI_destroy_eventset(EventSet);
				return -1;
			}
			break;
		}
		// get next event code
		retv = PAPI_enum_cmp_event(&EventCode, PAPI_ENUM_EVENTS, cidx);
		#ifdef DEBUG
			APAPI_PRINT("%d %d\n", __LINE__, retv)
		#endif
	}

	// in case of error (other than end of event enumeration)
	/*
	// This will not work since PAPI_enum_cmp_event returns PAPI_EINVAL when end of list is reached.
	if (retv != PAPI_ENOEVNT) {
		PAPI_destroy_eventset(EventSet);
		*num_events = 0;
		return retv;
	}
	*/

	return PAPI_OK;
}

/** Internal
 *	@class _apapi_addtime
 *	@brief Add seconds and nanoseconds to struct timespec
 *
 *	@param struct timespec *time
 *		struct timespec to add to
 *	@param time_t sec
 *		seconds to add
 *	@param long nsec
 *		nanoseconds to add
 */
void _apapi_addtime(struct timespec *time, time_t sec, long nsec) {
	time->tv_sec += sec;
	time->tv_nsec += nsec;
	if (time->tv_nsec > 1000000000) {
		time->tv_sec++;
		time->tv_nsec -= 1000000000;
	}
}

/** Internal
 *	@class _apapi_addtime_timespec
 *	@brief Add timespec to another timespec
 *
 *	@param struct timespec *time
 *		struct timespec to add to
 *	@param struct timespec *add
 *		struct timespec to add
 */
void _apapi_addtime_timespec(struct timespec *time, struct timespec *add) {
	time->tv_sec += add->tv_sec;
	time->tv_nsec += add->tv_nsec;
	if (time->tv_nsec > 1000000000) {
		time->tv_sec++;
		time->tv_nsec -= 1000000000;
	}
}

/** Internal
 *	@class _apapi_swap_pointer
 *	@brief Swap pointer destinations
 *
 *  @param void **pointer1
 *	@param void **pointer2
 */
void _apapi_swap_pointer(void **pointer1, void **pointer2){
	void* tmp;
	tmp = *pointer1;
	*pointer1 = *pointer2;
	*pointer2 = tmp;
}

/** Internal
 *	@class _apapi_stats
 *	@brief Computes statistics in case a new sample arrives
 *
 *	@param enum APAPI_stats stats_op
 *		Defines which statistics to compute for this counter
 *	@param long double value
 *		new value
 *	@param long long avg_value_weight
 *		weight of new value for computation of average, time interval the sample accounts for
 *	@param long long avg_last_total_weight
 *		weight of all samples (excluding new sample) for computation of average
 *	@param long long avg_new_total_weight
 *		weight of all samples (including new sample) for computation of average, whole time interval until now
 *	@param int sample_count
 *		number sampled values
 *	@param double *stats
 *		points to array for output values
 */
void _apapi_stats(enum APAPI_stats stats_op, long double value, long long avg_value_weight, 
	long long avg_last_total_weight, long long avg_new_total_weight, int sample_count, double *stats){

	// accumulate
	if ((stats_op & APAPI_STAT_ACC) == APAPI_STAT_ACC) {
		stats[APAPI_ACC] += value;
	}

	// average
	if ((stats_op & APAPI_STAT_AVG) == APAPI_STAT_AVG) {

		if (sample_count > 2) {
			// normal measurements between second and last measurment
			stats[APAPI_AVG] = stats[APAPI_AVG] * ((long double)avg_last_total_weight / (long double)avg_new_total_weight) + 
				value * ((long double)avg_value_weight / (long double)avg_new_total_weight);
			// TODO: order of division/multiplication: make avg value less precise or make weight factor less precise?
			// TODO: test different orders with "real" values
		} else
		if (sample_count == 2) {
			// second measurement hold first useful value, weight equals total weight
			stats[APAPI_AVG] = value; // weight    
		} // else first not-useful value or something's terribly wrong
	}

	// min
	if ((stats_op & APAPI_STAT_MIN) == APAPI_STAT_MIN) {
		if (value < stats[APAPI_MIN]) {
			stats[APAPI_MIN] = value;
		}
	}

	// max
	if ((stats_op & APAPI_STAT_MAX) == APAPI_STAT_MAX) {
		if (value > stats[APAPI_MAX]) {
			stats[APAPI_MAX] = value;
		}
	}
}

/** Internal
 *	@class _apapi_timer_measure_stats
 *	@brief Computes statistics for all events
 *
 *	@param struct apapi_eventset *set
 *		apapi event set with events to process
 */
void _apapi_timer_measure_stats(struct apapi_eventset *set) {
	
	int eventIx;
	double value1 = 0.0;
	for (eventIx = 0; eventIx < set->num_events; eventIx++) {

		// always skip first measurement
		if (set->count == 1) {
			continue;
		}

		value1 = 0.0;

		// compute value1
		if (APAPI_OP1_NOP != set->values_op1[eventIx]) {
			_apapi_exec_op1(set->values_op1[eventIx], set->previous_samples[eventIx], set->current_samples[eventIx], set->previous_time, set->current_time, &value1);
			set->last_values1[eventIx] = value1;
		}

		// compute value0 statistics
		if (set->values0_stats[eventIx] != APAPI_STAT_NO) {
			_apapi_stats(set->values0_stats[eventIx], set->current_samples[eventIx],
				set->current_time - set->previous_time, set->previous_time - set->first_time, set->current_time - set->first_time,
				set->count, &(set->values0[eventIx*APAPI_FIELDS]));
		}

		// compute value1 statistics
		if (set->values1_stats[eventIx] != APAPI_STAT_NO) {
			_apapi_stats(set->values1_stats[eventIx], value1,
				set->current_time - set->previous_time, set->previous_time - set->first_time, set->current_time - set->first_time,
				set->count, &(set->values1[eventIx*APAPI_FIELDS]));
		}
	}
}

/** Internal
 *	@class _apapi_fix_overflow
 *	@brief checks counter for overflow and repairs value
 *
 *	@param int values_num
 *		number of values
 *	@param long long *current_counters
 *		last counter values
 *	@param long long *previous_counters
 *		second last counter values
 *	@param long long *max_counters
 *		maximal value for counter, if 0 then value does not accumulate and check is not performed
 *	@param long long *new_samples
 *		output parameter for repaired values
 */
void _apapi_fix_overflow(int values_num, long long *current_counters, long long *previous_counters, long long *max_counters, long long *new_samples) {

	int eventIx;
	for (eventIx=0; eventIx<values_num; ++eventIx) {

		if (max_counters[eventIx] != 0) {
			if (current_counters[eventIx] < previous_counters[eventIx]) {
				// overflow detected
				new_samples[eventIx] = max_counters[eventIx] - previous_counters[eventIx] + current_counters[eventIx];
			} else {
				// just compute difference
				new_samples[eventIx] = current_counters[eventIx] - previous_counters[eventIx];
			}
		} else {
			// value is not accumulating
			new_samples[eventIx] = current_counters[eventIx];
		}
	}
}

/** internal
 *	@class _apapi_timer_measure
 *	@brief method called by timer to execute measurement, statistics computation and custom callback
 *
 *	@param struct apapi_timer *timer
 *		timer to use for measurment
 *	@param int last_measurement
 *		specifies if the measurement is the final one before stopping the timer
 *		last_measurement: true = 1, false = 0
 */
int _apapi_timer_measure(struct apapi_timer *timer, int last_measurement) {
	int retv = PAPI_OK;

	if (NULL != timer->set) {

		// swap arrays so current values become last values and new values overwrite last ones
		// swap before other operations, so most current values will reside in current_samples after this method for later access
		_apapi_swap_pointer((void**) &(timer->set->current_counters), (void**) &(timer->set->previous_counters));
		_apapi_swap_pointer((void**) &(timer->set->current_samples), (void**) &(timer->set->previous_samples));

		// swap timestamps
		long long tmp_time = timer->set->previous_time;
		timer->set->previous_time = timer->set->current_time;
		timer->set->current_time = tmp_time;

		// read counters
		retv = PAPI_read(timer->set->EventSet, timer->set->current_counters);

		if (retv != PAPI_OK) {
			// PAPI_read failed
			const PAPI_component_info_t *cmpinfo;
			int cmpix = PAPI_get_eventset_component(timer->set->EventSet);
			cmpinfo = PAPI_get_component_info(cmpix);
			
			APAPI_PRINTERR("failed on PAPI_read for component %s\n", cmpinfo->short_name)
			if (0 == _apapi_softfail) {
				exit(1);
			}
		}

		// update timestamp
		timer->set->current_time = _apapi_getcurrenttime();

		// check for overflow
		_apapi_fix_overflow(timer->set->num_events, timer->set->current_counters, timer->set->previous_counters,
			timer->set->max_counters, timer->set->current_samples);

		// update last/first timestamps and count number
		timer->set->last_time = timer->set->current_time;
		if (timer->set->first_time == 0) {
			timer->set->first_time = timer->set->current_time;
		}
		timer->set->count++;

		// compute statistics
		_apapi_timer_measure_stats(timer->set);	
	}

	// custom callback
	if (NULL != timer->measure){
		(*(timer->measure))(timer->measure_arg, last_measurement);
	}

	return retv;
}

/** Internal
 *	@class _apapi_timer_thread
 *	@brief method for timer thread, sleeping for configured interval and executing measurement/ statistics/ custom callback
 *	
 *	@param void *args
 *		thread parameter
 *		pointer to struct apapi_timer
 */
void* _apapi_timer_thread(void *args) {
	int retv;
	struct apapi_timer *timer = (struct apapi_timer *) args;
	pthread_t tid = 0;
	const char *cmp_name = NULL;

	if (1 == _apapi_verbose) {
		tid = pthread_self();
		cmp_name = (timer->set != NULL? timer->set->cmp_name: "unknown");
		APAPI_PRINT("Thread %lu created for component %s\n", (unsigned long) tid, cmp_name)
	}

	// wait until first measurement
	retv = pthread_mutex_lock(&(timer->mutex));
	if (0 != retv) {
		// abort
		return NULL;
	}

	// abort
	if (timer->interrupt) {
		return NULL;
	}

/*
	retv = PAPI_start(timer->set->EventSet);
	if (PAPI_OK != retv) {
		return -1;
	}
*/

	// first measurement
	_apapi_timer_measure(timer, 0);

	// get current time
	struct timespec next_wakeup;
	clock_gettime(CLOCK_REALTIME, &next_wakeup);

	if (1 == _apapi_verbose) {
		APAPI_PRINT("Thread %lu component %s interval %ld.%ld start %ld.%ld\n", (unsigned long) tid, cmp_name, timer->interval.tv_sec, timer->interval.tv_nsec,
		next_wakeup.tv_sec, next_wakeup.tv_nsec)
	}

	// sleep/ measurement loop
	while (1) {

		// add sleeping time to last wakeup
		_apapi_addtime_timespec(&next_wakeup, &(timer->interval));
		// wait for next wakeup or signal, in the meantime mutex is unlocked
		retv = pthread_mutex_timedlock(&(timer->mutex), &next_wakeup);

		// interrupted - immediate stop
		if(timer->interrupt) {
			break;
		}

		// measurement
		// if retv == 0, the mutex was acquired and this is the final measurement
		_apapi_timer_measure(timer, retv == 0 ? 1 : 0);
		
		// lock timed out - repeat
		if (ETIMEDOUT == retv)
			continue;

		// mutex acquired - stop measurement
		if (0 == retv)
			break;

		// error - abort
		break;
	}
	return NULL;
}

int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set) {

	int retv;
	*timer = calloc(1, sizeof(struct apapi_timer));
	if (NULL == *timer) {
		return -1;
	}
	struct apapi_timer *newtimer;
	newtimer = *timer;
	retv = pthread_mutex_init(&(newtimer->mutex), NULL);
	if (0 != retv) {
		free(*timer);
		*timer = NULL;
		return -1;
	}

	newtimer->state = APAPI_TIMER_STATE_DONE; // set state to DONE so reset_timer will work
	retv = APAPI_reset_timer(newtimer, tv_sec, tv_nsec, measure, measure_arg, set);
	if (PAPI_OK != retv) {
		free(*timer);
		*timer = NULL;
		return -1;
	}
	return PAPI_OK;
}

int APAPI_change_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set) {

	if (APAPI_TIMER_STATE_READY != timer->state)
		return -1;

	timer->interval.tv_sec = tv_sec;
	timer->interval.tv_nsec = tv_nsec;
	timer->measure = measure;
	timer->measure_arg = measure_arg;
	timer->set = set;
	
	if (NULL != set) {
		memset(set->current_counters, 0, set->num_events);
		memset(set->previous_counters, 0, set->num_events);
		memset(set->current_samples, 0, set->num_events);
		memset(set->previous_samples, 0, set->num_events);
		set->first_time = 0;
		set->last_time = 0;
		set->previous_time = 0;
		set->count = 0;
		int eventIx;
		for (eventIx = 0; eventIx < set->num_events; ++eventIx) {
			set->values0[eventIx * APAPI_FIELDS + APAPI_MIN] = DBL_MAX;
			set->values0[eventIx * APAPI_FIELDS + APAPI_MAX] = 0;
			set->values0[eventIx * APAPI_FIELDS + APAPI_AVG] = 0;
			set->values0[eventIx * APAPI_FIELDS + APAPI_ACC] = 0;
			set->values1[eventIx * APAPI_FIELDS + APAPI_MIN] = DBL_MAX;
			set->values1[eventIx * APAPI_FIELDS + APAPI_MAX] = 0;
			set->values1[eventIx * APAPI_FIELDS + APAPI_AVG] = 0;
			set->values1[eventIx * APAPI_FIELDS + APAPI_ACC] = 0;
		}
	}
	return PAPI_OK;
}

int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set) {

	if (APAPI_TIMER_STATE_DONE != timer->state) {
		return -1;
	}

	int retv;
	timer->interval.tv_sec = tv_sec;
	timer->interval.tv_nsec = tv_nsec;
	timer->measure = measure;
	timer->measure_arg = measure_arg;

	// clean up old values
	if (NULL != set) {
		timer->set = set;
		memset(set->current_counters, 0, set->num_events);
		memset(set->previous_counters, 0, set->num_events);
		memset(set->current_samples, 0, set->num_events);
		memset(set->previous_samples, 0, set->num_events);
		set->first_time = 0;
		set->last_time = 0;
		set->previous_time = 0;
		set->count = 0;
		int eventIx;
		for (eventIx = 0; eventIx < set->num_events; eventIx++) {
			set->values0[eventIx * APAPI_FIELDS + APAPI_MIN] = DBL_MAX;
			set->values0[eventIx * APAPI_FIELDS + APAPI_MAX] = 0;
			set->values0[eventIx * APAPI_FIELDS + APAPI_AVG] = 0;
			set->values0[eventIx * APAPI_FIELDS + APAPI_ACC] = 0;
			set->values1[eventIx * APAPI_FIELDS + APAPI_MIN] = DBL_MAX;
			set->values1[eventIx * APAPI_FIELDS + APAPI_MAX] = 0;
			set->values1[eventIx * APAPI_FIELDS + APAPI_AVG] = 0;
			set->values1[eventIx * APAPI_FIELDS + APAPI_ACC] = 0;
		}
	}
	
	timer->interrupt = 0;

	pthread_mutex_lock(&(timer->mutex));

	retv = pthread_create(&(timer->thread), NULL, _apapi_timer_thread, (void*)timer);
	if (retv != 0) {
		return -1;
	}

	timer->state = APAPI_TIMER_STATE_READY;
	return PAPI_OK;
}

int APAPI_initstart_timer(struct apapi_timer *timer) {
	if (APAPI_TIMER_STATE_READY != timer->state) {
		return -1;
	}
	int retv = PAPI_OK;

	if (NULL != timer->set) {
		retv = PAPI_start(timer->set->EventSet);
		if (PAPI_OK != retv) {
			return -1;
		}
	}
	return PAPI_OK;
}

int APAPI_start_timer(struct apapi_timer *timer) {

	if (APAPI_TIMER_STATE_READY != timer->state) {
		return -1;
	}

	pthread_mutex_unlock(&(timer->mutex));
	timer->state = APAPI_TIMER_STATE_STARTED;
	return PAPI_OK;
}

int APAPI_stop_timer(struct apapi_timer *timer) {

	if (APAPI_TIMER_STATE_STARTED != timer->state) {
		return -1;
	}
	int retv = PAPI_OK;
	retv = pthread_mutex_unlock(&(timer->mutex));

	retv = pthread_join(timer->thread, NULL);
	timer->state = APAPI_TIMER_STATE_DONE;
	if (NULL != timer->set) {
		retv = PAPI_stop(timer->set->EventSet, NULL);
		if (PAPI_OK != retv) {
			return -1;
		}
	}
	return PAPI_OK;
}

int APAPI_callstop_timer(struct apapi_timer *timer) {

	if (APAPI_TIMER_STATE_STARTED != timer->state) {
		return -1;
	}
	int retv = PAPI_OK;
	retv = pthread_mutex_unlock(&(timer->mutex));
	if (PAPI_OK != retv) {
		return -1;
	}
	return PAPI_OK;
}

int APAPI_join_timer(struct apapi_timer *timer) {

	if (APAPI_TIMER_STATE_STARTED != timer->state) {
		return -1;
	}
	int retv;
	retv = pthread_join(timer->thread, NULL);
	timer->state = APAPI_TIMER_STATE_DONE;
	if (NULL != timer->set) {
		retv = PAPI_stop(timer->set->EventSet, NULL);
		if (PAPI_OK != retv) {
			return -1;
		}
	}
	return PAPI_OK;
}

int APAPI_interrupt_timer(struct apapi_timer *timer) {

	if (APAPI_TIMER_STATE_STARTED != timer->state) {
		return -1;
	}
	timer->interrupt = 1;
	pthread_join(timer->thread, NULL);
	timer->state = APAPI_TIMER_STATE_DONE;
	return PAPI_OK;
}

int APAPI_destroy_timer(struct apapi_timer **timer){

	struct apapi_timer *oldtimer;
	oldtimer = *timer;
	switch (oldtimer->state) {
		case APAPI_TIMER_STATE_STARTED:
		case APAPI_TIMER_STATE_READY:
			APAPI_interrupt_timer(oldtimer);
		break;
		default:
		break;
	}

	pthread_mutex_destroy(&(oldtimer->mutex));

	free(*timer);
	return PAPI_OK;
}

int APAPI_init_apapi_eventset_cmp(struct apapi_eventset **set, int cidx, char **names, struct apapi_event_ops *event_ops) {

	int retv;

	const PAPI_component_info_t *cmpinfo;
	cmpinfo = PAPI_get_component_info(cidx);


	struct apapi_eventset *newset;
	*set = calloc(1, sizeof(struct apapi_eventset));
	if (NULL == *set) {
		return -1;
	}
	newset = *set;
	newset->EventSet = PAPI_NULL;
	newset->cmp_name = cmpinfo->short_name;

	if (NULL == names) {
		// search for default eventlist
		int knownCmpIx = -1;
		if (NULL != _apapi_default_eventlist_sorted) {
			for(knownCmpIx = 0; APAPI_default_components[knownCmpIx] != NULL; ++knownCmpIx ) {
				if (strcmp(APAPI_default_components[knownCmpIx], cmpinfo->short_name) == 0) {
					break;
				}
			}
			if (APAPI_default_components[knownCmpIx] != NULL) {
				if (NULL != _apapi_default_eventlist_sorted[knownCmpIx]) {
					names = _apapi_default_eventlist_sorted[knownCmpIx];
				}
			}
		}
	}
	if (NULL == names) {
		retv = APAPI_create_eventset_cmp_all(cidx, &(newset->EventSet), &(newset->num_events));
	} else {
		retv = APAPI_create_eventset_list(names, cidx, &(newset->EventSet), &(newset->num_events));
	}

	if (1 == _apapi_verbose) {		
		APAPI_PRINT("eventset for component %s (%d)\n", cmpinfo->short_name, cidx)
	}

	if (PAPI_OK != retv) {
		free(*set);
		*set = NULL;
		return -1;
	}

	// (5*num) * sizeof(long long) - space for papi samples, max samples and cleared values
	newset->current_samples = calloc(newset->num_events, sizeof(long long)*5);
	newset->previous_samples = &(newset->current_samples[newset->num_events]);
	newset->current_counters = &(newset->current_samples[newset->num_events*2]);
	newset->previous_counters = &(newset->current_samples[newset->num_events*3]);
	newset->max_counters = &(newset->current_samples[newset->num_events*4]);


	// (4*3*num) * sizeof(double) - space for stats
	newset->last_values1 = calloc(newset->num_events, sizeof(double));
	newset->values0 = calloc(newset->num_events, sizeof(double) * APAPI_FIELDS * 2);
	newset->values1 = &(newset->values0[newset->num_events * APAPI_FIELDS]);

	newset->values_op1 = calloc(newset->num_events, sizeof(enum APAPI_op1));
	newset->values0_stats = calloc(newset->num_events, sizeof(enum APAPI_stats)*3);
	newset->values1_stats = &(newset->values0_stats[newset->num_events]);

	newset->event_ops = calloc(newset->num_events, sizeof(struct apapi_event_ops));

	// check for allocation failure
	if (NULL == newset->current_samples ||
		NULL == newset->last_values1 ||
		NULL == newset->values0 ||
		NULL == newset->values_op1 ||
		NULL == newset->values0_stats ||
		NULL == newset->event_ops) {

		APAPI_destroy_apapi_eventset(set);
		return -1;
	}

	// load default event operations

	int events[newset->num_events];
	retv = PAPI_list_events(newset->EventSet, events, &(newset->num_events));
	char name[PAPI_MAX_STR_LEN];
	memset(name, 0, PAPI_MAX_STR_LEN);
	int eventIx, eventopIx;
	struct apapi_event_ops *event_operation;
	for (eventIx = 0; eventIx < newset->num_events; eventIx++) {
		retv = PAPI_event_code_to_name(events[eventIx], name);
		if (PAPI_OK != retv) {
			APAPI_destroy_apapi_eventset(set);
			return -1;
		}

		// try to find op for this event
		event_operation = NULL;
		// first check user defined operations from event_ops parameter
		if (NULL != event_ops) {
			for(eventopIx = 0; NULL != event_ops[eventopIx].event_name ; ++eventopIx) {
				if (strcmp(event_ops[eventopIx].event_name, name) == 0) {
					event_operation = &(event_ops[eventopIx]);
					break;
				}
			}
		}
		// if not user defined then check predefines from apapi_defaults.c
		if (NULL == event_operation && NULL != _apapi_default_eventops) {
			for(eventopIx = 0; NULL != _apapi_default_eventops[eventopIx].event_name ; ++eventopIx) {
				if (strcmp(_apapi_default_eventops[eventopIx].event_name, name) == 0) {
					event_operation = &(_apapi_default_eventops[eventopIx]);
					break;
				}
			}
		}
		// if not defined use default
		if (NULL == event_operation) {
			APAPI_PRINTERR("failed to find event operation definition for event %s\n", name)
			if (0 == _apapi_softfail) {
				APAPI_destroy_apapi_eventset(set);
				return -1;			
			}
			event_operation = &(_apapi_default_op);
		}
		// set event operations
		newset->values_op1[eventIx] = event_operation->op1;
		newset->values0_stats[eventIx] = event_operation->value0;
		newset->values1_stats[eventIx] = event_operation->value1;
		newset->max_counters[eventIx] = event_operation->max_sample;
		newset->event_ops[eventIx] = *event_operation;
		if (_apapi_verbose == 1) {
			APAPI_PRINT("%s %d %d %d %llx %s %s %f %s %s %f\n", name, event_operation->op1, event_operation->value0, event_operation->value1, event_operation->max_sample,
				event_operation->value0_type, event_operation->value0_unit, event_operation->value0_prefix, event_operation->value1_type, event_operation->value1_unit, event_operation->value1_prefix)
		}
	}

	return PAPI_OK;
}

int APAPI_destroy_apapi_eventset(struct apapi_eventset **set) {

	struct apapi_eventset *oldset;
	oldset = *set;
	PAPI_destroy_eventset(&(oldset->EventSet));

	// because of swap_pointer the original pointer maybe at one of both places
	if (oldset->current_samples < oldset->previous_samples) {
		free(oldset->current_samples);
	} else {
		free(oldset->previous_samples);
	}
	free(oldset->last_values1);
	free(oldset->values0);
	free(oldset->values_op1);
	free(oldset->values0_stats);
	free(oldset->event_ops);
	free(oldset);
	*set = NULL;
	return PAPI_OK;
}

void APAPI_print_apapi_eventset(struct apapi_eventset *set) {

	int retv;
	int cidx = PAPI_get_eventset_component(set->EventSet);

	const PAPI_component_info_t *cmpinfo;
	cmpinfo = PAPI_get_component_info(cidx);
	printf("%s: %d events\n", cmpinfo->name, set->num_events);
	printf("------\n");
	int events[set->num_events];
	int num_events = set->num_events;
	retv = PAPI_list_events(set->EventSet, events, &num_events);
	char name[PAPI_MAX_STR_LEN];
	memset(name, 0, PAPI_MAX_STR_LEN);
	int eventIx;
	double *values;
	for(eventIx = 0; eventIx < set->num_events; ++eventIx) {
		retv = PAPI_event_code_to_name(events[eventIx], name);
		if (PAPI_OK != retv) {
			strncpy(name, "unknown", 8); // also copy null-terminator
		}
		printf("%s %lld\n", name, set->current_samples[eventIx]);
		if (set->values0_stats[eventIx] != APAPI_STAT_NO) {
			values = &(set->values0[eventIx * APAPI_FIELDS]);
				printf("  %s %s %f\n", set->event_ops[eventIx].value0_type, 
					set->event_ops[eventIx].value0_unit, set->event_ops[eventIx].value0_prefix);
				if ((set->values0_stats[eventIx] & APAPI_STAT_MIN) == APAPI_STAT_MIN)
					printf("    min: %20.1f\n", 
						( set->event_ops[eventIx].value0_prefix == 1.0 ? values[APAPI_MIN] : 
						  values[APAPI_MIN] / set->event_ops[eventIx].value0_prefix));
				if ((set->values0_stats[eventIx] & APAPI_STAT_MAX) == APAPI_STAT_MAX)
					printf("    max: %20.1f\n", 
						( set->event_ops[eventIx].value0_prefix == 1.0 ? values[APAPI_MAX] :
						  values[APAPI_MAX] / set->event_ops[eventIx].value0_prefix));
				if ((set->values0_stats[eventIx] & APAPI_STAT_AVG) == APAPI_STAT_AVG)
					printf("    avg: %20.1f\n", 
						( set->event_ops[eventIx].value0_prefix == 1.0 ?  values[APAPI_AVG] :
						  values[APAPI_ACC] / set->event_ops[eventIx].value0_prefix));
				if ((set->values0_stats[eventIx] & APAPI_STAT_ACC) == APAPI_STAT_ACC)
					printf("    acc: %20.1f\n", 
						( set->event_ops[eventIx].value0_prefix == 1.0 ? values[APAPI_ACC] :
						  values[APAPI_ACC] / set->event_ops[eventIx].value0_prefix));
		}
		if (set->values1_stats[eventIx] != APAPI_STAT_NO) {
			values = &(set->values1[eventIx * APAPI_FIELDS]);
				printf("  %s %s %f\n", set->event_ops[eventIx].value1_type, 
					set->event_ops[eventIx].value1_unit, set->event_ops[eventIx].value1_prefix);
				if ((set->values1_stats[eventIx] & APAPI_STAT_MIN) == APAPI_STAT_MIN)
					printf("    min: %20.1f\n", 
						( set->event_ops[eventIx].value1_prefix == 1.0 ? values[APAPI_MIN] :
						  values[APAPI_MIN] / set->event_ops[eventIx].value1_prefix));
				if ((set->values1_stats[eventIx] & APAPI_STAT_MAX) == APAPI_STAT_MAX)
					printf("    max: %20.1f\n", 
						( set->event_ops[eventIx].value1_prefix == 1.0 ? values[APAPI_MAX] :
						  values[APAPI_MAX] / set->event_ops[eventIx].value1_prefix));
				if ((set->values1_stats[eventIx] & APAPI_STAT_AVG) == APAPI_STAT_AVG)
					printf("    avg: %20.1f\n", 
						( set->event_ops[eventIx].value1_prefix == 1.0 ? values[APAPI_AVG] :
						  values[APAPI_AVG] / set->event_ops[eventIx].value1_prefix));
				if ((set->values1_stats[eventIx] & APAPI_STAT_ACC) == APAPI_STAT_ACC)
					printf("    acc: %20.1f\n", 
						( set->event_ops[eventIx].value1_prefix == 1.0 ? values[APAPI_ACC] :
						  values[APAPI_ACC] / set->event_ops[eventIx].value1_prefix));
		}
		printf("\n");
	}
	printf("------\n\n");
}

int APAPI_apapi_eventset_find_event(struct apapi_eventset *set, char *name) {
	if (set == NULL || name == NULL) {
		return -1;
	}
	int eIx;
	for (eIx = 0; eIx < set->num_events; ++eIx)	{
		if (strcmp(set->event_ops[eIx].event_name, name) == 0) {
			return eIx;
		}
	}
	return -1;
}
