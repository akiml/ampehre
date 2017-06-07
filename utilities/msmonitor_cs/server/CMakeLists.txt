#
# CMakeLists.txt
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Ahmad El-Ali (aelali@mail.upb.de)
# created: 10/12/16
# version: 0.8.0 - extend libmeasure and add application for online monitoring
#

cmake_minimum_required(VERSION 2.6)

include_directories(include)
include_directories(../common)
include_directories(${CMAKE_SOURCE_DIR}/utilities/msmonitor/data)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(MSMONITOR_SERVER_SOURCES
	src/CMeasure.cpp
	src/CServer.cpp
	src/CProtocolS.cpp
	../common/CComAbstract.cpp
	../common/CComServer.cpp
	../common/CProtocol.cpp
	../common/utils.cpp
	main.cpp
)

add_executable(msmonitor_server ${MSMONITOR_SERVER_SOURCES})
target_link_libraries(msmonitor_server msmonitor_data ${MS_LIBRARY})

install(TARGETS msmonitor_server RUNTIME DESTINATION bin)
