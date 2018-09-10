#
# CMakeLists.txt
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 9/14/14
# version: 0.1.14 - write a cpufreq test app
#

add_executable(example_cpufreq
	example_cpufreq.c
)

if(NOT CPUPOWER_LIBRARY)
	message(FATAL_ERROR "Cpupower/Cpufreq library not found. Check find_library in root CMakeLists.txt")
endif()

target_link_libraries(example_cpufreq ${CPUPOWER_LIBRARY})

install(TARGETS example_cpufreq RUNTIME DESTINATION bin)
