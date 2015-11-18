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
#          0.5.3 - add abstract measure and abstract measure thread
#          0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
#          0.5.12 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
#                   and to select between the full or light library.
#

find_package(Threads)

option(SIGNALS "Signals are raised if the measuring system is successfully initialized or stopped." ON)
if (SIGNALS)
	message(STATUS "${Yellow}INFO: Raising SIGUSR1 and SIGUSR2 is ENABLED.${ColourReset}")
	add_definitions(-DSIGNALS)
else (SIGNALS)
	message(STATUS "${Yellow}INFO: Raising SIGUSR1 and SIGUSR2 is DISABLED.${ColourReset}")
endif (SIGNALS)

set(SRC_COMMON_LIBRARY
	interface.cpp mgmt.cpp CThread.cpp CMutex.cpp CLogger.cpp CMgmt.cpp CSemaphore.cpp CMeasureThreadTimer.cpp CMeasureAbstractResource.cpp CMeasureAbstractThread.cpp CResourceLibraryHandler.cpp
)

# Full library
add_library(ms_common SHARED ${SRC_COMMON_LIBRARY})
target_link_libraries(ms_common ${CMAKE_THREAD_LIBS_INIT} -lrt -lm -ldl)

install(TARGETS ms_common LIBRARY DESTINATION lib)