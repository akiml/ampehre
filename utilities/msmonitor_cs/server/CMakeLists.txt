
include_directories(include/)
include_directories(../../../include/)

set(DATA_SOURCES
	src/CComS.cpp
	src/CMeasure.cpp
	src/CServer.cpp
)

add_library(msmonitor_server SHARED ${DATA_SOURCES})

