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

#include "../../../include/unixsockethandler.h"

#define BUFFER_SIZE 1023

int main(int argc, char **argv);
static void client(char *socket_name);
static void server(char *socket_name);

static void client(char *socket_name) {
	USH_CLIENT *client = NULL;
	
	char buffer[BUFFER_SIZE+1];
	memset(buffer, 0, BUFFER_SIZE+1);
	
	// init client
	ush_client_init(&client, socket_name);
	ush_client_start(client);
	
	// main client loop
	ush_client_connect(client);
	
	snprintf(buffer, BUFFER_SIZE, "Greetings from the client (PID: %i).\n", getpid());
	int msg_len	= strlen(buffer);
	/*
	 * Usually, you should loop the send function to ensure that all data are sent.
	 * Accumulate the size data stored in msg_sent and break the loop if msg_len == msg_sent.
	 */
	int msg_sent = ush_client_send(client, buffer, msg_len);
	if (msg_len != msg_sent) {
		fprintf(stderr, "CLIENT Error: Could not send all data!\n");
		exit(EXIT_FAILURE);
	}
	memset(buffer, 0, BUFFER_SIZE+1);
	
	/*
	 * Usually, you should loop the recv function to ensure that all data are received.
	 * Loop recv as long as msg_len != 0.
	 */
	msg_len = ush_client_recv(client, buffer, BUFFER_SIZE);
	if (msg_len == 0) {
		fprintf(stderr, "CLIENT Error: Could not receive all data!\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "CLIENT recv from SERVER: %s", buffer);
	memset(buffer, 0, BUFFER_SIZE+1);
	
	// destroy client
	ush_client_stop(client);
	ush_client_fini(&client);
}

static void server(char *socket_name) {
	USH_SERVER *server = NULL;
	
	char buffer[BUFFER_SIZE+1];
	memset(buffer, 0, BUFFER_SIZE+1);
	
	// init server
	ush_server_init(&server, socket_name, 1);
	ush_server_start(server);
	
	// main server loop
	ush_server_accept(server);
	
	/*
	 * Usually, you should loop the recv function to ensure that all data are received.
	 * Loop recv as long as msg_len != 0.
	 */
	int msg_len = ush_server_recv(server, buffer, BUFFER_SIZE);
	if (msg_len == 0) {
		fprintf(stderr, "SERVER Error: Could not receive all data!\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "SERVER recv from CLIENT: %s", buffer);
	memset(buffer, 0, BUFFER_SIZE+1);
	
	snprintf(buffer, BUFFER_SIZE, "Greetings from the server (PID: %i).\n", getpid());
	msg_len	= strlen(buffer);
	/*
	 * Usually, you should loop the send function to ensure that all data are sent.
	 * Accumulate the size data stored in msg_sent and break the loop if msg_len == msg_sent.
	 */
	int msg_sent = ush_server_send(server, buffer, msg_len);
	if (msg_len != msg_sent) {
		fprintf(stderr, "SERVER Error: Could not send all data!\n");
		exit(EXIT_FAILURE);
	}
	memset(buffer, 0, BUFFER_SIZE+1);
	
	ush_server_close(server);
	
	// destroy server
	ush_server_stop(server);
	ush_server_fini(&server);
}

int main(int argc, char **argv) {
	char *socket_name = "/tmp/socket_ush_example";
	
	pid_t pid = fork();
	if (pid == 0) {
		fprintf(stdout, "CLIENT: I am the child.  My PID is %i.\n", getpid());
		client(socket_name);
	} else if (pid > 0) {
		fprintf(stdout, "SERVER: I am the parent. My PID is %i.\n", getpid());
		server(socket_name);
	} else {
		fprintf(stderr, "Error: I couldn't fork!\n");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
