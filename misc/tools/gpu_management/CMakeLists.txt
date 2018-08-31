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
# created: 11/19/14
# version: 0.2.3 - add gpu_management tool and use the tool in the libmeasure
#
#

include_directories(/usr/local/cuda-9.2/targets/x86_64-linux/include/)

add_executable(gpu_management
	gpu_mgmt.c
)

target_link_libraries(gpu_management ${NVML_LIBRARY})

install(TARGETS gpu_management RUNTIME DESTINATION bin)
