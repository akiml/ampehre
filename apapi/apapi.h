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

/**
 * Defines which statistic should be calculated.
 * to be used as bit mask values
 */
enum APAPI_stats {
	APAPI_STAT_NO  = 0,
	APAPI_STAT_MIN = 1,
	APAPI_STAT_MAX = 2,
	APAPI_STAT_AVG = 4,
	APAPI_STAT_ACC = 8,
	APAPI_STAT_ALL = -1
};

/**
 * Offsets for fields with statistics in values0/values1 arrays
 */
enum APAPI_stats_field {
	APAPI_MIN = 0,
	APAPI_MAX = 1,
	APAPI_AVG = 2,
	APAPI_ACC = 3,
	APAPI_FIELDS = 4
};

/**
 * Event parameters
 */
struct apapi_event_ops {
	// PAPI event name
	char *event_name;
	// operation to calculate value1
	enum APAPI_op1 op1;
	// statistics to calculate for value0
	enum APAPI_stats value0;
	// statistics to calculate for value1
	enum APAPI_stats value1;
	// maximal counter value for overflow check
	long long max_sample;
	// name of type for value0, e.g. "energy"
	char *value0_type;
	// name of unit for value0, e.g. "Ws"
	char *value0_unit;
	// factor to scale measured value to useful value, useful_value = value0 / factor
	double value0_prefix;
	// name of type for value1, e.g. "energy"
	char *value1_type;
	// name of unit for value1, e.g. "Ws"
	char *value1_unit;
	// factor to scale measured value to useful value, useful_value = value1 / factor
	double value1_prefix;
};

/*
extern int APAPI_DEFAULT_SET_NUM;
extern struct apapi_event_ops APAPI_DEFAULT_EVENT_OPS[];
extern struct apapi_event_ops APAPI_DEFAULT_OPS;
*/
//#include "apapi_defaults.h"

/**
 * Holds information for event measurement
 */
struct apapi_eventset {
	// PAPI eventset id
	int EventSet;
	// number of events
	int num_events;
	// latest counter values from PAPI
	long long *current_counters;
	// previous counter values from PAPI
	long long *previous_counters;
	// maximal values for events - important for overflow check
	long long *max_counters; // if value > 0 then do overflow_check else just apply new value
	// last values, overflow cleared
	long long *current_samples;
	// previous values, overflow cleared
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
	// statistics for unprocessed values
	double *values0;
	// statistics for values processed by op1
	double *values1;
	// operations to process values, value1 = op1(value0)
	enum APAPI_op1 *values_op1;
	// which statistics to calculate for value0/value1
	enum APAPI_stats *values0_stats;
	enum APAPI_stats *values1_stats;
	// event parameters
	struct apapi_event_ops *event_ops;
};

/**
 * Possible states of an apapi_timer
 * On init the timer is in state READY.
 * After start the timer is in state STARTED.
 * After stop the timer is in state DONE.
 * Resetting a timer sets it back to state READY.
 */
enum APAPI_timer_state {
	APAPI_TIMER_STATE_READY,
	APAPI_TIMER_STATE_STARTED,
	APAPI_TIMER_STATE_DONE
};

/**
 *  Timed wake up mechanism for periodic measurements.
 *  In case the apapi_eventset is not set, only the callback will be called.
 */
struct apapi_timer {
	// timer state
	enum APAPI_timer_state state;
	// underlying thread used to execute measurements
	pthread_t thread;
	// interval that the timer should wait
	struct timespec interval;
	// mutex used to wake up timer thread
	pthread_mutex_t mutex;
	// internally set to 1 to prevent a final measurement
	// in case the timer gets destroyed
	int interrupt;
	// callback method, parameters: measure_arg, last_measurement
	// last_measurement is 1 for the final callback call, else 0
	void (*measure)(void**, int);
	// callback method argument
	void** measure_arg;
	// apapi_eventset used for measurement
	struct apapi_eventset *set;
};


/** @class APAPI_init
 *  @brief Initializes PAPI
 *  
 *  @retval Returns the return value of PAPI_library_init
 */
int APAPI_init();

/** @class APAPI_create_eventset_list
 *  @brief Creates PAPI eventset and adds given events
 *
 *  @param char **events
 *  	List of event names to add to the eventset
 *  @param int cidx
 *  	Component id to use for eventset
 *  @param int *EventSet
 *  	Output parameter for resulting eventset id
 *  @param int *num_events
 *  	Output parameter for number of added events
 *
 *  @retval PAPI_OK if eventset was created, else return value of PAPI_create_eventset
 *          Events may not be added.
 *  @see PAPI_create_eventset
 */
