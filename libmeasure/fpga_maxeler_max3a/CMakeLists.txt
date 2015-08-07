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

find_library(MAX_LIBRARY libmaxeleros.so PATHS /opt/maxeler/maxeleros/lib)
include_directories(/upb/departments/agce/modules/software/maxeler/compiler/Maxcompiler-2013.3_vectis/lib/maxeleros-sim/include)

set(SRC_FPGA_LIBRARY
	ms_plugin_interface.cpp CMeasureMaxeler.cpp CMeasureMaxelerThread.cpp
)

# Full library
add_library(ms_fpga_maxeler_max3a SHARED ${SRC_FPGA_LIBRARY})
target_link_libraries(ms_fpga_maxeler_max3a ms_common cjson ${MAX_LIBRARY})

install(TARGETS ms_fpga_maxeler_max3a LIBRARY DESTINATION lib)

# Light library
add_library(ms_fpga_maxeler_max3a_light SHARED ${SRC_FPGA_LIBRARY})
target_link_libraries(ms_fpga_maxeler_max3a_light ms_common_light cjson ${MAX_LIBRARY})
set_target_properties(ms_fpga_maxeler_max3a_light PROPERTIES COMPILE_DEFINITIONS "LIGHT")

install(TARGETS ms_fpga_maxeler_max3a_light LIBRARY DESTINATION lib)
