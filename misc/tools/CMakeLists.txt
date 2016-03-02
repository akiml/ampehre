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
#          0.1.6 - add an measuring system driver for cpu stat readings
#          0.1.14 - write a cpufreq test app
#          0.1.16 - add wrapper for heterogenous Node and test tool
#          0.1.17 - add a hettime dummy executable for some hetsched measurements
#          0.1.18 - hettime dummy tool can be used similar to ordinary "sleep" command
#          0.1.19 - add a hettime based idle power measurement tool
#          0.2.3 - add gpu_management tool and use the tool in the libmeasure
#          0.3.0 - extend libmeasure and add application for online monitoring
#          0.3.2 - add a networking component to show influence of a task to measurements in GUI
#          0.3.4 - add a tool for first intergration tests of mic
#          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
#

add_subdirectory(example_ms)
add_subdirectory(example_ms_odroid)
add_subdirectory(example_ms_hetsched)
add_subdirectory(driver_tester)
add_subdirectory(example_cpufreq)
add_subdirectory(hetnodetester)
add_subdirectory(hettime_dummy)
add_subdirectory(hetpowidle)
add_subdirectory(ush_example)
add_subdirectory(ushms_example)
if(MIC_INTEL_KNC_SUPPORT)
	add_subdirectory(mic_reader)
endif(MIC_INTEL_KNC_SUPPORT)
if(GPU_NVIDIA_TESLA_SUPPORT)
	add_subdirectory(gpu_management)
endif(GPU_NVIDIA_TESLA_SUPPORT)
