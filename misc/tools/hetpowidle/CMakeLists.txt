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
# created: 10/27/14
# version: 0.1.19 - add a hettime based idle power measurement tool
#          0.5.1 - modularized libmeasure
#          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
#                  and to select between the full or light library.
#

add_executable(hetpowidle
	hetpowidle.c measure.c list.c printer.c
)
target_link_libraries(hetpowidle cjson -lrt -pthread)
target_link_libraries(hetpowidle cjson ${MS_LIBRARY})

install(TARGETS hetpowidle RUNTIME DESTINATION bin)
