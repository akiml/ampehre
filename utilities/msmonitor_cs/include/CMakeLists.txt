#
# CMakeLists.txt
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Ahmad El-Ali (aelali@mail.upb.de)
# created: 10/12/16
# version: 0.8.0 - extend libmeasure and add application for online monitoring
#

set(DATA_SOURCES
	CProtocol.cpp
)

add_library(msmonitor_protocol SHARED ${DATA_SOURCES})
target_link_libraries(msmonitor_protocol ${MS_LIBRARY})

install(TARGETS msmonitor_protocol LIBRARY DESTINATION lib)
