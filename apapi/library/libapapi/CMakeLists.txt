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



# build PAPI as custom target
#add_custom_command(
#	WORKING_DIRECTORY ../../
#	OUTPUT makepapi
#	COMMAND mkdir -p papi/build
#	COMMAND echo $(PWD)
#	COMMAND cd papi/src && ./configure --with-components="" --prefix="$(PWD)/papi/build"
#	COMMAND cd papi/src && make
#	COMMENT "Build PAPI library"
#)

#add_custom_target(papi
#	DEPENDS makepapi
#	COMMENT "Build PAPI library"
#)

set(SRC_COMMON_LIBRARY
	apapi.c
)

# Full library
add_library(apapi SHARED ${SRC_COMMON_LIBRARY})

# add dependency to custom PAPI target
# add_dependencies(apapi papi)

# build PAPI as PRE_BUILD command to apapi
add_custom_command(
	TARGET apapi
	PRE_BUILD
	WORKING_DIRECTORY ../../../../
	COMMAND mkdir -p papi/build
	COMMAND echo $(PWD)
	COMMAND cd papi/src && ./configure --with-components="" --prefix="$(PWD)/papi/build"
	COMMAND cd papi/src && make
	COMMENT "Build PAPI library"
)


set(PAPI_SOURCE_PATH ../../../../papi/build/include)
set(PAPI_LIBRARY_PATH ../../../../papi/build/lib)
set_target_properties(apapi PROPERTIES PAPI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PAPI_SOURCE_PATH})
set_target_properties(apapi PROPERTIES PAPI_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PAPI_LIBRARY_PATH})
set_target_properties(apapi PROPERTIES APAPI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_library(PAPI_LIBRARY libpapi.so PATHS ${PAPI_LIBRARY_PATH})

target_link_libraries(apapi ${CMAKE_THREAD_LIBS_INIT} ${PAPI_LIBRARY})

install(TARGETS apapi LIBRARY DESTINATION lib)

