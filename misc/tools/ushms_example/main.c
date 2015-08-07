/*
 * main.c
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 3/05/15
 * version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include "../../../include/ushmsmonitor.h"

int main(int argc, char **argv);
static void print_help(char **argv);
static int check_string_is_number(char *argument);

static void client(int id, int signal, int number_of_communications);
static void server(int number_of_communications);
static void client_server(int id, int signal, int number_of_communications);

static void client(int id, int signal, int number_of_communications) {
	USHMS_CLIENT *client = NULL;
	
	ushms_client_init(&client);
	
	int i;
	for (i=0; i<number_of_communications; ++i) {
		ushms_client_start(client);
		
		ushms_client_comm(client, id, signal);
		
		ushms_client_stop(client);
	}
	
	ushms_client_fini(&client);
}

static void server(int number_of_communications) {
	USHMS_SERVER *server = NULL;
	
	ushms_server_init(&server);
	ushms_server_start(server);
	
	int i;
	for (i=0; i<number_of_communications; ++i) {
		int client_signal	= 0;
		int client_id		= 0;
		int client_pid		= 0;
		
		ushms_server_comm(server, &client_id, &client_signal, &client_pid);
		
		fprintf(stdout, "SERVER recv from CLIENT: pid: %i, id: %i, signal: %i\n",
				client_pid, client_id, client_signal);
	}
	
	ushms_server_stop(server);
	ushms_server_fini(&server);
}

static void client_server(int id, int signal, int number_of_communications) {
	pid_t pid = fork();
	if (pid == 0) {
		fprintf(stdout, "CLIENT: I am the child.  My PID is %i.\n", getpid());
		client(id, signal, number_of_communications);
	} else if (pid > 0) {
		fprintf(stdout, "SERVER: I am the parent. My PID is %i.\n", getpid());
		server(number_of_communications);
	} else {
		fprintf(stderr, "Error: I couldn't fork!\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	int32_t opt;
	int cflag	= 0;
	int signal	= -1;
	int id		= -1;
	
	while ((opt = getopt(argc, argv, "h?ci:s:")) != -1) {
		switch (opt) {
			case 'c':
				cflag = 1;
				break;
			case 'i':
				if (!check_string_is_number(optarg)) {
					fprintf(stderr, "The argument of option 'i' is not a (positive) number.\n");
					print_help(argv);
					return EXIT_FAILURE;
				}
				id = atoi(optarg);
				break;
			case 's':
				if (!check_string_is_number(optarg)) {
					fprintf(stderr, "The argument of option 's' is not a (positive) number.\n");
					print_help(argv);
					return EXIT_FAILURE;
				}
				signal = atoi(optarg);
				break;
			case '?':
			case 'h':
			default:
				print_help(argv);
				return EXIT_FAILURE;
		}
	}
	
	if (signal < 0) {
		fprintf(stderr, "The argument of option 's' is missing or is < 0.\n");
		print_help(argv);
		return EXIT_FAILURE;
	}
	if (id < 0) {
		fprintf(stderr, "The argument of option 'i' is missing or is < 0.\n");
		print_help(argv);
		return EXIT_FAILURE;
	}
	
	int number_of_communications = 0;
	
	if (cflag) {
		number_of_communications = 1;
		client(id, signal, number_of_communications);
	} else {
		number_of_communications = 4;
		client_server(id, signal, number_of_communications);
	}
	
	return EXIT_SUCCESS;
}

static void print_help(char **argv) {
	fprintf(stderr,
			"Usage:\n"
			"%s [-h|-?] -i ID -s SIGNAL [-c]\n\n"
			"-h        | Print this help message.\n"
			"-?        | Print this help message.\n"
			"-c        | Only the client will be active. Ensure that msmonitor\n"
			"          | runs before executing this application with the -c flag.\n"
			"-i ID     | ID to send to server.\n"
			"-s SIGNAL | Signal to send to server.\n"
			"\n",
			argv[0]
	);
}

static int check_string_is_number(char *argument) {
	int i;
	
	if (NULL == argument || strlen(argument) == 0) {
		return 0;
	}
	
	for (i=0; i<strlen(argument); ++i) {
		if (!isdigit(argument[i])) {
			return 0;
		}
	}
	
	return 1;
}
