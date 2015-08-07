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
# created: 6/01/15
# version: 0.5.4 - add dynamic loading of resource specific libraries
#

set(SRC_CPU_LIBRARY
	ms_plugin_interface.cpp CMeasureStub.cpp CMeasureStubThread.cpp
)

# Full library
add_library(ms_stub SHARED ${SRC_CPU_LIBRARY})
target_link_libraries(ms_stub ms_common)

install(TARGETS ms_stub LIBRARY DESTINATION lib)