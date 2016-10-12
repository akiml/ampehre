
include_directories(include/)
include_directories(../../../include/)

set(DATA_SOURCES
	src/CComS.cpp
	src/CMeasure.cpp
	src/CServer.cpp
	src/CProtocolS.cpp
)

add_library(msmonitor_server SHARED ${DATA_SOURCES})
target_link_libraries(msmonitor_server ${MS_LIBRARY})

install(TARGETS msmonitor_server LIBRARY DESTINATION lib)
