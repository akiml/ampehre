#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "papi.h"
#include "apapi.h"

// known components
char *known_components[] = {
    "rapl",
    "nvml",
    NULL
};

// available components
char **user_components;
char **available_components;

// sets and timers
uint32_t set_count = 0;
struct apapi_eventset **sets = NULL;
struct apapi_timer **timers = NULL;

// event list
char **all_events = NULL;
char ***sorted_events = NULL;
char *eventlist_file_buffer = NULL;

// event defaults
struct apapi_event_ops *event_defaults = NULL;
char *defaults_file_buffer = NULL;

void quit(unsigned int line, char* msg){
    printf("%d %s\n", line, msg);
    exit(EXIT_FAILURE);
}

long gettime() {
    struct timespec result;
    clock_gettime(CLOCK_MONOTONIC_RAW, &result);
    return result.tv_sec* 1000000000 + result.tv_nsec;
}


int read_file(char* filename, char **file_buffer, long *filesize) {
    if (filename == NULL) {
        return 1;
    }
    int retv;
    FILE *file;
    // try to open file
    file = fopen(filename, "r");
    if (file == NULL && errno != 0) {
        printf("Unable to open file %s.\n", filename);
        return 1;
    }
    
    // check file size
    *filesize = 0;
    retv = fseek(file, 0L, SEEK_END);
    if (retv == -1 && errno != 0) {
        printf("Unable to seek file %s.\n",filename);
        fclose(file);
        return 1;
    }
    *filesize = ftell(file);
    if (*filesize == -1 && errno != 0) {
        printf("Unable to tell file %s.\n",filename);
        fclose(file);
        return 1;
    }
    rewind(file);
    if (*filesize > 10000000L) {
        printf("File %s too big.\n",filename);
        fclose(file);
        return 1;
    }
    *file_buffer = calloc(1, *filesize+1);
    if (*file_buffer == NULL) {
        printf("Not enough memory for file %s.\n",filename);
        fclose(file);
        return 1;
    }
    size_t read;
    read = fread(*file_buffer, *filesize, 1, file);
    if (read != 1) {
        printf("Reading file %s failed.\n.",filename);
        free(*file_buffer);
        fclose(file);
        return 1;
    }

    return 0;
}

void papi_read_events_file(char *filename, char **available_components, char ***user_components, char ****sorted_events) {
    int retv = 0;
    long filesize;
    retv = read_file(filename, &eventlist_file_buffer, &filesize);
    if (retv != 0) {
    }
    uint32_t event_count;    

    retv = read_event_list(eventlist_file_buffer, &all_events, &event_count);

    retv = sort_events(available_components, all_events, user_components, sorted_events);
    if (retv == PAPI_OK) {
        printf("Read event list.\n");
    } else {
        printf("Reading event list failed.\n");
    }
}

void papi_read_defaults_file(char *filename, struct apapi_event_ops **events_output) {

    int retv;
    long filesize;

    retv = read_file(filename, &defaults_file_buffer, &filesize);

    if (retv != 0) {
        return;
    }

    printf("Read defaults file.\n");

    int num_events;
    retv = APAPI_read_event_ops_csv(defaults_file_buffer, ',', events_output, &num_events);
    if (retv == PAPI_OK) {
        printf("Read %d defaults.\n", num_events);
    } else {
        printf("Reading defaults failed.\n");
    }

}


/*
void measure(void** arg){
    printf("%ld measure\n", gettime());
    printf("%020lld %020lld\n", set->previous_counters[0], set->current_counters[0]);
    printf("%020lld %020lld\n", set->previous_samples[0], set->current_samples[0]);  
}
*/


