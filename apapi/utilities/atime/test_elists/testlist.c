#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //getopt
#include <wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "papi.h"
#include "apapi.h"
#include "atime_eventlist.h"
#include "atime_papi.h"
//#define DEBUG

char* optional_events_file = NULL;

int main(int argc, char *argv[]) {

    int c = 0;

    int print = 0;

    while((c = getopt (argc, argv, "+e:vh?")) != -1) {
        switch(c) {
            case 'e':
                optional_events_file = optarg;
            break;
            case 'v':
                print = 1;
            break;
            default:
                exit(EXIT_FAILURE);
        };
    };

    char *buffer;
    char **events;
    long filesize;

    int retv = 0;
    retv = read_file(optional_events_file, &buffer, &filesize);
    if (retv != 0) {
        return 1;
    }

    uint32_t event_count = 0;
    retv = read_event_list(buffer, &events, &event_count);

    if (retv != 0) {
        return 1;
    }


    int eventIx = 0;
    if (print) {
        printf("%d events\n", event_count);
        for (eventIx = 0; events[eventIx] != NULL; eventIx++) {
            printf("%s\n", events[eventIx]);
        }
    }
    
    char ***sorted_events;

    char *(components)[3] = {
        "rapl",
        "nvml",
        NULL
    };

    char **used_components;

    retv = sort_events(components, events, &used_components, &sorted_events);
    if (retv != 0) {
        return -1;
    }

    int cmpIx = 0;
    if (print) {
        printf("sorted events\n");
        for(cmpIx = 0; components[cmpIx] != NULL; cmpIx++) {
            printf("component %s\n", components[cmpIx]);
            for(eventIx = 0; sorted_events[cmpIx][eventIx] != NULL; eventIx++) {
                printf("  %s\n", sorted_events[cmpIx][eventIx]);
            }
        }
    }

    free(buffer);
    free(events);
    free_sorted_events(&used_components, &sorted_events);


    return EXIT_SUCCESS;
}

