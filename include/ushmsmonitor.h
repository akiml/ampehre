/*
 * ushmsmonitor.h
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

#ifndef __USHMSMONITOR_H__
#define __USHMSMONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "unixsockethandler.h"

#include <sys/types.h>
#include <sys/un.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif /* UNIX_PATH_MAX */

#define USHMS_SOCKET_NAME "/tmp/ushms_socket"
#define SIZE_BUFFER 1023

// SERVER
typedef struct __ushms_server {
	USH_SERVER *server;
	char buffer[SIZE_BUFFER+1];
	int size_data;
	char socket_name[UNIX_PATH_MAX];
	int max_connections;
} USHMS_SERVER;

void ushms_server_init(USHMS_SERVER **server);
void ushms_server_fini(USHMS_SERVER **server);
void ushms_server_start(USHMS_SERVER *server);
void ushms_server_stop(USHMS_SERVER *server);
int ushms_server_comm(USHMS_SERVER *server, int *client_id, int *client_signal, int *client_pid);

// CLIENT
typedef struct __ushms_client {
	USH_CLIENT *client;
	char buffer[SIZE_BUFFER+1];
	int size_data;
	char socket_name[UNIX_PATH_MAX];
} USHMS_CLIENT;

void ushms_client_init(USHMS_CLIENT **client);
void ushms_client_fini(USHMS_CLIENT **client);
void ushms_client_start(USHMS_CLIENT *client);
void ushms_client_stop(USHMS_CLIENT *client);
int ushms_client_comm(USHMS_CLIENT *client, int client_id, int client_signal);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __USHMSMONITOR_H__ */