void papi_init(char *optional_definition_file, char *optional_events_file){

    int retv;

    if (optional_definition_file != NULL) {
        papi_read_defaults_file(optional_definition_file, &(event_defaults));
    }


    retv = PAPI_library_init(PAPI_VER_CURRENT);
    if (retv != PAPI_VER_CURRENT) {
        quit(__LINE__, "Failed to initialize PAPI library");
    }

    int32_t known_cmp_count = 0;
    int knownCmpIx = 0;
    for(knownCmpIx = 0; known_components[knownCmpIx] != NULL; knownCmpIx++);
    known_cmp_count = knownCmpIx;
    available_components = calloc(sizeof(char*), known_cmp_count);



    // check available papi components
    int num_components = PAPI_num_components();
    const PAPI_component_info_t *component_infos[num_components];
    int i;
    int available_cmp_count = 0;
    for(i=0; i<num_components; i++) {
        component_infos[i] = PAPI_get_component_info(i);
        if (component_infos[i]->disabled != 0) {
            printf("PAPI component %s disabled. Reason: %s\n", component_infos[i]->short_name, component_infos[i]->disabled_reason);
            continue;
        }
        //printf("%d: %s %s %d\n", i, component_infos[i]->short_name, component_infos[i]->name, component_infos[i]->num_native_events);
        // add component to list of known and available components
        for(knownCmpIx = 0; known_components[knownCmpIx] != NULL; knownCmpIx++) {
            if (strcmp(known_components[knownCmpIx], component_infos[i]->short_name) == 0) {
                available_components[available_cmp_count] = known_components[knownCmpIx];
                available_cmp_count++;
                break;
            }
        }
    }

    char **components = available_components;
    char ***component_events = NULL;
    set_count = available_cmp_count;
    

    // check user selected events from eventlist
    if (optional_events_file != NULL) {
        papi_read_events_file(optional_events_file, available_components, &user_components, &sorted_events);
    }

    if (sorted_events != NULL) {
        uint32_t user_cmp_count = 0;
        uint32_t userCmpIx = 0;
        for(userCmpIx = 0; user_components[userCmpIx] != NULL; userCmpIx++);
        user_cmp_count = userCmpIx;
        
        components = user_components;
        component_events = sorted_events;
        set_count = user_cmp_count;
    }

    // create apapi_event_set array
    // one set per component
    sets = calloc(sizeof(struct apapi_eventset *), set_count);
    timers = calloc(sizeof(struct apapi_timer *), set_count);
    uint32_t setIx = 0;
    int cidx;
    char ** cmp_events = NULL;
    for(setIx = 0; setIx < set_count; setIx++) {
        cidx = PAPI_get_component_index(components[setIx]);
        if (component_events != NULL) {
            cmp_events = component_events[setIx];
        }

        printf("Use PAPI component %s\n", components[setIx]);
        retv = APAPI_init_apapi_eventset_cmp(&(sets[setIx]), cidx, cmp_events, event_defaults);
        #ifdef DEBUG
        printf("atime %d %d\n", __LINE__, retv);
        #endif

        retv = APAPI_create_timer(&(timers[setIx]), 0, 100000000, NULL, NULL, sets[setIx]);
        #ifdef DEBUG
        printf("atime %d %d\n", __LINE__, retv);
        #endif
    }

}

void papi_start(){
    printf("papi_start\n");
    int retv;
    int setIx = 0;
    for(setIx = 0; setIx < set_count; setIx++){
        retv = APAPI_start_timer(timers[setIx]);
        #ifdef DEBUG
        printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
        #endif
    }
}

void papi_stop(){
    int retv;

    int setIx = 0;
    for(setIx = 0; setIx < set_count; setIx++){
        retv = APAPI_stop_timer(timers[setIx]);
        #ifdef DEBUG
        printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
        #endif
    }

    for(setIx = 0; setIx < set_count; setIx++){
        retv = APAPI_destroy_timer(&(timers[setIx]));
        #ifdef DEBUG
        printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
        #endif
    }

    for(setIx = 0; setIx < set_count; setIx++){
        APAPI_print_apapi_eventset(sets[setIx]);
        
        retv = APAPI_destroy_apapi_eventset(&(sets[setIx]));
        #ifdef DEBUG
        printf("atime %d set:%d retv:%d\n", __LINE__, setIx, retv);
        #endif
    }
 
    PAPI_shutdown();
    printf("papi stop\n");
}


