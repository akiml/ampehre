#
# CMakeLists.txt
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 3/02/15
# version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
#

set(SOCKET_SOURCES CUnixSocketServer.cpp CUnixSocketClient.cpp server.cpp client.cpp)

add_library(unix_socket_handler SHARED ${SOCKET_SOURCES})

install(TARGETS unix_socket_handler LIBRARY DESTINATION lib)
