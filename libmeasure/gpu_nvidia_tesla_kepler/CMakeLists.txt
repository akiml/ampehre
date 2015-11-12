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
# version: 0.5.1 - modularised libmeasure
#          0.5.4 - add dynamic loading of resource specific libraries
#          0.5.12 - add ioctl call to configure the ipmi timeout and possibility to skip every i-th measurement point
#

find_library(NVML_LIBRARY libnvidia-ml.so PATHS /usr/lib64)

set(SRC_GPU_LIBRARY
	ms_plugin_interface.cpp CMeasureNVMLThread.cpp
)

# Full library
add_library(ms_gpu_nvidia_tesla_kepler SHARED ${SRC_GPU_LIBRARY})
target_link_libraries(ms_gpu_nvidia_tesla_kepler ms_common ${NVML_LIBRARY})

install(TARGETS ms_gpu_nvidia_tesla_kepler LIBRARY DESTINATION lib)

# Light library
add_library(ms_gpu_nvidia_tesla_kepler_light SHARED ${SRC_GPU_LIBRARY})
target_link_libraries(ms_gpu_nvidia_tesla_kepler_light ms_common_light ${NVML_LIBRARY})
set_target_properties(ms_gpu_nvidia_tesla_kepler_light PROPERTIES COMPILE_DEFINITIONS "LIGHT")

install(TARGETS ms_gpu_nvidia_tesla_kepler_light LIBRARY DESTINATION lib)