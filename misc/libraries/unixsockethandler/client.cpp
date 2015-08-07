/*
 * client.cpp
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

#include "../../../include/unixsockethandler.h"
#include "CUnixSocketClient.hpp"

void ush_client_init(USH_CLIENT **client, char *socket_name) {
	*client = 0;
	
	*client = (USH_SERVER *)(new NUnixSocket::CUnixSocketClient(socket_name));
}

void ush_client_fini(USH_CLIENT **client) {
	delete (NUnixSocket::CUnixSocketClient *)(*client);
	
	*client = 0;
}

void ush_client_start(USH_CLIENT *client) {
	NUnixSocket::CUnixSocketClient *c = (NUnixSocket::CUnixSocketClient *)client;
	
	c->clientSocket();
}

void ush_client_stop(USH_CLIENT *client) {
	NUnixSocket::CUnixSocketClient *c = (NUnixSocket::CUnixSocketClient *)client;
	
	c->clientShutdown();
}

int ush_client_connect(USH_CLIENT *client) {
	int connected = 0;
	
	NUnixSocket::CUnixSocketClient *c = (NUnixSocket::CUnixSocketClient *)client;
	
	connected = c->clientConnect();
	
	return connected;
}

ssize_t ush_client_send(USH_CLIENT *client, void *buffer, size_t size_data) {
	NUnixSocket::CUnixSocketClient *c = (NUnixSocket::CUnixSocketClient *)client;
	ssize_t size_sent = 0;
	
	size_sent = c->clientSend(buffer, size_data);
	
	return size_sent;
}

ssize_t ush_client_recv(USH_CLIENT *client, void *buffer, size_t size_buffer) {
	NUnixSocket::CUnixSocketClient *c = (NUnixSocket::CUnixSocketClient *)client;
	ssize_t size_received = 0;
	
	size_received = c->clientRecv(buffer, size_buffer);
	
	return size_received;
}
