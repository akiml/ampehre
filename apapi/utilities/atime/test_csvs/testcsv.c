#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //getopt
#include <wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "papi.h"
#include "apapi.h"
//#define DEBUG

struct apapi_event_ops *test_event_defaults = NULL;
char *test_event_file_buffer = NULL;

int main(int argc, char *argv[]) {

	int num_events;
	int retv = 0;
	retv = APAPI_read_env_eventops(&test_event_file_buffer, &test_event_defaults, &num_events);

    if (retv != 0)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}

