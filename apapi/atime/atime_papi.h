#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "papi.h"
#include "apapi.h"

int RAPL_DEFAULT_SET_NUM = 28;
char *RAPL_DEFAULT_SET[] = {
"rapl:::THERMAL_SPEC_CNT:PACKAGE0",
"rapl:::THERMAL_SPEC_CNT:PACKAGE1",
"rapl:::MINIMUM_POWER_CNT:PACKAGE0",
"rapl:::MINIMUM_POWER_CNT:PACKAGE1",
"rapl:::MAXIMUM_POWER_CNT:PACKAGE0",
"rapl:::MAXIMUM_POWER_CNT:PACKAGE1",
"rapl:::MAXIMUM_TIME_WINDOW_CNT:PACKAGE0",
"rapl:::MAXIMUM_TIME_WINDOW_CNT:PACKAGE1",
"rapl:::PACKAGE_ENERGY_CNT:PACKAGE0",
"rapl:::PACKAGE_ENERGY_CNT:PACKAGE1",
"rapl:::DRAM_ENERGY_CNT:PACKAGE0",
"rapl:::DRAM_ENERGY_CNT:PACKAGE1",
"rapl:::PP0_ENERGY_CNT:PACKAGE0",
"rapl:::PP0_ENERGY_CNT:PACKAGE1",
"rapl:::THERMAL_SPEC:PACKAGE0",
"rapl:::THERMAL_SPEC:PACKAGE1",
"rapl:::MINIMUM_POWER:PACKAGE0",
"rapl:::MINIMUM_POWER:PACKAGE1",
"rapl:::MAXIMUM_POWER:PACKAGE0",
"rapl:::MAXIMUM_POWER:PACKAGE1",
"rapl:::MAXIMUM_TIME_WINDOW:PACKAGE0",
"rapl:::MAXIMUM_TIME_WINDOW:PACKAGE1",
"rapl:::PACKAGE_ENERGY:PACKAGE0",
"rapl:::PACKAGE_ENERGY:PACKAGE1",
"rapl:::DRAM_ENERGY:PACKAGE0",
"rapl:::DRAM_ENERGY:PACKAGE1",
"rapl:::PP0_ENERGY:PACKAGE0",
"rapl:::PP0_ENERGY:PACKAGE1",
NULL
};

int NVML_DEFAULT_SET_NUM = 6;
char *NVML_DEFAULT_SET[] = {
"nvml:::Tesla_K20c:fan_speed",
"nvml:::Tesla_K20c:total_memory",
"nvml:::Tesla_K20c:unallocated_memory",
"nvml:::Tesla_K20c:allocated_memory",
"nvml:::Tesla_K20c:power",
"nvml:::Tesla_K20c:temperature",
NULL
};

char *my_events[] = {
"rapl:::PACKAGE_ENERGY_CNT:PACKAGE0",
NULL
};

void quit(unsigned int line, char* msg){
    printf("%d %s\n", line, msg);
    exit(EXIT_FAILURE);
}

long gettime() {
    struct timespec result;
    clock_gettime(CLOCK_MONOTONIC_RAW, &result);
    return result.tv_sec* 1000000000 + result.tv_nsec;
}

struct apapi_event_ops *event_defaults = NULL;
char *event_file_buffer = NULL;

void papi_read_events_file(char* filename, struct apapi_event_ops **events_output) {
    if (filename == NULL) {
        return;
    }
    int retv;
    FILE *eventfile;
    // try to open file
    eventfile = fopen(filename, "r");
    if (eventfile == NULL && errno != 0) {
        printf("Unable to open eventfile.\n");
        return;
    }
    
    // check file size
    long filesize = 0;
    retv = fseek(eventfile, 0L, SEEK_END);
    if (retv == -1 && errno != 0) {
        printf("Unable to seek eventfile.\n");
        fclose(eventfile);
        return;
    }
    filesize = ftell(eventfile);
    if (filesize == -1 && errno != 0) {
        printf("Unable to tell eventfile.\n");
        fclose(eventfile);
        return;
    }
    rewind(eventfile);
    if (filesize > 10000000L) {
        printf("Event file too big.\n");
        fclose(eventfile);
        return;
    }
    char *event_file_buffer = calloc(1, filesize+1);
    if (event_file_buffer == NULL) {
        printf("Not enough memory for event file.\n");
        fclose(eventfile);
        return;
    }
    size_t read;
    read = fread(event_file_buffer, filesize, 1, eventfile);
    if (read != 1) {
        printf("Reading event file failed.\n.");
        free(event_file_buffer);
        fclose(eventfile);
        return;
    }

    printf("Read event file.\n");

    int num_events;
    retv = APAPI_read_event_ops_csv(event_file_buffer, ',', events_output, &num_events);
    if (retv == PAPI_OK) {
        printf("Read %d events.\n", num_events);
    } else {
        printf("Reading events failed.\n");
    }


    fclose(eventfile);
}

