

set(DATA_SOURCES
	CProtocol.cpp
)

add_library(msmonitor_protocol SHARED ${DATA_SOURCES})
target_link_libraries(msmonitor_protocol ${MS_LIBRARY})

install(TARGETS msmonitor_protocol LIBRARY DESTINATION lib)
