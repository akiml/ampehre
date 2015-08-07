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

# Full library
add_library(twmeasure SHARED
	ms_taskwrapper.c ms_taskwrapper_measure.c
)
target_link_libraries(twmeasure ${MS_LIBRARY})

install(TARGETS twmeasure LIBRARY DESTINATION lib)

# Light library
add_library(twmeasurelight SHARED
	ms_taskwrapper.c ms_taskwrapper_measure.c
)
target_link_libraries(twmeasurelight ${MS_LIBRARY_LIGHT})

install(TARGETS twmeasurelight LIBRARY DESTINATION lib)
