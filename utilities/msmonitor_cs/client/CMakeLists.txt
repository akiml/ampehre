cmake_minimum_required(VERSION 2.6)

include_directories(include/)
include_directories(../include/)


set(DATA_SOURCES
	src/CComC.cpp
	src/CClient.cpp
	src/CProtocolC.cpp
	../include/CProtocol.cpp
	../include/utils.cpp

)

add_library(client SHARED ${DATA_SOURCES})

add_executable(msm_client main.cpp)
target_link_libraries(msm_client client)




