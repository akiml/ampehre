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

char* optional_events_file = NULL;
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


int main(int argc, char *argv[]) {

    int c = 0;

    while((c = getopt (argc, argv, "+e:h?")) != -1) {
        switch(c) {
            case 'e':
                optional_events_file = optarg;
            break;
            default:
                exit(EXIT_FAILURE);
        };
    };

    papi_read_events_file(optional_events_file, &(event_defaults));

    return EXIT_SUCCESS;
}

