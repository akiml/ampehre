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
#          0.1.12 - use more C++ features in library
#          0.1.15 - make CPU frequency settable
#          0.2.1 - add support for IPMI to the measure driver
#          0.2.2 - add semaphore to synchronize the start of the measurements
#          0.4.0 - MIC integration into libmeasure
#          0.5.1 - modularised libmeasure
#          0.5.4 - add dynamic loading of resource specific libraries
#          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
#



if(DELL_IDRAC7_SUPPORT)
	add_definitions(-DSYS_LIB)
	add_subdirectory(sys_dell_idrac7)
endif(DELL_IDRAC7_SUPPORT)

if(MIC_INTEL_KNC_SUPPORT)
	add_definitions(-DMIC_LIB)
	add_subdirectory(mic_intel_knc)
endif(MIC_INTEL_KNC_SUPPORT)

if(GPU_NVIDIA_TESLA_SUPPORT)
	add_definitions(-DGPU_LIB)
	add_subdirectory(gpu_nvidia_tesla_kepler)
endif(GPU_NVIDIA_TESLA_SUPPORT)

if(FPGA_MAXELER_MAX3A_SUPPORT)
	add_definitions(-DFPGA_LIB)
	add_subdirectory(fpga_maxeler_max3a)
endif(FPGA_MAXELER_MAX3A_SUPPORT)

if(CPU_INTEL_SANDY_SUPPORT)
	add_definitions(-DCPU_LIB)
	add_subdirectory(cpu_intel_xeon_sandy)
endif(CPU_INTEL_SANDY_SUPPORT)

add_subdirectory(common)
add_subdirectory(stub)