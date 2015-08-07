/*
 * ushmsmonitor_client.c
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

#include "../../../include/ushmsmonitor.h"
#include "../unixsockethandler/common.hpp"
#include "../../../include/ms_version.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static void ushms_client_build_request_msg(USHMS_CLIENT *client, int pid, int id, int signal);
static void ushms_client_check_response_msg(USHMS_CLIENT *client, int pid, int id, ssize_t data_received);
static void ushms_client_reset_buffer(USHMS_CLIENT *client);

void ushms_client_init(USHMS_CLIENT **client) {
	*client = NULL;
	
	*client = malloc(sizeof(USHMS_CLIENT));
	if (*client == NULL) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	memset(*client, 0, sizeof(USHMS_CLIENT));
	
	snprintf((*client)->socket_name, UNIX_PATH_MAX-1, "%s_%s", USHMS_SOCKET_NAME, getenv("USER"));
	(*client)->size_data		= 0;
	
	ush_client_init(&((*client)->client), (*client)->socket_name);
}

void ushms_client_fini(USHMS_CLIENT **client) {
	ush_client_fini(&((*client)->client));
	
	*client = NULL;
}

void ushms_client_start(USHMS_CLIENT *client) {
	ush_client_start(client->client);
}

void ushms_client_stop(USHMS_CLIENT *client) {
	ush_client_stop(client->client);
}

int ushms_client_comm(USHMS_CLIENT *client, int client_id, int client_signal) {
	int connected	= 0;
	int pid			= getpid();
	
	ushms_client_reset_buffer(client);
	
	ushms_client_build_request_msg(client, pid, client_id, client_signal);
	
	connected = ush_client_connect(client->client);
	if (!connected) {
		ushms_client_reset_buffer(client);
		return connected;
	}
	
	ssize_t data_sent = ush_client_send(client->client, client->buffer, client->size_data);
	if (data_sent < client->size_data) {
		LOG_ERROR("Could not send all data.");
		exit(EXIT_FAILURE);
	}
	
	ushms_client_reset_buffer(client);
	
	ssize_t data_received = ush_client_recv(client->client, client->buffer, SIZE_BUFFER);
	ushms_client_check_response_msg(client, pid, client_id, data_received);
	
	ushms_client_reset_buffer(client);
	
	return connected;
}

static void ushms_client_build_request_msg(USHMS_CLIENT *client, int pid, int id, int signal) {
	snprintf(client->buffer, SIZE_BUFFER, "MSMONITOR:%i.%i.%i:%i:%i:%i",
			 MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, pid, id, signal);
	client->size_data = strlen(client->buffer);
}

static void ushms_client_check_response_msg(USHMS_CLIENT *client, int pid, int id, ssize_t data_received) {
	char local_buffer[SIZE_BUFFER+1];
	memset(local_buffer, 0, SIZE_BUFFER+1);
	
	snprintf(local_buffer, SIZE_BUFFER, "MSMONITOR:%i.%i.%i:%i:%i:", MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, pid, id);
	
	int length = (strlen(local_buffer) > data_received) ? data_received : strlen(local_buffer);
	
	int compare = strncmp(client->buffer, local_buffer, length);
	if (compare != 0 || length < strlen(local_buffer)) {
		LOG_ERROR("Bad response message received from server or protocol version mismatch.");
		exit(EXIT_FAILURE);
	}
	
	int result = atoi((client->buffer)+strlen(local_buffer));
	if (result != 1) {
		LOG_ERROR("Server could not handle request message.");
		exit(EXIT_FAILURE);
	}
}

static void ushms_client_reset_buffer(USHMS_CLIENT *client) {
	memset(client->buffer, 0, SIZE_BUFFER+1);
	client->size_data = 0;
}
