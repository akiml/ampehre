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
# created: 1/26/14
# version: 0.3.0 - extend libmeasure and add application for online monitoring
#          0.5.1 - modularized libmeasure
#

set(DATA_SOURCES
	CDataHandler.cpp
	CDataSettings.cpp
	CDataLibrary.cpp
	CDataMeasurement.cpp
	CDataTimer.cpp
)

add_library(msmonitor_data SHARED ${DATA_SOURCES})
target_link_libraries(msmonitor_data msmonitor_common ushmsmonitor_server)
target_link_libraries(msmonitor_data ${MS_LIBRARY})

install(TARGETS msmonitor_data LIBRARY DESTINATION lib)
