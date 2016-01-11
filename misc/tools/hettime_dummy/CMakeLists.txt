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
# created: 10/13/14
# version: 0.1.17 - add a hettime dummy executable for some hetsched measurements
#          0.5.1 - modularized libmeasure
#          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
#                  and to select between the full or light library.
#

add_executable(hettime_dummy
	hettime.c measure.c printer.c
)
target_link_libraries(hettime_dummy -lrt -pthread)
target_link_libraries(hettime_dummy ${MS_LIBRARY})

install(TARGETS hettime_dummy RUNTIME DESTINATION bin)
