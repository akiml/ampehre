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
# created: 8/27/14
# version: 0.1.6 - add an measuring system driver for cpu stat readings
#          0.2.0 - add support for msr registers to the measure driver
#          0.2.1 - add support for IPMI to the measure driver
#

add_executable(driver_tester
	driver_tester.c
)

target_link_libraries(driver_tester ipmimeasure rt)

install(TARGETS driver_tester RUNTIME DESTINATION bin)
