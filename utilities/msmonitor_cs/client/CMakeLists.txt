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

include_directories(msmonitor/include/)
include_directories(../include/)


set(DATA_SOURCES
	msmonitor/src/CComC.cpp
	msmonitor/src/CClient.cpp
	msmonitor/src/CProtocolC.cpp
	../include/CProtocol.cpp
	../include/utils.cpp

)

add_library(client SHARED ${DATA_SOURCES})

#add_executable(msm_client_test main.cpp)
#target_link_libraries(msm_client_test client)




