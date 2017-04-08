#include <stdio.h>
#include <stdlib.h> //calloc
#include <errno.h> //ETIMEDOUT
#include <string.h> //memset
#include <float.h> //DBL_MAX
#include "apapi.h"
#include "papi.h"
//#define DEBUG
#include "apapi_defaults.c"

void APAPI_dummy() {
    printf("apapi dummy method\n");
};

void APAPI_init() {
    int retval;
    retval = PAPI_library_init(1);
    if (retval == 1)
        printf("bla init\n");
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

void exec_op1(enum APAPI_op1 op1, long long sample0, long long sample1, long long time0, long long time1, double *value1){

    switch(op1) {
        case OP1_SAMPLE_DIFF:
            *value1 = sample1 - sample0;
        break;
        case OP1_SAMPLE1_MUL_DIFF_TIME:
            *value1 = sample1 * (time1 - time0);
        break;
        case OP1_AVG_SAMPLE_MUL_DIFF_TIME:
            *value1 = (sample0 + sample1) / 2.0 * (time1 - time0);
        break;
        case OP1_DIV_DIFF_TIME:
            *value1 = sample1 / (time1 - time0);
        break;
        //case OP1_NOP:
        default:
        break;
    }
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

void stats(enum APAPI_stats stats_op, double value, long long sample_count, double *stats){

    // stats = {min, max, avg, acc}

    //if ((stats_op) & STAT_STATS == STAT_STATS) {

    //printf("%x\n", stats);

        // accumulate
        stats[3] += value;

        // average
        stats[2] = stats[3] / sample_count;

        // min
        if (value < stats[0]) {
            stats[0] = value;
        }

        // max
        if (value > stats[1]) {
            stats[1] = value;
        }
    //}
}

// internal
void APAPI_timer_measure_stats(struct apapi_eventset *set) {
    
    #ifdef DEBUG
        printf("%d\n",__LINE__);
    #endif

    int eventIx;
    double value1 = 0.0;
    double value2 = 0.0;
    for(eventIx = 0; eventIx < set->num_events; eventIx++) {

        #ifdef DEBUG
        printf("in event %d prev %llx cur %llx prevtime %llx curtime %llx min0 %f max0 %f avg0 %f acc0 %f min1 %f max1 %f avg1 %f acc1 %f \n", eventIx,
            set->previous_values[eventIx],
            set->current_values[eventIx],
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


        if (set->count == 1 && set->max_samples[eventIx] > 0) {
            //printf("skip first stats for event %d\n", eventIx);
            continue;
        }

        value1 = 0.0;
        value2 = 0.0;


        exec_op1(set->values_op1[eventIx], set->previous_values[eventIx], set->current_values[eventIx], set->previous_time, set->current_time,
            &value1);
        
//        exec_op2(set->values_op2[eventIx], value1, set->previous_time, set->current_time, &value2);

        if (set->values0_stats[eventIx] != STAT_NO) {
            stats(set->values0_stats[eventIx], set->current_values[eventIx], set->count, &(set->values0[eventIx*4]));
        }
        if (set->values1_stats[eventIx] != STAT_NO) {
            stats(set->values1_stats[eventIx], value1, set->count, &(set->values1[eventIx*4]));
        }
//        if (set->values2_stats[eventIx] != STAT_NO) {
//            stats(set->values2_stats[eventIx], value2, set->count, &(set->values2[eventIx]));
//        }

        #ifdef DEBUG
        printf("out event %d prev %llx cur %llx prevtime %llx curtime %llx min0 %f max0 %f avg0 %f acc0 %f min1 %f max1 %f avg1 %f acc1 %f \n", eventIx,
            set->previous_values[eventIx],
            set->current_values[eventIx],
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

void fix_overflow(long long *current_values, long long *previous_values, long long *max_values, long long *new_values, int values_num) {

    int eventIx;
    for(eventIx=0; eventIx<values_num; eventIx++) {
        //printf("last %lld cur %lld new %lld\n", previous_values[eventIx], current_values[eventIx], new_values[eventIx]);
        if (max_values[eventIx] != 0) {
            if (current_values[eventIx] < previous_values[eventIx]) {
                // overflow detected
                new_values[eventIx] = max_values[eventIx] - previous_values[eventIx] + current_values[eventIx];
                printf("overflow\n");
            } else {
                // just compute difference
                new_values[eventIx] = current_values[eventIx] - previous_values[eventIx];
            }
        } else {
            // value is not accumulating
            new_values[eventIx] = current_values[eventIx];
        }
        //printf("last %lld cur %lld new %lld\n", previous_values[eventIx], current_values[eventIx], new_values[eventIx]);
    }
}

// internal
int APAPI_timer_measure(struct apapi_timer *timer) {
    int retv;

    // swap arrays so current values become last values and new values overwrite last ones
    // swap before other operations, so most current values will reside in current_values for later access
    if (timer->set != NULL) {
        //swap_pointer((void**) (timer->current_values), (void**) (timer->previous_values));
        swap_pointer((void**) &(timer->set->current_values), (void**) &(timer->set->previous_values));
    }
    swap_pointer((void**) (timer->current_samples), (void**) (timer->previous_samples));

    retv = PAPI_read(timer->EventSet, *(timer->current_samples));

    // check for overflow
    fix_overflow(*(timer->current_samples), *(timer->previous_samples), timer->max_samples, *(timer->current_values), timer->num_values);
    
    if (retv == PAPI_OK && timer->set != NULL) {
        struct apapi_eventset *set = timer->set;

        // swap timestamps
        long long tmp_time = set->previous_time;
        set->previous_time = set->current_time;
        set->current_time = tmp_time;

        set->current_time = getcurrenttime();
        set->last_time = set->current_time;
        if (set->first_time == 0) {
            set->first_time = set->current_time;
        }
        set->count++;
        APAPI_timer_measure_stats(timer->set);
        
    }

    if (retv == PAPI_OK && timer->measure != NULL){
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

int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, int EventSet, long long **current_values, long long **current_samples, long long **previous_samples, long long *max_samples, void(measure)(void*), void* measure_arg, struct apapi_eventset *set) {
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

    newtimer->state = APAPI_TIMER_STATE_DONE;
    retv = APAPI_reset_timer(newtimer, tv_sec, tv_nsec, EventSet, current_values, current_samples, previous_samples, max_samples, measure, measure_arg, set);
    if (retv != PAPI_OK) {
        // TODO:
    }
    return PAPI_OK;
}

int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, int EventSet, long long **current_values, long long **current_samples, long long **previous_samples, long long *max_samples, void(measure)(void*), void* measure_arg, struct apapi_eventset *set) {

    if (timer->state != APAPI_TIMER_STATE_DONE)
        return -1;

    int retv;
    timer->interval.tv_sec = tv_sec;
    timer->interval.tv_nsec = tv_nsec;

    timer->EventSet = EventSet;
    int num_events, eventIx;
    num_events = PAPI_num_events(EventSet);
    timer->num_values = num_events;
    timer->current_values = current_values;
    timer->current_samples = current_samples;
    memset(*current_samples, 0, num_events);
    timer->previous_samples = previous_samples;
    memset(*previous_samples, 0, num_events);
    timer->max_samples = max_samples;
    timer->measure = measure;
    timer->measure_arg = measure_arg;
    timer->set = set;
    if (timer->set != NULL) {
        timer->set->first_time = 0;
        timer->set->previous_time = 0;
        timer->set->count = 0;
        for(eventIx=0; eventIx<num_events; eventIx++) {
            timer->set->values0[eventIx*4] = DBL_MAX;
            timer->set->values0[eventIx*4 + 1] = 0;
            timer->set->values0[eventIx*4 + 2] = 0;
            timer->set->values0[eventIx*4 + 3] = 0;
            timer->set->values1[eventIx*4] = DBL_MAX;
            timer->set->values1[eventIx*4 + 1] = 0;
            timer->set->values1[eventIx*4 + 2] = 0;
            timer->set->values1[eventIx*4 + 3] = 0;

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
    int retv;
    retv = PAPI_start(timer->EventSet);
    if (retv != PAPI_OK) {
        // TODO:
    }
    pthread_mutex_unlock(&(timer->mutex));
    timer->state = APAPI_TIMER_STATE_STARTED;
    return PAPI_OK;
}

int APAPI_stop_timer(struct apapi_timer *timer) {
    if (timer->state != APAPI_TIMER_STATE_STARTED) {
        return -1;
    }
    int retv;
    retv = pthread_mutex_unlock(&(timer->mutex));

    #ifdef DEBUG
        printf("apapi %d %x\n", __LINE__, (int)(timer->thread));
    #endif
    retv = pthread_join(timer->thread, NULL);
    timer->state = APAPI_TIMER_STATE_DONE;
    retv = PAPI_stop(timer->EventSet, NULL);
    if (retv != PAPI_OK) {
        // TODO:
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


int APAPI_init_apapi_eventset_cmp(struct apapi_eventset **set, int cidx, char **names) {
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
    newset->current_values = calloc(newset->num_events, sizeof(long long)*5);
    newset->previous_values = &(newset->current_values[newset->num_events]);
    newset->current_samples = &(newset->current_values[newset->num_events*2]);
    newset->previous_samples = &(newset->current_values[newset->num_events*3]);
    newset->max_samples = &(newset->current_values[newset->num_events*4]);

    printf("cur values%p %p %p %p %p\n", newset->current_values, newset->previous_values, newset->current_samples, newset->previous_samples, newset->max_samples);

    // (4*3*num) * sizeof(double) - space for stats
    printf("calloc %d, %ld\n", newset->num_events, sizeof(double)*4*3);
    newset->values0 = calloc(newset->num_events, sizeof(double)*4*3);
    newset->values1 = &(newset->values0[4*newset->num_events]);
//    newset->values2 = &(newset->values0[8*newset->num_events]);
    printf("values %p %p\n", newset->values0, newset->values1);
    printf("long long %ld double %ld\n", sizeof(long long), sizeof(double));
    printf("%f %f %f %f\n", newset->values0[0], newset->values0[1], newset->values0[2], newset->values0[3]);
    printf("%f %f %f %f\n", newset->values1[0], newset->values1[1], newset->values1[2], newset->values1[3]);

    newset->values_op1 = calloc(newset->num_events, sizeof(enum APAPI_op1));
//    newset->values_op2 = calloc(newset->num_events, sizeof(enum APAPI_op2));
    newset->values0_stats = calloc(newset->num_events, sizeof(enum APAPI_stats)*3);
    newset->values1_stats = &(newset->values0_stats[newset->num_events]);
//    newset->values2_stats = &(newset->values0_stats[newset->num_events*2]);

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
        //printf("%s %lld\n", name, set->current_values[i]);
        // try to find default op
        ops = NULL;
        for(defaultIx=0; defaultIx<APAPI_DEFAULT_SET_NUM; defaultIx++) {
            if (strcmp(APAPI_DEFAULT_EVENT_OPS[defaultIx].event_name, name) == 0) {
                ops = &(APAPI_DEFAULT_EVENT_OPS[defaultIx]);
                break;
            }
        }
        if (ops == NULL) {
            ops = &(APAPI_DEFAULT_OPS);
        }
        // set ops
        newset->values_op1[eventIx] = ops->op1;
//        newset->values_op2[eventIx] = ops->op2;
        newset->values0_stats[eventIx] = ops->value0;
        newset->values1_stats[eventIx] = ops->value1;
//        newset->values2_stats[eventIx] = ops->value2;
        newset->max_samples[eventIx] = ops->max_sample;
        printf("%s %d %d %d %d %d %llx\n", name, ops->op1, ops->op2, ops->value0, ops->value1, ops->value2, ops->max_sample);
    }


    return PAPI_OK;
}

int APAPI_destroy_apapi_eventset(struct apapi_eventset **set) {
    struct apapi_eventset *oldset;
    oldset = *set;
    PAPI_destroy_eventset(&(oldset->EventSet));

    // because of swap_pointer the original pointer maybe at one of both places
    if (oldset->current_values < oldset->previous_values) {
        free(oldset->current_values);
    } else {
        free(oldset->previous_values);
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
        printf("%s %lld\n", name, set->current_values[eventIx]);
        if (set->values0_stats[eventIx] != STAT_NO) {
            values = &(set->values0[eventIx*4]);
            printf("  min: %20.6f max: %20.6f avg: %20.6f acc: %20.6f\n", values[0], values[1], values[2], values[3]);
        }
        if (set->values1_stats[eventIx] != STAT_NO) {
            values = &(set->values1[eventIx*4]);
            printf("  min: %20.6f max: %20.6f avg: %20.6f acc: %20.6f\n", values[0], values[1], values[2], values[3]);
        }
//        if (set->values2_stats[eventIx] != STAT_NO) {
//            values = &(set->values2[eventIx]);
//            printf("  min: %f max: %f avg: %f acc: %f\n", values[0], values[1], values[2], values[3]);
//        }

    }
    printf("------\n\n");
}
