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
#

set(COMMON_SOURCES CMutex.cpp CThread.cpp CAbstractTimer.cpp)

add_library(msmonitor_common SHARED ${COMMON_SOURCES})

install(TARGETS msmonitor_common LIBRARY DESTINATION lib)
