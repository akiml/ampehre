/*                                                                                                                                              
 * atime.c
 * 
 * Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Alex Wiens <awiens@mail.uni-paderborn.de>  * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Alex Wiens (awiens@mail.uni-paderborn.de)
 * created: 6/01/17
 * version: 0.8.0 - initial implementation
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //getopt
#include <wait.h>
#include <signal.h>
#include <time.h>
#include "apapi.h"
#include "atime_eventlist.h"
#include "atime_papi.h"
//#define DEBUG

void help(char* appname){
	printf("%s [OPTIONS] FILENAME [ARGUMENTS]\n", appname);
	printf("Executes and measures the binary FILENAME using the PAPI library.\n");
	printf("Options:\n");
	printf(" -t interval list\t\tList with timer intervals in ns for each component\n");
	printf("                 \t\te.g. -t \"rapl:200000000:nvml:350000000\"\n");
	printf("                 \t\tDefault time interval is 100ms\n");
	printf("Environment variables:\n");
	printf("\tAPAPI_EVENTLIST\t\tPath to file with lists of events to be used\n");
	printf("\tAPAPI_EVENTOPS\t\tPath to file with event operation definitions\n");
	printf("\tAPAPI_CMPLIST\t\tList of component (short) names to be used\n");
}

int main(int argc, char *argv[]) {

	int retv;
	int program_argc = 0;
	char **program_argv;
	char *option_intervalstr = NULL;

	int c = 0;

	while((c = getopt (argc, argv, "+t:h?")) != -1) {
		switch(c) {
			case 't':
				option_intervalstr = optarg;
			break;
			case 'h':
			case '?':
				help(argv[0]);
				exit(EXIT_SUCCESS);
			default:
				help(argv[0]);
				exit(EXIT_FAILURE);
			break;
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
	//pid_t parent_pid = getpid();
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

	papi_init(option_intervalstr);

	papi_start();

	kill(child_pid, SIGUSR1);
   
	int child_status;
	do {
		retv = waitpid(child_pid, &child_status, 0);
	} while(!WIFEXITED(child_status) && !WIFSIGNALED(child_status));

	papi_stop();

	printf("done\n");
 
	free(program_argv);

	return EXIT_SUCCESS;
}
