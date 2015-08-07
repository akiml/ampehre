/*
 * unixsockethandler.h
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
 * created: 3/04/15
 * version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
 */

#ifndef __UNIXSOCKETHANDLER_H__
#define __UNIXSOCKETHANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/types.h>

// SERVER
typedef void USH_SERVER;
void ush_server_init(USH_SERVER **server, char *socket_name, int max_connections);
void ush_server_fini(USH_SERVER **server);
void ush_server_start(USH_SERVER *server);
void ush_server_stop(USH_SERVER *server);
int ush_server_accept(USH_SERVER *server);
void ush_server_close(USH_SERVER *server);
ssize_t ush_server_send(USH_SERVER *server, void *buffer, size_t size_data);
ssize_t ush_server_recv(USH_SERVER *server, void *buffer, size_t size_buffer);

// CLIENT
typedef void USH_CLIENT;
void ush_client_init(USH_CLIENT **client, char *socket_name);
void ush_client_fini(USH_CLIENT **client);
void ush_client_start(USH_CLIENT *client);
void ush_client_stop(USH_CLIENT *client);
int ush_client_connect(USH_CLIENT *client);
ssize_t ush_client_send(USH_CLIENT *client, void *buffer, size_t size_data);
ssize_t ush_client_recv(USH_CLIENT *client, void *buffer, size_t size_buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UNIXSOCKETHANDLER_H__ */
