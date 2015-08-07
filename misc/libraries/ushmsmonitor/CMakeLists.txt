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
# created: 3/05/15
# version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
#

set(USHMSMONITOR_SERVER_SOURCES ushmsmonitor_server.c)
add_library(ushmsmonitor_server SHARED ${USHMSMONITOR_SERVER_SOURCES})
target_link_libraries(ushmsmonitor_server unix_socket_handler)
install(TARGETS ushmsmonitor_server LIBRARY DESTINATION lib)

set(USHMSMONITOR_CLIENT_SOURCES ushmsmonitor_client.c)
add_library(ushmsmonitor_client SHARED ${USHMSMONITOR_CLIENT_SOURCES})
target_link_libraries(ushmsmonitor_client unix_socket_handler)
install(TARGETS ushmsmonitor_client LIBRARY DESTINATION lib)
