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

get_target_property(APAPI_SOURCE_PATH apapi APAPI_SOURCE_PATH)
get_target_property(PAPI_SOURCE_PATH apapi PAPI_SOURCE_PATH)
include_directories(${PAPI_SOURCE_PATH})
include_directories(${APAPI_SOURCE_PATH})

add_executable(atime
	atime.c
)

set(CMAKE_VERBOSE_MAKEFILE ON)
target_link_libraries(atime apapi -lrt -pthread -ldl )
add_dependencies(atime apapi)

install(TARGETS atime RUNTIME DESTINATION bin)
