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
# created: 1/15/14
# version: 0.3.0 - extend libmeasure and add application for online monitoring
#          0.3.2 - add a networking component to show influence of a task to measurements in GUI
#          0.5.0 - add cpu, gpu and mic memory information
#          0.7.3 - add heatmaps to msmonitor and the enum ipmi_timeout_setting in libmeasure
#

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_path(QWT_INCLUDE_DIR
	NAMES qwt_plot.h
	HINTS ${QT_INCLUDE_DIR}
	PATH_SUFFIXES qwt qwt-qt3 qwt-qt4 qwt-qt5
)

find_library(QWT_LIBRARY
	NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${QWT_INCLUDE_DIR})

set(MSMONITOR_OTHER
	CGuiBuilder.cpp
	CGuiMarker.cpp
	CHeatmapData.cpp
)

set(MSMONITOR_FORMS
	ui/mainwindow.ui
	ui/forminfo.ui
	ui/formsettings.ui
	ui/formmeasurement.ui
	ui/formheatmap.ui
)

set(MSMONITOR_RESOURCES
	res/forminfo.qrc
)

set(MSMONITOR_HEADERS
	QMSMMainWindow.hpp
	QMSMFormInfo.hpp
	QMSMFormSettings.hpp
	QMSMFormMeasurementAbstract.hpp
	QMSMFormPower.hpp
	QMSMFormTemperature.hpp
	QMSMFormClock.hpp
	QMSMFormUtilization.hpp
	QMSMFormMemory.hpp
	QMSMFormHeatmapAbstract.hpp
	QMSMFormHeatmapUtilization.hpp
	QMSMFormHeatmapTemperature.hpp
	CHeatmap.hpp
)

set(MSMONITOR_SOURCES
	QMSMMainWindow.cpp
	QMSMFormInfo.cpp
	QMSMFormSettings.cpp
	QMSMFormMeasurementAbstract.cpp
	QMSMFormPower.cpp
	QMSMFormTemperature.cpp
	QMSMFormClock.cpp
	QMSMFormUtilization.cpp
	QMSMFormMemory.cpp
	QMSMFormHeatmapAbstract.cpp
	QMSMFormHeatmapUtilization.cpp
	QMSMFormHeatmapTemperature.cpp
	CHeatmap.cpp
)

qt4_wrap_ui(MSMONITOR_FORMS_HEADERS ${MSMONITOR_FORMS})
qt4_add_resources(MSMONITOR_RESOURCES_RCC ${MSMONITOR_RESOURCES})
qt4_wrap_cpp(MSMONITOR_HEADERS_MOC ${MSMONITOR_HEADERS})

add_library(msmonitor_gui SHARED ${MSMONITOR_OTHER} ${MSMONITOR_SOURCES} ${MSMONITOR_HEADERS_MOC} ${MSMONITOR_FORMS_HEADERS} ${MSMONITOR_RESOURCES_RCC})
target_link_libraries(msmonitor_gui ${QT_LIBRARIES} ${QWT_LIBRARY} msmonitor_data)

install(TARGETS msmonitor_gui LIBRARY DESTINATION lib)
