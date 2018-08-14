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
# author: Christoph Knorr (cknorr@mail.upb.de)
# created: 5/12/15
# version: 0.5.1 - modularized libmeasure
#          0.5.4 - add dynamic loading of resource specific libraries
#          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
#                  and to select between the full or light library.
#          0.7.0 - modularized measurement struct
#

set(SRC_CPU_LIBRARY
	ms_plugin_interface.cpp
)

# Full library
add_library(ms_cpu_intel_xeon_sandy SHARED ${SRC_CPU_LIBRARY})

if(NOT CPUPOWER_LIBRARY)
	message(FATAL_ERROR "Cpupower/Cpufreq library not found. Check find_library in root CMakeLists.txt")
endif()

target_link_libraries(ms_cpu_intel_xeon_sandy ms_common ${CPUPOWER_LIBRARY})

install(TARGETS ms_cpu_intel_xeon_sandy LIBRARY DESTINATION lib)
install(FILES ms_cpu_intel_xeon_sandy.h DESTINATION include)
