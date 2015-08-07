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
# created: 8/05/14
# version: 0.1.0 - initial cmake file
#          0.5.1 - modularised libmeasure
#

add_executable(example_ms
	example_ms.c
)
target_link_libraries(example_ms ${MS_LIBRARY})

install(TARGETS example_ms RUNTIME DESTINATION bin)
