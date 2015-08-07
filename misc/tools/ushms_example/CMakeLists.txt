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
# created: 3/05/15
# version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
#

add_executable(ushms_example
	main.c
)
target_link_libraries(ushms_example ushmsmonitor_server ushmsmonitor_client)

install(TARGETS ushms_example RUNTIME DESTINATION bin)
