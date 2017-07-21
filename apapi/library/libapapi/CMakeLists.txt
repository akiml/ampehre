#
# CMakeLists.txt
#
# Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Alex Wiens <awiens@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Alex Wiens (awiens@mail.uni-paderborn.de)
# created: 6/01/17
# version: 0.8.0 - initial cmake file
#

## PAPI

set(PAPI_SOURCE_PATH ${CMAKE_SOURCE_DIR}/papi/src)
set(PAPI_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/papi/src)

add_library(pfm SHARED IMPORTED)

set_target_properties(
	pfm
	PROPERTIES LINKER_LANGUAGE C
	IMPORTED_LOCATION ${PAPI_LIBRARY_PATH}/libpfm4/lib/libpfm.so
	IMPORTED_SONAME libpfm.so
)

get_filename_component(NVML_LIB_LOC ${NVML_LIBRARY} DIRECTORY)

get_filename_component(MAX_LIB_LOC ${MAX_LIBRARY} DIRECTORY)

add_custom_target(makepapi
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND cd papi && ./cmake_build.sh "${CMAKE_INSTALL_PREFIX}" "${CMAKE_SOURCE_DIR}" "${NVML_LIB_LOC}" "${NVML_HEADER}" "${MAX_LIB_LOC}" "${MAX_HEADER}"
	COMMENT "Build PAPI library"
)

add_library(papi SHARED IMPORTED)

set_target_properties(
	papi
	PROPERTIES LINKER_LANGUAGE C
	IMPORTED_LOCATION ${PAPI_LIBRARY_PATH}/libpapi.so
	IMPORTED_SONAME libpapi.so
)
add_dependencies(papi makepapi)


## APAPI

set(CMAKE_VERBOSE_MAKEFILE ON)
# Full library
add_library(apapi SHARED
	apapi.c
)

# add dependency to apapi: custom PAPI target
add_dependencies(apapi papi pfm)
set_target_properties(apapi PROPERTIES COMPILE_DEFINITIONS DATADIR=\"${CMAKE_INSTALL_PREFIX}/share\")

set_target_properties(apapi PROPERTIES PAPI_SOURCE_PATH ${PAPI_SOURCE_PATH})
set_target_properties(apapi PROPERTIES PAPI_LIBRARY_PATH ${PAPI_LIBRARY_PATH})
set_target_properties(apapi PROPERTIES APAPI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PAPI_SOURCE_PATH})
target_link_libraries(apapi ${CMAKE_THREAD_LIBS_INIT} papi pfm ipmimeasure cjson)

install(TARGETS apapi LIBRARY DESTINATION lib)
install(FILES apapi.h DESTINATION include)
install(FILES default_eventops.csv DESTINATION share/apapi)
install(FILES default_eventlist.txt DESTINATION share/apapi)

