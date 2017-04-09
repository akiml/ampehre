#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //getopt
#include <wait.h>
#include <signal.h>
#include <time.h>
#include "apapi.h"
#include "atime_papi.h"
//#define DEBUG

void help(char* appname){
    printf("%s FILENAME [ARGUMENTS]\n", appname);
    printf("Executes and measures the binary FILENAME using the PAPI library.\n");
}

char* optional_events_file = NULL;

int main(int argc, char *argv[]) {

    int retv;
    int program_argc = 0;
    char **program_argv;

    int c = 0;

    while((c = getopt (argc, argv, "+e:h?")) != -1) {
        switch(c) {
            case 'e':
                optional_events_file = optarg;
            break;
            case 'h':
            case '?':
                help(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                help(argv[0]);
                exit(EXIT_FAILURE);
        };
    };

    if (optind == argc) {
        // Only option parameters, program name is missing
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    // prepare program arguments
    program_argc = argc-optind;
    program_argv = calloc(program_argc+1, sizeof(char*));
    int i;
    for (i=0; i<program_argc; i++) {
        program_argv[i] = argv[optind + i];
            if (i==0) {
                printf("Program: ");
            }
            printf("%s ", program_argv[i]);
            if (i+1==program_argc) {
                printf("\n");
            }
    }


    // run the program
    pid_t parent_pid = getpid();
    pid_t child_pid;

    child_pid = fork();
    /* CHILD */
    if (child_pid == 0) {

        sigset_t signal;
        int sig;
        sigemptyset(&signal);
        sigaddset(&signal, SIGUSR1);

        sigprocmask(SIG_BLOCK, &signal, NULL);


        sigwait(&signal, &sig);

        execvp(program_argv[0], program_argv);

        exit(EXIT_FAILURE);
    }

    /* PARENT */
    struct timespec tosleep;
    tosleep.tv_sec = 0;
    tosleep.tv_nsec = 3000000;
    do {
        retv = nanosleep(&tosleep, &tosleep);
    } while(retv != 0);

    papi_init(optional_events_file);

    papi_start();

    kill(child_pid, SIGUSR1);
   
    int child_status;
    do {
        retv = waitpid(child_pid, &child_status, 0);
        //printf("waitpid cpid %d retv %d status %d %d %d\n", child_pid, retv, WIFEXITED(child_status), WIFSIGNALED(child_status), WTERMSIG(child_status));
    } while(!WIFEXITED(child_status) && !WIFSIGNALED(child_status));

    papi_stop();

    printf("done\n");
 


    return EXIT_SUCCESS;
}
