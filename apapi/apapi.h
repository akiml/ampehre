#ifndef APAPI_H
#define APAPI_H
#include <time.h>
#include <pthread.h>
#include <unistd.h>

extern char **environ;

// operation to compute derived value
// sample1 at time1: current sample
// sample0 at time0: last sample
enum APAPI_op1 {
	APAPI_OP1_NOP,						// no operation
	APAPI_OP1_SAMPLE_DIFF,				// sample1-sample0
	APAPI_OP1_DIV_DIFF_TIME,			// value / (time1-time0)
	APAPI_OP1_SAMPLE1_MUL_DIFF_TIME,	// sample1 * (time1-time0)
	APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME	// (sample0+sample1)/2 * (time1-time0)
};

// to be used as bit mask values
enum APAPI_stats {
	APAPI_STAT_NO  = 0,
	APAPI_STAT_MIN = 1,
	APAPI_STAT_MAX = 2,
	APAPI_STAT_AVG = 4,
	APAPI_STAT_ACC = 8,
	APAPI_STAT_ALL = -1
};

// TODO: add enums for min,max,avg,acc offsets in stat arrays!

struct apapi_event_ops {
	char *event_name;
	enum APAPI_op1 op1;
	enum APAPI_stats value0;
	enum APAPI_stats value1;
	long long max_sample; // max value - for overflow check
	char *value0_type;
	char *value0_unit;
	double value0_prefix;
	char *value1_type;
	char *value1_unit;
	double value1_prefix;
};

extern int APAPI_DEFAULT_SET_NUM;
extern struct apapi_event_ops APAPI_DEFAULT_EVENT_OPS[];
extern struct apapi_event_ops APAPI_DEFAULT_OPS;
//#include "apapi_defaults.h"

struct apapi_eventset {
	int EventSet;
	int num_events;
	// papi samples
	long long *current_counters;
	long long *previous_counters;
	// maximal values for events - important for overflow check
	long long *max_counters; // if value > 0 then do overflow_check else just apply new value
	// samples - overflow cleared
	long long *current_samples;
	long long *previous_samples;
	// sample counter
	long long count;
	// time
	long long current_time;
	long long previous_time;
	long long first_time;
	long long last_time;
	// last value1 values
	double *last_values1;
				// min,max,avg,acc values - as arrays of size 4
	double *values0;
	double *values1;
	// operations
	enum APAPI_op1 *values_op1;
	// value statistics
	enum APAPI_stats *values0_stats;
	enum APAPI_stats *values1_stats;
	struct apapi_event_ops *event_ops;
};

enum APAPI_timer_state {
	APAPI_TIMER_STATE_READY,
	APAPI_TIMER_STATE_STARTED,
	APAPI_TIMER_STATE_DONE
};

struct apapi_timer {
	enum APAPI_timer_state state;
	pthread_t thread;
	struct timespec interval;
	pthread_mutex_t mutex;
	int interrupt;
	void (*measure)(void**, int);
	void** measure_arg;
	struct apapi_eventset *set;
};



int APAPI_init();

int APAPI_create_eventset_list(char **events, int cidx, int *EventSet, int *num_events);

int APAPI_create_eventset_cmp_all(int cidx, int *EventSet, int *num_events);

int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set);

int APAPI_change_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set);

int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set);

int APAPI_start_timer(struct apapi_timer *timer);

// always call join_timer afterwards to properly stop the timer
int APAPI_callstop_timer(struct apapi_timer *timer);

// always call callstop_timer before
int APAPI_join_timer(struct apapi_timer *timer);

// the same as callstop_timer + join_timer
int APAPI_stop_timer(struct apapi_timer *timer);

int APAPI_destroy_timer(struct apapi_timer **timer);

int APAPI_init_apapi_eventset_cmp(struct apapi_eventset **set, int cidx, char **names, struct apapi_event_ops *event_defaults);

int APAPI_destroy_apapi_eventset(struct apapi_eventset **set);

void APAPI_print_apapi_eventset(struct apapi_eventset *set);

int APAPI_read_event_ops_csv(char *input, char delimiter, struct apapi_event_ops **events_out, int *num_events_out);

int APAPI_apapi_eventset_find_event(struct apapi_eventset *set, char *name);

int APAPI_read_environ_defaults(char **buffer, struct apapi_event_ops **events_out, int *num_events_out);

#endif
