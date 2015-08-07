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
# created: 7/28/14
# version: 0.1.0 - initial cmake file
#          0.1.10 - add child execution time measurement
#          0.1.11 - add a seperate csv printer file to hettime tool
#          0.3.2 - add a networking component to show influence of a task to measurements in GUI
#          0.5.1 - modularised libmeasure
#

add_executable(hettime
	hettime.c measure.c common.c ostream_printer.c csv_printer.c
)
target_link_libraries(hettime ushmsmonitor_client -lrt -pthread)
target_link_libraries(hettime ${MS_LIBRARY})

install(TARGETS hettime RUNTIME DESTINATION bin)
