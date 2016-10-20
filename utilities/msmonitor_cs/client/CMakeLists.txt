cmake_minimum_required(VERSION 2.6)

include_directories(include/)
include_directories(../../../include/)

set(DATA_SOURCES
	src/CComC.cpp
	src/CClient.cpp
	src/CProtocolC.cpp
)

add_library(msmonitor_client SHARED ${DATA_SOURCES})


install(TARGETS msmonitor_client LIBRARY DESTINATION lib)
