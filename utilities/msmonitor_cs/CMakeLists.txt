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
# created: 1/15/14
# version: 0.3.0 - extend libmeasure and add application for online monitoring
#

#cmake_minimum_required(VERSION 2.6)

#project(msmonitor)

#set(CMAKE_C_FLAGS "-O3 -Wall")
#set(CMAKE_CXX_FLAGS "-O3 -Wall")

link_directories(${CMAKE_BINARY_DIR}/..)

add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(gui)

set(BIN_SOURCES main.cpp)

add_executable(msmonitor ${BIN_SOURCES})
target_link_libraries(msmonitor msmonitor_gui msmonitor_data)

install(TARGETS msmonitor RUNTIME DESTINATION bin)
