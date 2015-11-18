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
#          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
#                   and to select between the full or light library.
#

set(SRC_SYS_LIBRARY
	ms_plugin_interface.cpp
)

# Full library
add_library(ms_sys_dell_idrac7 SHARED ${SRC_SYS_LIBRARY})
target_link_libraries(ms_sys_dell_idrac7 ms_common ipmimeasure)

install(TARGETS ms_sys_dell_idrac7 LIBRARY DESTINATION lib)