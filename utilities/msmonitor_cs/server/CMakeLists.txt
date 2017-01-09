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

include_directories(include/)
include_directories(../../../include/)
include_directories(../include/)
include_directories(../../msmonitor/data/)

set(DATA_SOURCES
	src/CComS.cpp
	src/CMeasure.cpp
	src/CServer.cpp
	src/CProtocolS.cpp
	../include/CProtocol.cpp
	../include/utils.cpp
)

add_library(server SHARED ${DATA_SOURCES})
link_directories(${CMAKE_BINARY_DIR}/..)


add_executable(msm_srv main.cpp)
target_link_libraries(msm_srv server)
target_link_libraries(msm_srv msmonitor_data)
target_link_libraries(msm_srv ${MS_LIBRARY})



install(TARGETS msm_srv RUNTIME DESTINATION bin)




