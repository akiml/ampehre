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


set(PAPI_SOURCE_PATH ${CMAKE_SOURCE_DIR}/papi/src)
set(PAPI_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/papi/src)

## build PAPI as custom target
#add_custom_command(
#	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#	OUTPUT ${PAPI_LIBRARY_PATH}/libpapi.so
##	COMMAND mkdir -p papi/build
##	COMMAND echo '-- <<<<'
##	COMMAND printenv
##	COMMAND echo '-- >>>>'
##	COMMAND echo $(PWD)
##	COMMAND cd papi/src && echo ich bin hier: $(PWD)
##	COMMAND cd papi/src && echo ich bin hier: `pwd`
##	COMMAND cd papi/src && echo ich bin hier: "`pwd`"
##	COMMAND echo "PAPI make clean"
##	COMMAND cd papi/src && make clean
##	COMMAND cd papi/src/components/nvml && ./configure --with-nvml-dir=/
##	COMMAND echo ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$(PWD)/papi/build"
##	COMMAND cd papi/src && ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$(PWD)/papi/build"
##	COMMAND cd papi/src && echo foo: "`pwd && cd ../build/ && pwd`"
##	COMMAND cd papi/src && ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=`cd ../build/ && pwd`"
##	COMMAND export FOO="bar" && echo "${FOO}"
##	COMMAND echo "$(cd .. && pwd)"
##	COMMAND cd papi/src && echo ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=`sh -c 'cd .. && pwd'`"
##	COMMAND set PREFIX=$(PWD)/papi/build && cd papi/src && ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$(PREFIX)"
##	COMMAND echo "PAPI make -j8"
##	COMMAND cd papi/src && make -j8
##	COMMAND echo "PAPI make install"
##	COMMAND cd papi/src && make install
#	COMMAND cd papi && ./cmake_build.sh "${CMAKE_INSTALL_PREFIX}"
#	COMMENT "Build PAPI library"
#)
#
#add_custom_target(makepapi
##	DEPENDS ${PAPI_LIBRARY_PATH}/libpapi.so
#	COMMENT "Build PAPI library"
#	SOURCES ${PAPI_LIBRARY_PATH}/libpapi.so
#)

# build PAPI as custom target
#add_custom_command(
#	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#	OUTPUT ${PAPI_LIBRARY_PATH}/libpapi.so
#	COMMAND mkdir -p papi/build
#	COMMAND echo '-- <<<<'
#	COMMAND printenv
#	COMMAND echo '-- >>>>'
#	COMMAND echo $(PWD)
#	COMMAND cd papi/src && echo ich bin hier: $(PWD)
#	COMMAND cd papi/src && echo ich bin hier: `pwd`
#	COMMAND cd papi/src && echo ich bin hier: "`pwd`"
#	COMMAND echo "PAPI make clean"
#	COMMAND cd papi/src && make clean
#	COMMAND cd papi/src/components/nvml && ./configure --with-nvml-dir=/
#	COMMAND echo ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$(PWD)/papi/build"
#	COMMAND cd papi/src && ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$(PWD)/papi/build"
#	COMMAND cd papi/src && echo foo: "`pwd && cd ../build/ && pwd`"
#	COMMAND cd papi/src && ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=`cd ../build/ && pwd`"
#	COMMAND export FOO="bar" && echo "${FOO}"
#	COMMAND echo "$(cd .. && pwd)"
#	COMMAND cd papi/src && echo ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=`sh -c 'cd .. && pwd'`"
#	COMMAND set PREFIX=$(PWD)/papi/build && cd papi/src && ./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$(PREFIX)"
#	COMMAND echo "PAPI make -j8"
#	COMMAND cd papi/src && make -j8
#	COMMAND echo "PAPI make install"
#	COMMAND cd papi/src && make install
#	COMMAND cd papi && ./cmake_build.sh "${CMAKE_INSTALL_PREFIX}"
#	COMMENT "Build PAPI library"
#)

add_custom_target(makepapi
#	DEPENDS ${PAPI_LIBRARY_PATH}/libpapi.so
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND cd papi && ./cmake_build.sh "${CMAKE_INSTALL_PREFIX}"
	COMMENT "Build PAPI library"
#	SOURCES ${PAPI_LIBRARY_PATH}/libpapi.so
)


add_library(papi SHARED IMPORTED) #${PAPI_LIBRARY_PATH}/libpapi.so)
#add_library(papi SHARED ${PAPI_LIBRARY_PATH}/libpapi.so)
set_target_properties(
	papi
	PROPERTIES LINKER_LANGUAGE C
#	IMPORTED_LOCATION ${PAPI_LIBRARY_PATH}/libpapi.so
	IMPORTED_LOCATION ${PAPI_LIBRARY_PATH}/libpapi.so
	IMPORTED_SONAME libpapi.so
)
add_dependencies(papi makepapi)

## APAPI

# Full library
add_library(apapi SHARED
	apapi.c
)

# add dependency to custom PAPI target
add_dependencies(apapi papi)

# build PAPI as PRE_BUILD command to apapi
#add_custom_command(
#	TARGET apapi
#	PRE_BUILD
#	WORKING_DIRECTORY ../../../../
#	COMMAND mkdir -p papi/build
#	COMMAND echo $(PWD)
#	COMMAND cd papi/src && ./configure --with-components="" --prefix="$(PWD)/papi/build"
#	COMMAND cd papi/src && make
#	COMMENT "Build PAPI library"
#)

set(CMAKE_VERBOSE_MAKEFILE ON)
set_target_properties(apapi PROPERTIES PAPI_SOURCE_PATH ${PAPI_SOURCE_PATH})
set_target_properties(apapi PROPERTIES PAPI_LIBRARY_PATH ${PAPI_LIBRARY_PATH})
set_target_properties(apapi PROPERTIES APAPI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PAPI_SOURCE_PATH})
#find_library(PAPI_LIBRARY libpapi.so PATHS ${PAPI_LIBRARY_PATH})
message(PAPI DIR LIB FOO ${PAPI_LIBRARY_PATH})
message(PAPI DIR SOURCE BAR ${PAPI_SOURCE_PATH})
#link_directories(${PAPI_LIBRARY_PATH})
#target_link_libraries(apapi ${CMAKE_THREAD_LIBS_INIT} ${PAPI_LIBRARY_PATH} papi)
target_link_libraries(apapi ${CMAKE_THREAD_LIBS_INIT} papi )

install(TARGETS apapi LIBRARY DESTINATION lib)

