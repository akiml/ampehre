#include <stdio.h>
#include <stdlib.h> //calloc
#include <errno.h> //ETIMEDOUT
#include <string.h> //memset
#include <float.h> //DBL_MAX
#include "apapi.h"
#include "papi.h"
//#define DEBUG
#include "apapi_defaults.c"
#include "apapi_op1.c"
#include "apapi_csv.c"

int APAPI_init() {
    int retval;
    retval = PAPI_library_init(1);
    return retval;
};

long long getcurrenttime() {
    struct timespec result;
    clock_gettime(CLOCK_MONOTONIC_RAW, &result);
    return (long long)result.tv_sec* 1000000000LL + (long long)result.tv_nsec;
}

int APAPI_create_eventset_list(char **names, int cidx, int *EventSet, int *num_events){

    int retv;

    // create new event set
    retv = PAPI_create_eventset(EventSet);
    if (retv != PAPI_OK)
        return retv;
    int eventIx;
    *num_events = 0;
    for(eventIx = 0; names[eventIx] != NULL; eventIx++) {
        retv = PAPI_add_named_event(*EventSet, names[eventIx]);
        if (retv == PAPI_OK) {
            (*num_events)++;
        } else {
            printf("Failed to add event \"%s\" to EventSet.\n", names[eventIx]);
        }
    }

    PAPI_assign_eventset_component(*EventSet, cidx);

    return PAPI_OK;
}

