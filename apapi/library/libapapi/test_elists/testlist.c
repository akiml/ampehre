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

char ***user_eventlist_sorted = NULL;
char *user_eventlist_file = NULL;
char **user_eventlist_components = NULL;


int main(int argc, char *argv[]) {

    int print = 1;

    char *(components)[3] = {
        "rapl",
        "nvml",
        NULL
    };

	int retv;

	retv = APAPI_read_env_eventlist(components, &user_eventlist_file, &user_eventlist_sorted, &user_eventlist_components);

    if (print) {
        printf("sorted events\n");
		int cmpIx, eventIx;
        for(cmpIx = 0; components[cmpIx] != NULL; cmpIx++) {
            printf("component %s\n", components[cmpIx]);
            for(eventIx = 0; user_eventlist_sorted[cmpIx][eventIx] != NULL; eventIx++) {
                printf("  %s\n", user_eventlist_sorted[cmpIx][eventIx]);
            }
        }
    }


	if (retv != 0) {
		return EXIT_FAILURE;
	} else {
	    return EXIT_SUCCESS;
	}
}

