#
# CMakeLists.txt
#
# Copyright (C) 2018, Alex Wiens <awiens@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Alex Wiens (awiens@mail.uni-padeborn.de)
# created: 7/06/18
# version: 0.8.0 - initial cmake file
#

add_executable(hetweb
	hetweb.cpp CHTTPServer.cpp CHTTPClient.cpp CHTTPRequest.cpp CWebMeasure.cpp
)
set_target_properties(hetweb PROPERTIES
COMPILE_FLAGS -std=c++11
)
target_link_libraries(hetweb)
target_link_libraries(hetweb ${MS_LIBRARY})

install(TARGETS hetweb RUNTIME DESTINATION bin)