int APAPI_create_eventset_list(char **events, int cidx, int *EventSet, int *num_events);

/** @class APAPI_create_eventset_cmp_all
 *  @brief creates new eventset containing all events of a certain component
 * 
 *  @param int cidx
 *      Component id to use for eventset
 *  @param int *EventSet
 *      Output parameter for resulting eventset id
 *  @param int *num_events
 *  	Output parameter for number of added events
 *
 *  @retval PAPI_OK or return value of PAPI_create_eventset
 *          Events may not be added.
 *  @see PAPI_create_eventset
 */
int APAPI_create_eventset_cmp_all(int cidx, int *EventSet, int *num_events);

/** @class APAPI_create_timer
 *  @brief Creates a new timer and intializes the timer with the given parameters
 *
 *  @param struct apapi_timer **timer
 *  	Output parameter for created timer struct
 *  @param time_t tv_sec
 *  	Seconds for timer interval
 *  @param long tv_nsec
 *  	Nanoseconds for timer interval
 *  @param void(measure)(void**, int)
 *  	Pointer to callback method that is called on each timer interval
 *      Callback parameters:
 *  		void** arg, custom parameter, see measure_arg
 *      	int last_measurement, 1 if the measurment is being stopped, else 0
 *  @param void** measure_arg
 *  	Argument that is passed to called callback method
 *  @param struct apapi_eventset *set
 *  	apapi_eventset to be used for measurement on each timer interval
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_create_timer(struct apapi_timer **timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set);

/** @class APAPI_change_timer
 *  @brief If the timer is in READY state, the timer parameters are changed
 *
 *  @param struct apapi_timer *timer
 *  	Timer struct to change
 *  @param time_t tv_sec
 *  	Seconds for timer interval
 *  @param long tv_nsec
 *  	Nanoseconds for timer interval
 *  @param void(measure)(void**, int)
 *  	Pointer to callback method that is called on each timer interval
 *      Callback parameters:
 *  		void** arg, custom parameter, see measure_arg
 *      	int last_measurement, 1 if the measurment is being stopped, else 0
 *  @param void** measure_arg
 *  	Argument that is passed to called callback method
 *  @param struct apapi_eventset *set
 *  	apapi_eventset to be used for measurement on each timer interval
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_change_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set);

/** @class APAPI_reset_timer
 *  @brief If the timer is in DONE state, the timer is resetted to state READY and parameters are updated
 *
 *  @param struct apapi_timer *timer
 *  @param time_t tv_sec
 *  @param long tv_nsec
 *  @param void(measure)(void**, int)
 *      Callback parameters:
 *  		void** arg, custom parameter, see measure_arg
 *      	int last_measurement, 1 if the measurment is being stopped, else 0
 *  @param void** measure_arg
 *  	Argument that is passed to called callback method
 *  @param struct apapi_eventset *set
 *  	apapi_eventset to be used for measurement on each timer interval
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_reset_timer(struct apapi_timer *timer, time_t tv_sec, long tv_nsec, void(measure)(void**, int), void** measure_arg, struct apapi_eventset *set);

/** @class APAPI_start_timer
 *  @brief Starts the timer
 *
 *  @param struct apapi_timer *timer
 *  	Timer to be started
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_start_timer(struct apapi_timer *timer);

/** @class APAPI_callstop_timer
 *  @brief Triggers the timer to stop
 *  	The timer will execute one final measurement and stop.
 *  	Always call APAPI_join_timer afterwards to properly stop the timer !
 *
 *  @param struct apapi_timer *timer
 *  	Timer to trigger to stop
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_callstop_timer(struct apapi_timer *timer);

/** @class APAPI_join_timer
 *  @brief Waits until the timer is done and properly stops it
 *  	Always call APAPI_callstop_timer before !
 *
 *  @param struct apapi_timer *timer
 *  	Timer to wait for
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_join_timer(struct apapi_timer *timer);

/** @class APAPI_stop_timer
 *  @brief Triggers the timer to stop and waits for it
 *  	Essentially a combination of callstop_timer and join_timer.
 *
 *  @param struct apapi_timer *timer
 *  	Timer to stop
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_stop_timer(struct apapi_timer *timer);

/** @class APAPI_destroy_timer
 *  @brief Frees the timer
 *  	If the timer is not done it will be stopped.
 *		No final measurement will be executed.
 *
 *  @param struct apapi_timer **timer
 *		Timer to destroy, Pointer will be set to NULL
 *
 *	@retval PAPI_OK or on error something else
 */
int APAPI_destroy_timer(struct apapi_timer **timer);

