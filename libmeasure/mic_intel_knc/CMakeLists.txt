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
# version: 0.5.1 - modularised libmeasure
#          0.5.4 - add dynamic loading of resource specific libraries
#

set(SRC_MIC_LIBRARY
	ms_plugin_interface.cpp CMeasureMIC.cpp CMeasureMICThread.cpp
)

# Full library
add_library(ms_mic_intel_knc SHARED ${SRC_MIC_LIBRARY})
target_link_libraries(ms_mic_intel_knc ms_common -lmicmgmt -lscif)

install(TARGETS ms_mic_intel_knc LIBRARY DESTINATION lib)

# Light library
add_library(ms_mic_intel_knc_light SHARED ${SRC_MIC_LIBRARY})
target_link_libraries(ms_mic_intel_knc_light ms_common_light -lmicmgmt -lscif)
set_target_properties(ms_mic_intel_knc_light PROPERTIES COMPILE_DEFINITIONS "LIGHT")

install(TARGETS ms_mic_intel_knc_light LIBRARY DESTINATION lib)