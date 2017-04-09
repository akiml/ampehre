#ifndef APAPI_H
#define APAPI_H
#include <time.h>
#include <pthread.h>

// operation to compute derived value
// sample1 at time1: current sample
// sample0 at time0: last sample
enum APAPI_op1 {
    OP1_NOP,                        // no operation
    OP1_SAMPLE_DIFF,                // sample1-sample0
    OP1_DIV_DIFF_TIME,              // value / (time1-time0)
    OP1_SAMPLE1_MUL_DIFF_TIME,      // sample1 * (time1-time0)
    OP1_AVG_SAMPLE_MUL_DIFF_TIME    // (sample0+sample1)/2 * (time1-time0)
};


enum APAPI_op2 {
    OP2_NOP,                        // no operation
    OP2_DIV_DIFF_TIME               // value / (time1-time0)
};


enum APAPI_stats {
    STAT_NO    = 0,
    STAT_STATS = 1, // e.g. min,max,avg,acc
};

struct apapi_event_ops {
    char *event_name;
    enum APAPI_op1 op1;
    enum APAPI_op2 op2;
    enum APAPI_stats value0;
    enum APAPI_stats value1;
    enum APAPI_stats value2;
    long long max_sample; // max value - for overflow check
};

extern int APAPI_DEFAULT_SET_NUM;
extern struct apapi_event_ops APAPI_DEFAULT_EVENT_OPS[];
extern struct apapi_event_ops APAPI_DEFAULT_OPS;
//#include "apapi_defaults.h"

struct apapi_eventset {
    int EventSet;
    int num_events;
    // papi samples
    long long *current_samples;
    long long *previous_samples;
    // maximal values for events - important for overflow check
    long long *max_samples; // if value > 0 then do overflow_check else just apply new value
    // values - overflow cleared
    long long *current_values;
    long long *previous_values;
    // sample counter
    long long count;
    // time
    long long current_time;
    long long previous_time;
    long long first_time;
    long long last_time;
    // min,max,avg,acc values - as arrays of size 4
    double *values0;
    double *values1;
    double *values2;
    // operations
    enum APAPI_op1 *values_op1;
//    enum APAPI_op2 *values_op2;
    // value statistics
    enum APAPI_stats *values0_stats;
    enum APAPI_stats *values1_stats;
//    enum APAPI_stats *values2_stats;
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
    void (*measure)(void**);
    void** measure_arg;
    struct apapi_eventset *set;
};



void APAPI_dummy();

void APAPI_init();

int APAPI_create_eventset_list(char **names, int cidx, int *EventSet, int *num_events);

int APAPI_create_eventset_cmp_all(int cidx, int *EventSet, int *num_events);

int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, void(measure)(void**), void** measure_arg, struct apapi_eventset *set);

int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**), void** measure_arg, struct apapi_eventset *set);

int APAPI_start_timer(struct apapi_timer *timer);

int APAPI_stop_timer(struct apapi_timer *timer);

int APAPI_destroy_timer(struct apapi_timer **timer);

int APAPI_init_apapi_eventset_cmp(struct apapi_eventset **set, int cidx, char **names);

int APAPI_destroy_apapi_eventset(struct apapi_eventset **set);

void APAPI_print_apapi_eventset(struct apapi_eventset *set);

#endif
