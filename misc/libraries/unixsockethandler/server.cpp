/*
 * server.cpp
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
#include "CUnixSocketServer.hpp"

void ush_server_init(USH_SERVER **server, char *socket_name, int max_connections) {
	*server = 0;
	
	*server = (USH_SERVER *)(new NUnixSocket::CUnixSocketServer(socket_name, max_connections));
}

void ush_server_fini(USH_SERVER **server) {
	delete (NUnixSocket::CUnixSocketServer *)(*server);
	
	*server = 0;
}

void ush_server_start(USH_SERVER *server) {
	NUnixSocket::CUnixSocketServer *s = (NUnixSocket::CUnixSocketServer *)server;
	
	s->serverSocket();
	s->serverBind();
	s->serverListen();
}

void ush_server_stop(USH_SERVER *server) {
	NUnixSocket::CUnixSocketServer *s = (NUnixSocket::CUnixSocketServer *)server;
	
	s->serverShutdownLocal();
}

int ush_server_accept(USH_SERVER *server) {
	int accepted = 0;
	NUnixSocket::CUnixSocketServer *s = (NUnixSocket::CUnixSocketServer *)server;
	
	accepted = s->serverAccept();
	
	return accepted;
}

void ush_server_close(USH_SERVER *server) {
	NUnixSocket::CUnixSocketServer *s = (NUnixSocket::CUnixSocketServer *)server;
	
	s->serverShutdownRemote();
}

ssize_t ush_server_send(USH_SERVER *server, void *buffer, size_t size_data) {
	NUnixSocket::CUnixSocketServer *s = (NUnixSocket::CUnixSocketServer *)server;
	ssize_t size_sent = 0;
	
	size_sent = s->serverSend(buffer, size_data);
	
	return size_sent;
}

ssize_t ush_server_recv(USH_SERVER *server, void *buffer, size_t size_buffer) {
	NUnixSocket::CUnixSocketServer *s = (NUnixSocket::CUnixSocketServer *)server;
	ssize_t size_received = 0;
	
	size_received = s->serverRecv(buffer, size_buffer);
	
	return size_received;
}
