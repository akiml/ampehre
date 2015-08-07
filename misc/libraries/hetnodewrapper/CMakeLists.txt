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
# created: 9/02/14
# version: 0.1.16 - add wrapper for heterogenous Node and test tool
#          0.5.1 - modularised libmeasure
#

# Full library
add_library(hnmeasure SHARED
			ms_hetnodewrapper.c ms_hetnodewrapper_data.c)


target_link_libraries(hnmeasure ${MS_LIBRARY})

install(TARGETS hnmeasure LIBRARY DESTINATION lib)

