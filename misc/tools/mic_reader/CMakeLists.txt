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
# created: 3/18/15
# version: 0.3.4 - add a tool for first intergration tests of mic
#

add_executable(mic_reader
	mic_reader.c mic.c
)
target_link_libraries(mic_reader -lscif -lmicmgmt)

install(TARGETS mic_reader RUNTIME DESTINATION bin)