/** @class APAPI_create_eventset_cmp_all
 *  @brief creates new eventset containing all events of a certain component
 * 
 *  @param int cidx
 *      Component index
 *  @param int *EventSet
 *      EventSet
 *
 *  @retval 0 or ERROR_CODE
 *  @see PAPI_create_eventset
 */
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
    while(retv == PAPI_OK) {
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

void addtime(struct timespec *time, time_t sec, long nsec) {
    time->tv_sec += sec;
    time->tv_nsec += nsec;
    if (time->tv_nsec > 1000000000) {
        time->tv_sec++;
        time->tv_nsec -= 1000000000;
    }
}

void addtime_timespec(struct timespec *time, struct timespec *add) {
    time->tv_sec += add->tv_sec;
    time->tv_nsec += add->tv_nsec;
    if (time->tv_nsec > 1000000000) {
        time->tv_sec++;
        time->tv_nsec -= 1000000000;
    }
}

// internal
void swap_pointer(void **pointer1, void**pointer2){
    void* tmp;
    tmp = *pointer1;
    *pointer1 = *pointer2;
    *pointer2 = tmp;
}


/*
void exec_op2(enum APAPI_op2 op2, double value1, long long time0, long long time1, double *value2) {

    switch(op2) {
        case OP2_DIV_DIFF_TIME:
            *value2 = value1 / (time1 - time0);
        break;
        //case OP2_NOP:
        default:
        break;
    }
}
*/

void stats(enum APAPI_stats stats_op, long double value, long long avg_value_weight, 
    long long avg_last_total_weight, long long avg_new_total_weight, int sample_count, double *stats){

    // accumulate
    if ((stats_op & APAPI_STAT_ACC) == APAPI_STAT_ACC) {

        stats[3] += value;
    }

    // average
    if ((stats_op & APAPI_STAT_AVG) == APAPI_STAT_AVG) {

        if (sample_count > 2) {
            // normal measurements between second and last measurment
            stats[2] = stats[2] * ((long double)avg_last_total_weight / (long double)avg_new_total_weight) + 
                value * ((long double)avg_value_weight / (long double)avg_new_total_weight);
            // TODO: order of division/multiplication: make avg value less precise or make weight factor less precise?
            // TODO: test different orders with "real" values
        } else
        if (sample_count == 2) {
            // second measurement hold first useful value, weight equals total weight
            stats[2] = value; // weight    
        } // else first not-useful value or something's terribly wrong
    }

    // min
    if ((stats_op & APAPI_STAT_MIN) == APAPI_STAT_MIN) {
        if (value < stats[0]) {
            stats[0] = value;
        }
    }

    // max
    if ((stats_op & APAPI_STAT_MAX) == APAPI_STAT_MAX) {

        if (value > stats[1]) {
            stats[1] = value;
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
    for(eventIx = 0; eventIx < set->num_events; eventIx++) {

        #ifdef DEBUG
        printf("in event %d cur %llx prevtime %llx curtime %llx min0 %f max0 %f avg0 %f acc0 %f min1 %f max1 %f avg1 %f acc1 %f \n", eventIx,
            set->current_samples[eventIx],
            set->previous_time,
            set->current_time,
            set->values0[eventIx*4 + 0],
            set->values0[eventIx*4 + 1],
            set->values0[eventIx*4 + 2],
            set->values0[eventIx*4 + 3],
            set->values1[eventIx*4 + 0],
            set->values1[eventIx*4 + 1],
            set->values1[eventIx*4 + 2],
            set->values1[eventIx*4 + 3]);
        #endif

        // always skip first measurement
        if (set->count == 1) {
            continue;
        }

        value1 = 0.0;


        exec_op1(set->values_op1[eventIx], set->previous_samples[eventIx], set->current_samples[eventIx], set->previous_time, set->current_time,
            &value1);
        
//        exec_op2(set->values_op2[eventIx], value1, set->previous_time, set->current_time, &value2);

        if (set->values0_stats[eventIx] != APAPI_STAT_NO) {
            stats(set->values0_stats[eventIx], set->current_samples[eventIx],
                set->current_time - set->previous_time, set->previous_time - set->first_time, set->current_time - set->first_time,
                set->count, &(set->values0[eventIx*4]));
        }
        if (set->values1_stats[eventIx] != APAPI_STAT_NO) {
            stats(set->values1_stats[eventIx], value1,
                set->current_time - set->previous_time, set->previous_time - set->first_time, set->current_time - set->first_time,
                set->count, &(set->values1[eventIx*4]));
        }
//        if (set->values2_stats[eventIx] != APAPI_STAT_NO) {
//            stats(set->values2_stats[eventIx], value2, set->count, &(set->values2[eventIx]));
//        }

        #ifdef DEBUG
        printf("out event %d prev %llx cur %llx prevtime %llx curtime %llx min0 %f max0 %f avg0 %f acc0 %f min1 %f max1 %f avg1 %f acc1 %f \n", eventIx,
            set->previous_samples[eventIx],
            set->current_samples[eventIx],
            set->previous_time,
            set->current_time,
            set->values0[eventIx*4 + 0],
            set->values0[eventIx*4 + 1],
            set->values0[eventIx*4 + 2],
            set->values0[eventIx*4 + 3],
            set->values1[eventIx*4 + 0],
            set->values1[eventIx*4 + 1],
            set->values1[eventIx*4 + 2],
            set->values1[eventIx*4 + 3]);
        #endif



    }

}

void fix_overflow(long long *current_counters, long long *previous_counters, long long *max_counters, long long *new_samples, int values_num) {

    int eventIx;
    for(eventIx=0; eventIx<values_num; eventIx++) {
        //printf("last %lld cur %lld new %lld\n", previous_values[eventIx], current_samples[eventIx], new_values[eventIx]);
        if (max_counters[eventIx] != 0) {
            if (current_counters[eventIx] < previous_counters[eventIx]) {
                // overflow detected
                new_samples[eventIx] = max_counters[eventIx] - previous_counters[eventIx] + current_counters[eventIx];
                printf("overflow\n");
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
int APAPI_timer_measure(struct apapi_timer *timer) {
    int retv = PAPI_OK;
    // TODO: is last_measurement bool necessary/useful?

    if (timer->set != NULL) {

        // swap arrays so current values become last values and new values overwrite last ones
        // swap before other operations, so most current values will reside in current_samples for later access
        swap_pointer((void**) &(timer->set->current_counters), (void**) &(timer->set->previous_counters));
        swap_pointer((void**) &(timer->set->current_samples), (void**) &(timer->set->previous_samples));

        // swap timestamps
        long long tmp_time = timer->set->previous_time;
        timer->set->previous_time = timer->set->current_time;
        timer->set->current_time = tmp_time;

        retv = PAPI_read(timer->set->EventSet, timer->set->current_counters);
        if (retv != PAPI_OK) {
            // TODO:
        }

        timer->set->current_time = getcurrenttime();

        // check for overflow
        fix_overflow(timer->set->current_counters, timer->set->previous_counters, timer->set->max_counters, 
            timer->set->current_samples, timer->set->num_events);
    
        timer->set->last_time = timer->set->current_time;
        if (timer->set->first_time == 0) {
            timer->set->first_time = timer->set->current_time;
        }
        timer->set->count++;
        APAPI_timer_measure_stats(timer->set);
        
    }

    if (timer->measure != NULL){
        (*(timer->measure))(timer->measure_arg);
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

    APAPI_timer_measure(timer);

    //
    struct timespec next_wakeup;
    clock_gettime(CLOCK_REALTIME, &next_wakeup);
    while(1){
        // add sleeping time to last wakeup
        addtime_timespec(&next_wakeup, &(timer->interval));
        // wait for next wakeup or signal, in the mean time mutex is unlocked
        retv = pthread_mutex_timedlock(&(timer->mutex), &next_wakeup);

        // interrupted - immediate stop
        if(timer->interrupt) {
            break;
        }

        APAPI_timer_measure(timer);
        
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

int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, void(measure)(void**), void** measure_arg, struct apapi_eventset *set) {
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

int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**), void** measure_arg, struct apapi_eventset *set) {

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
        for(eventIx=0; eventIx < set->num_events; eventIx++) {
            set->values0[eventIx*4] = DBL_MAX;
            set->values0[eventIx*4 + 1] = 0;
            set->values0[eventIx*4 + 2] = 0;
            set->values0[eventIx*4 + 3] = 0;
            set->values1[eventIx*4] = DBL_MAX;
            set->values1[eventIx*4 + 1] = 0;
            set->values1[eventIx*4 + 2] = 0;
            set->values1[eventIx*4 + 3] = 0;
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
    switch(oldtimer->state) {
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

    // (5*num) * sizeof(long long) - space for papi samples, max samples and cleared values
    newset->current_samples = calloc(newset->num_events, sizeof(long long)*5);
    newset->previous_samples = &(newset->current_samples[newset->num_events]);
    newset->current_counters = &(newset->current_samples[newset->num_events*2]);
    newset->previous_counters = &(newset->current_samples[newset->num_events*3]);
    newset->max_counters = &(newset->current_samples[newset->num_events*4]);

    //printf("cur values%p %p %p %p %p\n", newset->current_samples, newset->previous_values, newset->current_counters, newset->previous_counters, newset->max_counters);

    // (4*3*num) * sizeof(double) - space for stats
    //printf("calloc %d, %ld\n", newset->num_events, sizeof(double)*4*3);
    newset->values0 = calloc(newset->num_events, sizeof(double)*4*3);
    newset->values1 = &(newset->values0[4*newset->num_events]);
//    newset->values2 = &(newset->values0[8*newset->num_events]);
    //printf("values %p %p\n", newset->values0, newset->values1);
    //printf("long long %ld double %ld\n", sizeof(long long), sizeof(double));
    //printf("%f %f %f %f\n", newset->values0[0], newset->values0[1], newset->values0[2], newset->values0[3]);
    //printf("%f %f %f %f\n", newset->values1[0], newset->values1[1], newset->values1[2], newset->values1[3]);

    newset->values_op1 = calloc(newset->num_events, sizeof(enum APAPI_op1));
//    newset->values_op2 = calloc(newset->num_events, sizeof(enum APAPI_op2));
    newset->values0_stats = calloc(newset->num_events, sizeof(enum APAPI_stats)*3);
    newset->values1_stats = &(newset->values0_stats[newset->num_events]);
//    newset->values2_stats = &(newset->values0_stats[newset->num_events*2]);

    newset->event_ops = calloc(newset->num_events, sizeof(struct apapi_event_ops));

    // load default event ops

    int events[newset->num_events];
    retv = PAPI_list_events(newset->EventSet, events, &(newset->num_events));
    char name[PAPI_MAX_STR_LEN];
    memset(name, 0, PAPI_MAX_STR_LEN);
    int eventIx,defaultIx;
    struct apapi_event_ops *ops;
    for(eventIx=0; eventIx<newset->num_events; eventIx++) {
        retv = PAPI_event_code_to_name(events[eventIx], name);
        if (retv != PAPI_OK) {
            // TODO:
        }
        //printf("%s %lld\n", name, set->current_samples[i]);
        // try to find default op
        ops = NULL;
        // first check user defined ops from event_defaults parameter
        if (event_defaults != NULL) {
            for(defaultIx=0; event_defaults[defaultIx].event_name!=NULL; defaultIx++) {
                if (strcmp(event_defaults[defaultIx].event_name, name) == 0) {
                    ops = &(event_defaults[defaultIx]);
                    break;
                }
            }
        }
        // if not user defined then check predefines from apapi_defaults.c
        if (ops == NULL) {
            for(defaultIx=0; APAPI_DEFAULT_EVENT_OPS[defaultIx].event_name!=NULL; defaultIx++) {
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
//        newset->values_op2[eventIx] = ops->op2;
        newset->values0_stats[eventIx] = ops->value0;
        newset->values1_stats[eventIx] = ops->value1;
//        newset->values2_stats[eventIx] = ops->value2;
        newset->max_counters[eventIx] = ops->max_sample;
        newset->event_ops[eventIx] = *ops;
        printf("%s %d %d %d %llx %s %s %f %s %s %f\n", name, ops->op1, ops->value0, ops->value1, ops->max_sample,
            ops->value0_type, ops->value0_unit, ops->value0_prefix, ops->value1_type, ops->value1_unit, ops->value1_prefix);
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
    free(oldset->values0);
    free(oldset->values_op1);
//    free(oldset->values_op2);
    free(oldset->values0_stats);
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
    for(eventIx=0; eventIx<set->num_events; eventIx++) {
        retv = PAPI_event_code_to_name(events[eventIx], name);
        if (retv != PAPI_OK) {
            // TODO:
        }
        printf("%s %lld\n", name, set->current_samples[eventIx]);
        if (set->values0_stats[eventIx] != APAPI_STAT_NO) {
            values = &(set->values0[eventIx*4]);
                printf("  %s %s %f\n", set->event_ops[eventIx].value0_type, 
                    set->event_ops[eventIx].value0_unit, set->event_ops[eventIx].value0_prefix);
                if ((set->values0_stats[eventIx] & APAPI_STAT_MIN) == APAPI_STAT_MIN)
                    printf("    min: %20.6f\n", values[0]);
                if ((set->values0_stats[eventIx] & APAPI_STAT_MAX) == APAPI_STAT_MAX)
                    printf("    max: %20.6f\n", values[1]);
                if ((set->values0_stats[eventIx] & APAPI_STAT_AVG) == APAPI_STAT_AVG)
                    printf("    avg: %20.6f\n", values[2]);
                if ((set->values0_stats[eventIx] & APAPI_STAT_ACC) == APAPI_STAT_ACC)
                    printf("    acc: %20.6f\n", values[3]);
        }
        if (set->values1_stats[eventIx] != APAPI_STAT_NO) {
            values = &(set->values1[eventIx*4]);
                printf("  %s %s %f\n", set->event_ops[eventIx].value1_type, 
                    set->event_ops[eventIx].value1_unit, set->event_ops[eventIx].value1_prefix);
                if ((set->values1_stats[eventIx] & APAPI_STAT_MIN) == APAPI_STAT_MIN)
                    printf("    min: %20.6f\n", values[0]);
                if ((set->values1_stats[eventIx] & APAPI_STAT_MAX) == APAPI_STAT_MAX)
                    printf("    max: %20.6f\n", values[1]);
                if ((set->values1_stats[eventIx] & APAPI_STAT_AVG) == APAPI_STAT_AVG)
                    printf("    avg: %20.6f\n", values[2]);
                if ((set->values1_stats[eventIx] & APAPI_STAT_ACC) == APAPI_STAT_ACC)
                    printf("    acc: %20.6f\n", values[3]);
        }
        printf("\n");
//        if (set->values2_stats[eventIx] != STAT_NO) {
//            values = &(set->values2[eventIx]);
//            printf("  min: %f max: %f avg: %f acc: %f\n", values[0], values[1], values[2], values[3]);
//        }

    }
    printf("------\n\n");
}
