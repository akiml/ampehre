
cmake_minimum_required(VERSION 2.6)

include_directories(include/)
include_directories(../../../include/)
include_directories(../include/)

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
target_link_libraries(msm_srv ${MS_LIBRARY})



install(TARGETS msm_srv RUNTIME DESTINATION bin)




