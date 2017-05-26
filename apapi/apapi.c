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
#include "apapi_defaults.c"
int _apapi_verbose = 0;
// 0 = default, only errors
// 1 = additional prints
#include "apapi_op1.c"
#include "apapi_csv.c"


int APAPI_init() {

	// search for APAPI_VERBOSE variable
	int envIx;
	char *defaults_env = NULL;
	for (envIx = 0; environ[envIx] != NULL; ++envIx) {
		if (strncmp(environ[envIx], "APAPI_VERBOSE=", 14) == 0) {
			defaults_env = environ[envIx];
			break;
		}
	}

	// variable found
	if (defaults_env != NULL) {
		long int v;
		char *endptr;
		v = strtol(&(defaults_env[14]), &endptr, 0);
		if (endptr != &(defaults_env[14]) && *endptr == 0) {
			_apapi_verbose = v;
		}
	}


	int retval;
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	return retval;
};

/** Internal
 *  Returns time from clock_gettime with seconds and nanoseconds combined as long long
 */
long long _apapi_getcurrenttime() {
	struct timespec result;
	clock_gettime(CLOCK_MONOTONIC_RAW, &result);
	return (long long)result.tv_sec* 1000000000LL + (long long)result.tv_nsec;
}

int APAPI_create_eventset_list(char **events, int cidx, int *EventSet, int *num_events){

	int retv;

	// create new event set
	retv = PAPI_create_eventset(EventSet);
	if (retv != PAPI_OK)
		return retv;
	int eventIx;
	*num_events = 0;
	for(eventIx = 0; events[eventIx] != NULL; eventIx++) {
		retv = PAPI_add_named_event(*EventSet, events[eventIx]);
		if (retv == PAPI_OK) {
			(*num_events)++;
		} else {
			APAPI_PRINTERR("Failed to add event \"%s\" to EventSet.\n", events[eventIx])
		}
	}

	PAPI_assign_eventset_component(*EventSet, cidx);

	return PAPI_OK;
}

