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
# created: 7/16/15
# version: 0.5.8 - change project name to ampehre
#

set(CJSON_SOURCES cJSON.c)

add_library(cjson SHARED ${CJSON_SOURCES})

install(TARGETS cjson LIBRARY DESTINATION lib)
install(FILES cJSON.h DESTINATION include)
