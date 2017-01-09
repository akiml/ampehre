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
# author: Achim Lösch (achim.loesch@upb.de)
# created: 10/12/16
# version: 0.8.0 - extend libmeasure and add application for online monitoring
#

cmake_minimum_required(VERSION 2.6)

#project(msmonitor)

#set(CMAKE_C_FLAGS "-O3 -Wall")
#set(CMAKE_CXX_FLAGS "-O3 -Wall")

include_directories(server/include/)
include_directories(client/include/)
include_directories(include/)

link_directories(${CMAKE_BINARY_DIR}/..)

add_subdirectory(client)
add_subdirectory(server)
#add_subdirectory(include)