int APAPI_create_eventset_cmp_all(int cidx, int *EventSet, int *num_events) {

	int retv;

	// create new event set
	retv = PAPI_create_eventset(EventSet);
	#ifdef DEBUG
		printf("apapi %d %d %x\n", __LINE__, retv, EventSet);
	#endif
	if (retv != PAPI_OK)
		return retv;

	// iterate over component's events and add to event set
	int EventCode = 0 | PAPI_NATIVE_MASK;
	*num_events = 0;
	// get first event code
	retv = PAPI_enum_cmp_event(&EventCode, PAPI_ENUM_FIRST, cidx);
	#ifdef DEBUG
		printf("apapi %d %d %d\n", __LINE__, retv, cidx);
	#endif
	while (retv == PAPI_OK) {
		(*num_events)++;
		retv = PAPI_add_event(*EventSet, EventCode);
		#ifdef DEBUG
			printf("apapi %d %d\n", __LINE__, retv);
		#endif
		// error on adding
		if (retv != PAPI_OK)
			break;
		// get next event code
		retv = PAPI_enum_cmp_event(&EventCode, PAPI_ENUM_EVENTS, cidx);
		#ifdef DEBUG
			printf("apapi %d %d\n", __LINE__, retv);
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

void _apapi_addtime(struct timespec *time, time_t sec, long nsec) {
	time->tv_sec += sec;
	time->tv_nsec += nsec;
	if (time->tv_nsec > 1000000000) {
		time->tv_sec++;
		time->tv_nsec -= 1000000000;
	}
}

void _apapi_addtime_timespec(struct timespec *time, struct timespec *add) {
	time->tv_sec += add->tv_sec;
	time->tv_nsec += add->tv_nsec;
	if (time->tv_nsec > 1000000000) {
		time->tv_sec++;
		time->tv_nsec -= 1000000000;
	}
}

// internal
void _apapi_swap_pointer(void **pointer1, void**pointer2){
	void* tmp;
	tmp = *pointer1;
	*pointer1 = *pointer2;
	*pointer2 = tmp;
}

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

// internal
void APAPI_timer_measure_stats(struct apapi_eventset *set) {
	
	#ifdef DEBUG
		printf("%d\n",__LINE__);
	#endif

	int eventIx;
	double value1 = 0.0;
	for (eventIx = 0; eventIx < set->num_events; eventIx++) {

		#ifdef DEBUG
		printf("in event %d cur %llx prevtime %llx curtime %llx min0 %f max0 %f avg0 %f acc0 %f min1 %f max1 %f avg1 %f acc1 %f \n", eventIx,
			set->current_samples[eventIx],
			set->previous_time,
			set->current_time,
			set->values0[eventIx*APAPI_FIELDS + APAPI_MIN],
			set->values0[eventIx*APAPI_FIELDS + APAPI_MAX],
			set->values0[eventIx*APAPI_FIELDS + APAPI_AVG],
			set->values0[eventIx*APAPI_FIELDS + APAPI_ACC],
			set->values1[eventIx*APAPI_FIELDS + APAPI_MIN],
			set->values1[eventIx*APAPI_FIELDS + APAPI_MAX],
			set->values1[eventIx*APAPI_FIELDS + APAPI_AVG],
			set->values1[eventIx*APAPI_FIELDS + APAPI_ACC]);
		#endif

		// always skip first measurement
		if (set->count == 1) {
			continue;
		}

		value1 = 0.0;

		// first check if op1 is not NOP

		exec_op1(set->values_op1[eventIx], set->previous_samples[eventIx], set->current_samples[eventIx], set->previous_time, set->current_time,
			&value1);

		set->last_values1[eventIx] = value1;
		
//		exec_op2(set->values_op2[eventIx], value1, set->previous_time, set->current_time, &value2);
		if (set->values0_stats[eventIx] != APAPI_STAT_NO) {
			_apapi_stats(set->values0_stats[eventIx], set->current_samples[eventIx],
				set->current_time - set->previous_time, set->previous_time - set->first_time, set->current_time - set->first_time,
				set->count, &(set->values0[eventIx*APAPI_FIELDS]));
		}
		if (set->values1_stats[eventIx] != APAPI_STAT_NO) {
			_apapi_stats(set->values1_stats[eventIx], value1,
				set->current_time - set->previous_time, set->previous_time - set->first_time, set->current_time - set->first_time,
				set->count, &(set->values1[eventIx*APAPI_FIELDS]));
		}
//		if (set->values2_stats[eventIx] != APAPI_STAT_NO) {
//			_apapi_stats(set->values2_stats[eventIx], value2, set->count, &(set->values2[eventIx]));
//		}

		#ifdef DEBUG
		printf("out event %d prev %llx cur %llx prevtime %llx curtime %llx min0 %f max0 %f avg0 %f acc0 %f min1 %f max1 %f avg1 %f acc1 %f \n", eventIx,
			set->previous_samples[eventIx],
			set->current_samples[eventIx],
			set->previous_time,
			set->current_time,
			set->values0[eventIx*APAPI_FIELDS + APAPI_MIN],
			set->values0[eventIx*APAPI_FIELDS + APAPI_MAX],
			set->values0[eventIx*APAPI_FIELDS + APAPI_AVG],
			set->values0[eventIx*APAPI_FIELDS + APAPI_ACC],
			set->values1[eventIx*APAPI_FIELDS + APAPI_MIN],
			set->values1[eventIx*APAPI_FIELDS + APAPI_MAX],
			set->values1[eventIx*APAPI_FIELDS + APAPI_AVG],
			set->values1[eventIx*APAPI_FIELDS + APAPI_ACC]);
		#endif



	}

}

void _apapi_fix_overflow(long long *current_counters, long long *previous_counters, long long *max_counters, long long *new_samples, int values_num) {

	int eventIx;
	for (eventIx=0; eventIx<values_num; eventIx++) {
		//printf("last %lld cur %lld new %lld\n", previous_values[eventIx], current_samples[eventIx], new_values[eventIx]);
		if (max_counters[eventIx] != 0) {
			if (current_counters[eventIx] < previous_counters[eventIx]) {
				// overflow detected
				new_samples[eventIx] = max_counters[eventIx] - previous_counters[eventIx] + current_counters[eventIx];
				//printf("overflow\n");
			} else {
				// just compute difference
				new_samples[eventIx] = current_counters[eventIx] - previous_counters[eventIx];
			}
		} else {
			// value is not accumulating
			new_samples[eventIx] = current_counters[eventIx];
		}
		//printf("last %lld cur %lld new %lld\n", previous_values[eventIx], current_samples[eventIx], new_values[eventIx]);
	}
}

// internal
// last_measurement: true = 1, false = 0
int APAPI_timer_measure(struct apapi_timer *timer, int last_measurement) {
	int retv = PAPI_OK;
	// TODO: is last_measurement bool necessary/useful?

	if (timer->set != NULL) {

		// swap arrays so current values become last values and new values overwrite last ones
		// swap before other operations, so most current values will reside in current_samples for later access
		_apapi_swap_pointer((void**) &(timer->set->current_counters), (void**) &(timer->set->previous_counters));
		_apapi_swap_pointer((void**) &(timer->set->current_samples), (void**) &(timer->set->previous_samples));

		// swap timestamps
		long long tmp_time = timer->set->previous_time;
		timer->set->previous_time = timer->set->current_time;
		timer->set->current_time = tmp_time;

		retv = PAPI_read(timer->set->EventSet, timer->set->current_counters);
		if (retv != PAPI_OK) {
			// TODO:
		}

		timer->set->current_time = _apapi_getcurrenttime();

		// check for overflow
		_apapi_fix_overflow(timer->set->current_counters, timer->set->previous_counters, timer->set->max_counters, 
			timer->set->current_samples, timer->set->num_events);
	
		timer->set->last_time = timer->set->current_time;
		if (timer->set->first_time == 0) {
			timer->set->first_time = timer->set->current_time;
		}
		timer->set->count++;
		APAPI_timer_measure_stats(timer->set);
		
	}

	if (timer->measure != NULL){
		(*(timer->measure))(timer->measure_arg, last_measurement);
	}

	return retv;
}


void* APAPI_timer_thread(void *args) {
	int retv;
	struct apapi_timer *timer = (struct apapi_timer *) args;
	// wait until first measurement
	retv = pthread_mutex_lock(&(timer->mutex));
	if (retv != 0) {
		// abort
		return NULL;
	}

	if (timer->interrupt) {
		return NULL;
	}

	APAPI_timer_measure(timer, 0);

	//
	struct timespec next_wakeup;
	clock_gettime(CLOCK_REALTIME, &next_wakeup);
	while (1) {
		// add sleeping time to last wakeup
		_apapi_addtime_timespec(&next_wakeup, &(timer->interval));
		// wait for next wakeup or signal, in the mean time mutex is unlocked
		retv = pthread_mutex_timedlock(&(timer->mutex), &next_wakeup);

		// interrupted - immediate stop
		if(timer->interrupt) {
			break;
		}

		APAPI_timer_measure(timer, retv == 0 ? 1 : 0);
		
		// lock timed out - repeat
		if (retv == ETIMEDOUT)
			continue;

		// lock acquired - stop measurement
		if (retv == 0)
			break;

		// error - abort
		break;
	}
	return NULL;
}

int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set) {
	int retv;
	*timer = calloc(1, sizeof(struct apapi_timer));
	if (*timer == NULL) {
		return -1;
	}
	struct apapi_timer *newtimer;
	newtimer = *timer;
	retv = pthread_mutex_init(&(newtimer->mutex), NULL);
	if (retv != 0) {
		// TODO:
	}

	newtimer->state = APAPI_TIMER_STATE_DONE; // set state to DONE so reset_timer will work
	retv = APAPI_reset_timer(newtimer, tv_sec, tv_nsec, measure, measure_arg, set);
	if (retv != PAPI_OK) {
		// TODO:
	}
	return PAPI_OK;
}

int APAPI_change_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set) {
	if (timer->state != APAPI_TIMER_STATE_READY)
		return -1;

	timer->interval.tv_sec = tv_sec;
	timer->interval.tv_nsec = tv_nsec;
	timer->measure = measure;
	timer->measure_arg = measure_arg;
	
	if (set != NULL) {
		timer->set = set;
		memset(set->current_counters, 0, set->num_events);
		memset(set->previous_counters, 0, set->num_events);
		memset(set->current_samples, 0, set->num_events);
		memset(set->previous_samples, 0, set->num_events);
		set->first_time = 0;
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
	return PAPI_OK;
}

int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set) {

	if (timer->state != APAPI_TIMER_STATE_DONE)
		return -1;

	int retv;
	timer->interval.tv_sec = tv_sec;
	timer->interval.tv_nsec = tv_nsec;
	timer->measure = measure;
	timer->measure_arg = measure_arg;

	// clean up old values

	if (set != NULL) {
		timer->set = set;
		memset(set->current_counters, 0, set->num_events);
		memset(set->previous_counters, 0, set->num_events);
		memset(set->current_samples, 0, set->num_events);
		memset(set->previous_samples, 0, set->num_events);
		set->first_time = 0;
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

	retv = pthread_create(&(timer->thread), NULL, APAPI_timer_thread, (void*)timer);
	if (retv != 0) {
		// TODO:
	}

	timer->state = APAPI_TIMER_STATE_READY;
	return PAPI_OK;
}

int APAPI_start_timer(struct apapi_timer *timer) {
	if (timer->state != APAPI_TIMER_STATE_READY) {
		return -1;
	}
	int retv = PAPI_OK;
	if (timer->set != NULL) {
		retv = PAPI_start(timer->set->EventSet);
		if (retv != PAPI_OK) {
			// TODO:
		}
	}
	pthread_mutex_unlock(&(timer->mutex));
	timer->state = APAPI_TIMER_STATE_STARTED;
	return PAPI_OK;
}

int APAPI_stop_timer(struct apapi_timer *timer) {
	if (timer->state != APAPI_TIMER_STATE_STARTED) {
		return -1;
	}
	int retv = PAPI_OK;
	retv = pthread_mutex_unlock(&(timer->mutex));

	#ifdef DEBUG
		printf("apapi %d %x\n", __LINE__, (int)(timer->thread));
	#endif
	retv = pthread_join(timer->thread, NULL);
	timer->state = APAPI_TIMER_STATE_DONE;
	if (timer->set != NULL) {
		retv = PAPI_stop(timer->set->EventSet, NULL);
		if (retv != PAPI_OK) {
			// TODO:
		}
	}
	return PAPI_OK;
}

int APAPI_callstop_timer(struct apapi_timer *timer) {
	if (timer->state != APAPI_TIMER_STATE_STARTED) {
		return -1;
	}
	int retv = PAPI_OK;
	retv = pthread_mutex_unlock(&(timer->mutex));
	if (retv != PAPI_OK) {
		// TODO:
	}
	return PAPI_OK;
}

int APAPI_join_timer(struct apapi_timer *timer) {
	int retv;
	if (timer->state != APAPI_TIMER_STATE_STARTED) {
		return -1;
	}
	retv = pthread_join(timer->thread, NULL);
	timer->state = APAPI_TIMER_STATE_DONE;
	if (timer->set != NULL) {
		retv = PAPI_stop(timer->set->EventSet, NULL);
		if (retv != PAPI_OK) {
			// TODO:
		}
	}
	return PAPI_OK;
}

int APAPI_interrupt_timer(struct apapi_timer *timer) {
	if (timer->state != APAPI_TIMER_STATE_STARTED) {
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


int APAPI_init_apapi_eventset_cmp(struct apapi_eventset **set, int cidx, char **names, struct apapi_event_ops *event_defaults) {
	int retv;
	struct apapi_eventset *newset;
	*set = calloc(1, sizeof(struct apapi_eventset));
	newset = *set;
	newset->EventSet = PAPI_NULL;
	if (names == NULL) {
		retv = APAPI_create_eventset_cmp_all(cidx, &(newset->EventSet), &(newset->num_events));
	} else {
		retv = APAPI_create_eventset_list(names, cidx, &(newset->EventSet), &(newset->num_events));
	}
	#ifdef DEBUG
		printf("apapi %d %d\n", __LINE__, retv);
	#endif
	if (retv != PAPI_OK) {
		// TODO:
	}

	retv = PAPI_assign_eventset_component(newset->EventSet, cidx);
	if (retv != PAPI_OK) {
		// TODO:
	}
	// TODO: check after every calloc if it was successful
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

	// load default event ops

	int events[newset->num_events];
	retv = PAPI_list_events(newset->EventSet, events, &(newset->num_events));
	char name[PAPI_MAX_STR_LEN];
	memset(name, 0, PAPI_MAX_STR_LEN);
	int eventIx,defaultIx;
	struct apapi_event_ops *ops;
	for (eventIx = 0; eventIx < newset->num_events; eventIx++) {
		retv = PAPI_event_code_to_name(events[eventIx], name);
		if (retv != PAPI_OK) {
			// TODO:
		}
		//printf("%s %lld\n", name, set->current_samples[i]);
		// try to find default op
		ops = NULL;
		// first check user defined ops from event_defaults parameter
		if (event_defaults != NULL) {
			for(defaultIx = 0; event_defaults[defaultIx].event_name != NULL; ++defaultIx) {
				if (strcmp(event_defaults[defaultIx].event_name, name) == 0) {
					ops = &(event_defaults[defaultIx]);
					break;
				}
			}
		}
		// if not user defined then check predefines from apapi_defaults.c
		if (ops == NULL) {
			for(defaultIx = 0; APAPI_DEFAULT_EVENT_OPS[defaultIx].event_name != NULL; ++defaultIx) {
				if (strcmp(APAPI_DEFAULT_EVENT_OPS[defaultIx].event_name, name) == 0) {
					ops = &(APAPI_DEFAULT_EVENT_OPS[defaultIx]);
					break;
				}
			}
		}
		// if not defined use default
		if (ops == NULL) {
			ops = &(APAPI_DEFAULT_OPS);
		}
		// set ops
		newset->values_op1[eventIx] = ops->op1;
		newset->values0_stats[eventIx] = ops->value0;
		newset->values1_stats[eventIx] = ops->value1;
		newset->max_counters[eventIx] = ops->max_sample;
		newset->event_ops[eventIx] = *ops;
		if (_apapi_verbose == 1) {
			printf("%s %d %d %d %llx %s %s %f %s %s %f\n", name, ops->op1, ops->value0, ops->value1, ops->max_sample,
				ops->value0_type, ops->value0_unit, ops->value0_prefix, ops->value1_type, ops->value1_unit, ops->value1_prefix);
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
		if (retv != PAPI_OK) {
			// TODO:
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
