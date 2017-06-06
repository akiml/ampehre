#
# CMakeLists.txt
#
# Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 7/02/17
# version: 0.7.5 - add a python wrapper for simple measuring by python scripts
#

add_library(pythonmeasure SHARED
	ms_pythonwrapper.c
)
target_link_libraries(pythonmeasure ${MS_LIBRARY})

install(TARGETS pythonmeasure LIBRARY DESTINATION lib)
install(FILES ampehre.py DESTINATION share)
