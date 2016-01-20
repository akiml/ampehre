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
#          0.5.1 - modularized libmeasure
#          0.6.1 - add json printer to hettime
#

add_executable(hettime
	hettime.c measure.c common.c ostream_printer.c csv_printer.c json_printer.c
)
target_link_libraries(hettime ushmsmonitor_client cjson -lrt -pthread)
#This line can be used to link hettime for profiling with gperftools.
#You have to link against lprofiler and all the plugins which are loaded with dlopen to resolve the symbols in the profiling output.
#Here is an example how to start hettime with profiling: CPUPROFILE=hettime.prof CPUPROFILE_FREQUENCY=1000 hettime -e sleep -a 10
#target_link_libraries(hettime ushmsmonitor_client -lrt -pthread ms_mic_intel_knc ms_gpu_nvidia_tesla_kepler ms_fpga_maxeler_max3a ms_cpu_intel_xeon_sandy ms_stub ms_sys_dell_idrac7 -lprofiler)
target_link_libraries(hettime ${MS_LIBRARY})

install(TARGETS hettime RUNTIME DESTINATION bin)
