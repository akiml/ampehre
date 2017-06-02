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

char* test_optional_events_file = NULL;
struct apapi_event_ops *test_event_defaults = NULL;
char *test_event_file_buffer = NULL;

int main(int argc, char *argv[]) {

    int c = 0;

    while((c = getopt (argc, argv, "+e:h?")) != -1) {
        switch(c) {
            case 'e':
                test_optional_events_file = optarg;
            break;
            default:
                exit(EXIT_FAILURE);
        };
    };

    papi_read_events_file(test_optional_events_file, &(test_event_defaults));

    if (test_optional_events_file == NULL)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}

