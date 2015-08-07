/*
 * ushmsmonitor_server.c
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

static int ushms_server_check_request_msg(USHMS_SERVER *server, int *pid, int *id, ssize_t data_received);
static void ushms_server_build_response_msg(USHMS_SERVER *server, int pid, int id);
static void ushms_server_reset_buffer(USHMS_SERVER *server);

void ushms_server_init(USHMS_SERVER **server) {
	*server = NULL;
	
	*server = malloc(sizeof(USHMS_SERVER));
	if (*server == NULL) {
		LOG_ERROR("Cannot allocate memory.");
		exit(EXIT_FAILURE);
	}
	memset(*server, 0, sizeof(USHMS_SERVER));
	
	snprintf((*server)->socket_name, UNIX_PATH_MAX-1, "%s_%s", USHMS_SOCKET_NAME, getenv("USER"));
	(*server)->max_connections	= 5;
	(*server)->size_data		= 0;
	
	ush_server_init(&((*server)->server), (*server)->socket_name, (*server)->max_connections);
}

void ushms_server_fini(USHMS_SERVER **server) {
	ush_server_fini(&((*server)->server));
	
	*server = NULL;
}

void ushms_server_start(USHMS_SERVER *server) {
	ush_server_start(server->server);
}

void ushms_server_stop(USHMS_SERVER *server) {
	ush_server_stop(server->server);
}

int ushms_server_comm(USHMS_SERVER *server, int *client_id, int *client_signal, int *client_pid) {
	int accepted	= 0;
	int signal		= 0;
	int pid			= 0;
	int id			= 0;
	
	ushms_server_reset_buffer(server);
	
	accepted = ush_server_accept(server->server);
	if (!accepted) {
		ushms_server_reset_buffer(server);
		return accepted;
	}
	
	ssize_t data_received = ush_server_recv(server->server, server->buffer, SIZE_BUFFER);
	signal = ushms_server_check_request_msg(server, &pid, &id, data_received);
	
	ushms_server_reset_buffer(server);
	
	ushms_server_build_response_msg(server, pid, id);
	ssize_t data_sent = ush_server_send(server->server, server->buffer, server->size_data);
	if (data_sent < server->size_data) {
		LOG_ERROR("Could not send all data.");
		exit(EXIT_FAILURE);
	}
	
	ush_server_close(server->server);
	
	ushms_server_reset_buffer(server);
	
	*client_id		= id;
	*client_signal	= signal;
	*client_pid		= pid;
	
	return accepted;
}

static int ushms_server_check_request_msg(USHMS_SERVER *server, int *pid, int *id, ssize_t data_received) {
	int signal = 0;
	
	int major		= 0;
	int minor		= 0;
	int revision	= 0;
	
	int found = sscanf(server->buffer, "MSMONITOR:%i.%i.%i:%i:%i:%i",
					   &major, &minor, &revision, pid, id, &signal);
	if (found != 6) {
		LOG_ERROR("Bad request message received from client.");
		exit(EXIT_FAILURE);
	}
	
	if (major != MS_MAJOR_VERSION || minor != MS_MINOR_VERSION || revision != MS_REVISION_VERSION) {
		LOG_ERROR("Protocol version mismatch.");
		exit(EXIT_FAILURE);
	}
	
	return signal;
}

static void ushms_server_build_response_msg(USHMS_SERVER *server, int pid, int id) {
	snprintf(server->buffer, SIZE_BUFFER, "MSMONITOR:%i.%i.%i:%i:%i:%i",
			 MS_MAJOR_VERSION, MS_MINOR_VERSION, MS_REVISION_VERSION, pid, id, 1);
	server->size_data = strlen(server->buffer);
}

static void ushms_server_reset_buffer(USHMS_SERVER *server) {
	memset(server->buffer, 0, SIZE_BUFFER+1);
	server->size_data = 0;
}