struct apapi_eventset *set;
struct apapi_timer *timer;

struct apapi_eventset *nvml_set;
struct apapi_timer *nvml_timer;

void measure(void** arg){
    printf("%ld measure\n", gettime());
    printf("%020lld %020lld\n", set->previous_counters[0], set->current_counters[0]);
    printf("%020lld %020lld\n", set->previous_samples[0], set->current_samples[0]);
    
}


void papi_init(char *optional_events_file){

    int retv;

    papi_read_events_file(optional_events_file, &(event_defaults));

    retv = PAPI_library_init(PAPI_VER_CURRENT);
    if (retv != PAPI_VER_CURRENT) {
        quit(__LINE__, "Failed to initialize PAPI library");
    }

    int num_components = PAPI_num_components();
    const PAPI_component_info_t *component_infos[num_components];
    int i;
    for(i=0; i<num_components; i++) {
        component_infos[i] = PAPI_get_component_info(i);
        printf("%d: %s %s %d\n", i, component_infos[i]->short_name, component_infos[i]->name, component_infos[i]->num_native_events);
    }

    // create rapl aeventset
    int cidx;
    cidx = PAPI_get_component_index("rapl");
    //retv = APAPI_init_apapi_eventset_cmp(&set, cidx);
    //retv = APAPI_init_apapi_eventset_cmp(&set, cidx, my_events);
    retv = APAPI_init_apapi_eventset_cmp(&set, cidx, NULL, event_defaults);
    cidx = PAPI_get_component_index("nvml");
    retv = APAPI_init_apapi_eventset_cmp(&nvml_set, cidx, NULL, event_defaults);
    #ifdef DEBUG
    printf("atime %d %d\n", __LINE__, retv);
    printf("atime %d events used\n", set->num_events);
    #endif

    retv = APAPI_create_timer(&timer, 0, 100000000, NULL, NULL, set);
//    retv = APAPI_create_timer(&timer, 0, 100000000, set->EventSet, &measure, NULL, NULL);
    retv = APAPI_create_timer(&nvml_timer, 0, 133000000, NULL, NULL, nvml_set);
    #ifdef DEBUG
    printf("atime %d %d\n", __LINE__, retv);
    #endif
}

void papi_start(){
    printf("papi_start\n");
    int retv;
    retv = APAPI_start_timer(timer);
    retv = APAPI_start_timer(nvml_timer);
    #ifdef DEBUG
    printf("atime %d %d\n", __LINE__, retv);
    #endif
}

void papi_stop(){
    int retv;
    retv = APAPI_stop_timer(timer);
    retv = APAPI_stop_timer(nvml_timer);
    #ifdef DEBUG
    printf("atime %d %d\n", __LINE__, retv);
    #endif

    retv = APAPI_destroy_timer(&timer);
    retv = APAPI_destroy_timer(&nvml_timer);
    #ifdef DEBUG
    printf("atime %d %d\n", __LINE__, retv);
    #endif
   
    APAPI_print_apapi_eventset(set);
    APAPI_print_apapi_eventset(nvml_set);
 
    printf("Alles ok\n");
    retv = APAPI_destroy_apapi_eventset(&set);
    retv = APAPI_destroy_apapi_eventset(&nvml_set);
    #ifdef DEBUG
    printf("atime %d %d\n", __LINE__, retv);
    #endif


   
    PAPI_shutdown();
    printf("papi stop\n");
}


