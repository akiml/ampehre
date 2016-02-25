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
# created: 3/03/14
# version: 0.1.0 - initial cmake file
#          0.1.16 - add wrapper for heterogenous Node and test tool
#          0.2.1 - add support for IPMI to the measure driver
#          0.2.4 - add version check functionality to library, wrappers, and tools
#          0.3.2 - add a networking component to show influence of a task to measurements in GUI
#          0.5.1 - modularized libmeasure
#          0.5.4 - add dynamic loading of resource specific libraries
#          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
#          0.5.8 - change project name to ampehre
#          0.7.1 - move functions to query measurement results into the modules
#

cmake_minimum_required (VERSION 2.6)

project(ampehre)

# Please set support flags to ON if the corresponding device should be enabled for measuring.
option(DELL_IDRAC7_SUPPORT "build library with dell idrac7 support" OFF)
option(MIC_INTEL_KNC_SUPPORT "build library with mic intel knc support" OFF)
option(GPU_NVIDIA_TESLA_SUPPORT "build library with gpu nvidia tesla support" OFF)
option(FPGA_MAXELER_MAX3A_SUPPORT "build library with fpga maxeler max3a support" OFF)
option(CPU_INTEL_SANDY_SUPPORT "build library with cpu intel xeon sandy support" OFF)
option(ODROID_HARDKERNEL_XU4_SUPPORT "build library with odroid hardkernel xu4 support" ON)

set(CMAKE_C_FLAGS "-O3 -g -Wall -std=gnu99")
set(CMAKE_CXX_FLAGS "-O3 -g -Wall -std=gnu++98")
#set(CMAKE_C_FLAGS "-O3 -Wall -DDEBUG")
#set(CMAKE_CXX_FLAGS "-O3 -Wall -DDEBUG")

if(NOT WIN32)
	string(ASCII 27 Esc)
	set(ColourReset "${Esc}[m")
	set(ColourBold  "${Esc}[1m")
	set(Red         "${Esc}[31m")
	set(Green       "${Esc}[32m")
	set(Yellow      "${Esc}[33m")
	set(Blue        "${Esc}[34m")
	set(Magenta     "${Esc}[35m")
	set(Cyan        "${Esc}[36m")
	set(White       "${Esc}[37m")
	set(BoldRed     "${Esc}[1;31m")
	set(BoldGreen   "${Esc}[1;32m")
	set(BoldYellow  "${Esc}[1;33m")
	set(BoldBlue    "${Esc}[1;34m")
	set(BoldMagenta "${Esc}[1;35m")
	set(BoldCyan    "${Esc}[1;36m")
	set(BoldWhite   "${Esc}[1;37m")
endif(NOT WIN32)

set(MS_LIBRARY
	ms_common
)

set(MS_LIBRARY_LIGHT
	ms_common_light
)

set(MS_LIBRARY_MODULE_PATH
	libmeasure/cpu_intel_xeon_sandy
	libmeasure/gpu_nvidia_tesla_kepler
	libmeasure/fpga_maxeler_max3a
	libmeasure/mic_intel_knc
	libmeasure/sys_dell_idrac7
	libmeasure/odroid_hardkernel_xu4
)

option(DEBUG_SYMS "Compile library and applications with debug symbols." OFF)
if (DEBUG_SYMS)
	message(STATUS "${Yellow}INFO: Compile with debug symbols ENABLED.${ColourReset}")
	add_definitions(-g)
else (DEBUG_SYMS)
	message(STATUS "${Yellow}INFO: Compile with debug symbols DISABLED.${ColourReset}")
endif (DEBUG_SYMS)

if(DELL_IDRAC7_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with ipmi dell    idrac7       ENABLED.${ColourReset}")
else(DELL_IDRAC7_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with ipmi dell    idrac7       DISABLED.${ColourReset}")
endif(DELL_IDRAC7_SUPPORT)

if(MIC_INTEL_KNC_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with mic  intel   phi    knc   ENABLED.${ColourReset}")
else(MIC_INTEL_KNC_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with mic  intel   phi    knc   DISABLED.${ColourReset}")
endif(MIC_INTEL_KNC_SUPPORT)

if(GPU_NVIDIA_TESLA_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with gpu  nvidia  tesla        ENABLED.${ColourReset}")
else(GPU_NVIDIA_TESLA_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with gpu  nvidia  tesla        DISABLED.${ColourReset}")
endif(GPU_NVIDIA_TESLA_SUPPORT)

if(FPGA_MAXELER_MAX3A_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with fpga maxeler max3a        ENABLED.${ColourReset}")
else(FPGA_MAXELER_MAX3A_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with fpga maxeler max3a        DISABLED.${ColourReset}")
endif(FPGA_MAXELER_MAX3A_SUPPORT)

if(CPU_INTEL_SANDY_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with cpu  intel   xeon   sandy ENABLED.${ColourReset}")
else(CPU_INTEL_SANDY_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with cpu  intel   xeon   sandy DISABLED.${ColourReset}")
endif(CPU_INTEL_SANDY_SUPPORT)

if(ODROID_HARDKERNEL_XU4_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with odroid hardkernel   xu4   ENABLED.${ColourReset}")
else(ODROID_HARDKERNEL_XU4_SUPPORT)
	message(STATUS "${BoldYellow}INFO: Compile with odroid hardkernel   xu4   DISABLED.${ColourReset}")
endif(ODROID_HARDKERNEL_XU4_SUPPORT)

include_directories(${MS_LIBRARY_MODULE_PATH} include/)

add_subdirectory(cjson)
#add_subdirectory(driver)
add_subdirectory(libmeasure)
add_subdirectory(utilities)
add_subdirectory(misc)

install(FILES include/unixsockethandler.h DESTINATION include)
install(FILES include/ushmsmonitor.h DESTINATION include)
install(FILES include/ms_measurement.h DESTINATION include)
install(FILES include/ms_taskwrapper.h DESTINATION include)
install(FILES include/ms_hetschedwrapper.h DESTINATION include)
install(FILES include/ms_hetschedwrapper_client.h DESTINATION include)
install(FILES include/ms_driver.h DESTINATION include)
install(FILES include/ms_hetnodewrapper.h DESTINATION include)
install(FILES include/ms_ipmiwrapper.h DESTINATION include)
install(FILES include/ms_version.h DESTINATION include)
install(FILES include/ms_plugin_interface.h DESTINATION include)
install(FILES include/ms_list.h DESTINATION include)
