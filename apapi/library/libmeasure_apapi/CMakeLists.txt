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
include_directories(${APAPI_SOURCE_PATH})
get_target_property(PAPI_SOURCE_PATH apapi PAPI_SOURCE_PATH)
include_directories(${PAPI_SOURCE_PATH})

set(SRC_LIBMEASURE_APAPI_LIBRARY
	ms_common_apapi.c
	list.cpp
	ms_cpu_intel_xeon_sandy.c
	ms_fpga_maxeler_max3a.c
	ms_gpu_nvidia_tesla_kepler.c
	ms_mic_intel_knc.c
	ms_sys_dell_idrac7.c
)

# Full library
add_library(ms_common_apapi SHARED ${SRC_LIBMEASURE_APAPI_LIBRARY})
target_link_libraries(ms_common_apapi ${CMAKE_THREAD_LIBS_INIT} apapi)

install(TARGETS ms_common_apapi LIBRARY DESTINATION lib)
