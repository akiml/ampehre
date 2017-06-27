# APAPI #

APAPI is a wrapper library for the [Performance Application Programming Interface (PAPI)][papi] library.

APAPI extends PAPI by following features:
* a timer mechanism to periodically read PAPI counters
* gathering of statistics (minimal, maximal, average and accumlated value)

## History ##

### PAPI ###

PAPI is a library to read performance counters available on your system.
Performance counters allow to measure certain system characteristics, e.g. miss-rate of the CPU cache.
PAPI's generic terminology for a measurable value is "event".
PAPI allows to be extended by "components" that offer additional events, e.g. values for specific devices.


### libmeasure and APAPI ###

APAPI was primarily build to replace libmeasure's measurement mechanism.
libapapi wraps PAPI and provides functionality that is required by libmeasure.
libms_common_apapi provides the well-known interface to libmeasure while using PAPI for measurement.
Applications that use libms_common can use libms_common_apapi to use PAPI.

## Statistics ##

Values delivered by PAPI are raw values.
There are two types of raw values:
* accumulating values are growing monotonously and overflow
* non-accumulating values are simply updated

APAPI stores the last two raw values.
If the raw value is non-accumulating then the last value is simply used as **value0**.
If the raw value is accumulating then the last two values are used to check for an overflow and fix it.
In this case the difference between the last two raw values is used as **value0**.
This way the **value0** is computed after every timer interval.

APAPI stores the last two values for **value0**.
The last two values for **value0** and it's timestamps can be used to compute **value1**.

For **value0** and **value1** statistics can be computed:
* minimal value - the smallest value encountered
* maximal value - the largest value encountered
* average value - the average value over the complete time period, every value is weighted by it's timing period
* accumulated value - the sum of all values


## Control mechanisms ##

### Defaults ###

* default_eventops.txt: defines event metadata used to convert and print results
* default_eventlist.txt: defines lists of events that should be used when measuring certain PAPI components
* default component list: defines a default list of components to be used by applications

Default component list: rapl, nvml, maxeler, micknc, ipmi

### Environment variables ###

APAPI provides methods to applications to evaluate environment variables.
* APAPI_EVENTLIST: path to file that defines alternative event lists similar to default_eventlist.txt
* APAPI_EVENTOPS: path to file that defines alternative event metadata similar to default_eventops.txt
* APAPI_CMPLIST: list of component names, e.g. APAPI_CMPLIST="nvml:maxeler:ipmi"

Debugging variables:
* APAPI_VERBOSE: libapapi will print additional messages, e.g. APAPI_VERBOSE=1
* APAPI_SOFTFAIL: libapapi will ignore certain unsuccessful actions, e.g. APAPI_VERBOSE=1



## Tools ##

### atime ###

atime uses libapapi to measure arbitrary events.

```
atime [-t intervals] program [program arguments]
```
* -t -- allows to specifiy measurement intervals in nanoseconds, e.g. -t "nvml:250000000:ipmi:3000000000" for 250ms for nvml and 3s for ipmi
* program [program arguments] -- atime will start the program with its arguments and measure while it is running

atime will use the environment variables as following:
If APAPI_CMPLIST is not specified the default component list will be used.
Only components in APAPI_CMPLIST are used.
If in the APAPI_EVENTLIST file no events for a component are specified the list from default_eventlist.txt is used.
If the list is neither in the APAPI_EVENTLIST file nor in default_eventlist.txt specified, all events from the component are used.
For a event the definition from the APAPI_EVENTOPS file is prioritized over the definition in default_eventops.csv.

### hettime ###

```
LD_PRELOAD=/path/to/libms_common_apapi.so hettime [...]
```

Use LD_PRELOAD to force hettime to use libapapi.
libms_common_apapi will use APAPI_CMPLIST only as subset of the default components supported by libmeasure (rapl, nvml, maxeler, micknc, ipmi).

## Default file specification ##

### eventlist file ###

Every line is interpreted as one event name.
Lines starting with '#' are ignored.
Empty lines are ignored.

### eventops file ###

Every line is interpreted as one event definition.
Lines starting with '#' are ignored.
Empty lines are ignored.
One definition consists of several fields delimited by a ','.
Fields should not contain unnecessary whitespace.
One definition consists of the following fields:
* event name
* operation to use to compute value1 from value0 (last two values for value0 are called sample1 (last) and sample0 (previous))
  * APAPI_OP1_SAMPLE_DIFF
    * value1 = sample1 - sample0
  * APAPI_OP1_SAMPLE1_MUL_DIFF_TIME
    * value1 = sample1 * (time1 - time0)
  * APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME
    * value1 = (sample0 + sample1) / 2.0 * (time1 - time0)
  * APAPI_OP1_DIV_DIFF_TIME
    * value1 = sample1 / (time1 - time0)
* statistics to compute for value0, single term or '|' delimited list of terms:
  * APAPI_STAT_NO - no statistics
  * APAPI_STAT_MIN
  * APAPI_STAT_MAX
  * APAPI_STAT_AVG
  * APAPI_STAT_ACC
  * APAPI_STAT_ALL
* statistics to compute for value1
* maximal raw counter value or 0 if counter is not accumulating
* name for type of value0
* unit for value0
* factor to use for value0
* name for type of value1
* unit for value1
* factor to use for value1

Example definition:
```
rapl:::PACKAGE_ENERGY:PACKAGE0,APAPI_OP1_DIV_DIFF_TIME,APAPI_STAT_ACC,APAPI_STAT_MIN|APAPI_STAT_MAX|APAPI_STAT_AVG,0xffffffff,energy,Ws,1E9,power,W,1
```



[papi]: <https://icl.utk.edu/papi/>