/** @class APAPI_init_apapi_eventset_cmp
 *  @brief Creates a new apapi_eventset.
 *		The underlying PAPI eventset is initialized with the given events, if names is given, or else with all component events.
 *      The given event defaults, if event_defaults is given, are used to set the event parameters.
 *
 *  @param struct apapi_eventset **set
 *		Output parameter for created apapi_eventset
 *  @param int cidx
 *  	Component id to use for PAPI eventset
 *  @param char **names
 *  	Names of events to add to the PAPI eventset. If NULL all component events are used.
 *  @param struct apapi_event_ops *event_defaults
 *  	Used to set event parameters. If NULL or event is not listed internal definitions or fallback parameters are used.
 *
 *  @retval PAPI_OK or on error something else
 *  @see APAPI_create_eventset_cmp_all
 *	@see APAPI_create_eventset_list
 */
int APAPI_init_apapi_eventset_cmp(struct apapi_eventset **set, int cidx, char **names, struct apapi_event_ops *event_defaults);

/** @class APAPI_destroy_apapi_eventset
 *  @brief Frees apapi_eventset
 *
 *  @param struct apapi_eventset **set
 *  	apapi_eventset to free, Pointer is set to NULL
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_destroy_apapi_eventset(struct apapi_eventset **set);

/** @class APAPI_print_apapi_eventset
 *  @brief Prints measurement results according to event parameters to stdout.
 *
 *  @param struct apapi_eventset *set
 *  	apapi_eventset to print
 */
void APAPI_print_apapi_eventset(struct apapi_eventset *set);

/** @class APAPI_read_event_ops_csv
 *  @brief Reads a CSV (Comma separated values) file for event parameters.
 *		Empty lines are permitted.
 *		Example line:
 *			rapl:::PACKAGE_ENERGY:PACKAGE0,APAPI_OP1_DIV_DIFF_TIME,APAPI_STAT_ACC,APAPI_STAT_MIN|APAPI_STAT_MAX|APAPI_STAT_AVG,0xffffffff,energy,Ws,1E9,power,W,1
 *  	Fields:
 *			Name of the corresponding PAPI event
 *			Operation to compute value1, see enum APAPI_op1
 *			Statistics for value0, see enum APAPI_stats, combination of values possible
 *			Statistics for value1, see enum APAPI_stats, combination of values possible
 *			Overflow value for counters as double literal, if set to 0 it is assumed that the event won't overflow
 *			Name for type of value0
 *			Name for unit of value0
 *			Factor used to scale value0 to the correct prefix, corrected_value = value0 / factor
 *			Name for type of value1
 *			Name for unit of value1
 *			Factor used to scale value1 to the correct prefix, corrected_value = value1 / factor
 *
 *  @param char *input
 *  	Null delimited text that contains the event parameters.
 *		The content will be changed in the process of processing.
 *  @param char delimiter
 *  	To use as delimiter between the fields, usually ',' is used
 *  @param struct apapi_event_ops **events_out
 *  	Output parameter for read event parameters.
 *		The char* inside the structs point inside the given input text.
 *		Free the pointer after use.
 *  @param int *num_events_out
 *		Output parameter for number of read event parameters.
 *
 *  @retval PAPI_OK or on error something else
 */
int APAPI_read_event_ops_csv(char *input, char delimiter, struct apapi_event_ops **events_out, int *num_events_out);

/** @class APAPI_apapi_eventset_find_event
 *  @brief Searches for the index of an event inside an apapi_eventset
 *
 *  @param struct apapi_eventset *set
 *  	apapi_eventset to search in
 *  @param char *name
 *		Name of the event of interest
 *
 *  @retval -1 if not found, else the event's index
 */
int APAPI_apapi_eventset_find_event(struct apapi_eventset *set, char *name);

/** @class APAPI_read_environ_defaults
 *  @brief Checks if the APAPI_DEFAULTS environment variable is set.
 *		If the variable is set it's value is interpreted as file name.
 *		The file is read and interpreted as CSV with ',' delimiter for event parameter defaults.
 *		See APAPI_read_event_ops_csv
 *
 *  @param char **buffer
 *  	Output parameter for read text file
 *		char* fields in events_out point to the contents of the buffer
 *		NULL on error
 *  @param struct apapi_event_ops **events_out
 *  	Output parameter for array of event parameters
 *		char* fields in events_out point to the contents of the buffer
 *		NULL on error
 *  @param int *num_events_out
 *  	Output parameter for number of read events
 *
 *  @retval If the file was found and read then returns the return value of APAPI_read_event_ops_csv.
 *			If reading of the file failed then returns -1.
 *	@see APAPI_read_event_ops_csv
 */
int APAPI_read_environ_defaults(char **buffer, struct apapi_event_ops **events_out, int *num_events_out);

#endif
