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
#          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
#

set(SRC_SYS_LIBRARY
	ms_plugin_interface.cpp CMeasureIPMIThread.cpp
)

# Full library
add_library(ms_sys_dell_idrac7 SHARED ${SRC_SYS_LIBRARY})
target_link_libraries(ms_sys_dell_idrac7 ms_common ipmimeasure)

install(TARGETS ms_sys_dell_idrac7 LIBRARY DESTINATION lib)

# Light library
add_library(ms_sys_dell_idrac7_light SHARED ${SRC_SYS_LIBRARY})
target_link_libraries(ms_sys_dell_idrac7_light ms_common ipmimeasure)
set_target_properties(ms_sys_dell_idrac7_light PROPERTIES COMPILE_DEFINITIONS "LIGHT")

install(TARGETS ms_sys_dell_idrac7_light LIBRARY DESTINATION lib)