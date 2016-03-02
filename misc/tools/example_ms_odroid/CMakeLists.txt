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
# created: 3/02/16
# version: 0.7.4 - add support for Odroid XU4 systems
#

add_executable(example_ms_odroid
	example_ms_odroid.c
)
target_link_libraries(example_ms_odroid ${MS_LIBRARY})

install(TARGETS example_ms_odroid RUNTIME DESTINATION bin)
