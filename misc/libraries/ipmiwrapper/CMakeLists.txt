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
# author: Christoph Knorr (cknorr@mail.upb.de)
#
# created: 10/29/14
# version: 0.2.1 - add support for IPMI to the measure driver
#

# Full library
add_library(ipmimeasure SHARED
			ms_ipmiwrapper.c)
			
target_link_libraries(ipmimeasure m)

install(TARGETS ipmimeasure LIBRARY DESTINATION lib)
