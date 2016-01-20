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
# created: 5/12/15
# version: 0.5.1 - modularized libmeasure
#          0.5.4 - add dynamic loading of resource specific libraries
#          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
#                  and to select between the full or light library.
#          0.7.0 - modularized measurement struct
#

find_library(NVML_LIBRARY libnvidia-ml.so PATHS /usr/lib64)

set(SRC_GPU_LIBRARY
	ms_plugin_interface.cpp
)

# Full library
add_library(ms_gpu_nvidia_tesla_kepler SHARED ${SRC_GPU_LIBRARY})
target_link_libraries(ms_gpu_nvidia_tesla_kepler ms_common ${NVML_LIBRARY})

install(TARGETS ms_gpu_nvidia_tesla_kepler LIBRARY DESTINATION lib)
install(FILES ms_gpu_nvidia_tesla_kepler.h DESTINATION include)
