#-------------------------------------------------
#
# Project created by QtCreator 2017-01-11T13:54:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR		= build
OBJECTS_DIR	= build/obj
MOC_DIR		= build/moc
RCC_DIR		= build/rcc
UI_DIR		= build/ui

TARGET			= msmonitor_client
TARGET.files	= $$DESTDIR/msmonitor_client
TARGET.path		= $$PREFIX/bin/

INSTALLS		+= TARGET

TEMPLATE	= app

INCLUDEPATH	+= $$PWD/include/ \
		$$PWD/../common/ \
		/usr/local/qwt-6.1.3/include/ \
		/usr/include/qwt/

SOURCES	+= src/main.cpp \
		src/CClient.cpp \
		src/CProtocolC.cpp \
		src/CConfig.cpp \
		src/gui/mainwindow.cpp \
		src/gui/QMSMpowerplot.cpp \
		src/gui/QMSMplot.cpp \
		src/gui/QMSMtemperatureplot.cpp \
		src/gui/QMSMclockplot.cpp \
		src/gui/QMSMmemoryplot.cpp \
		src/gui/QMSMutilplot.cpp \
		src/gui/QMSMheatmap.cpp \
		src/gui/QMSMsettings.cpp \
		src/gui/QMSMsystemoverview.cpp \
		../common/CProtocol.cpp \
		../common/utils.cpp \
                ../common/CComAbstract.cpp \
                ../common/CComClient.cpp \
                src/gui/QMSMlinearcolormaprgb.cpp \
                src/gui/QMSMabout.cpp

HEADERS	+= include/gui/mainwindow.h \
		include/gui/QMSMpowerplot.h \
		include/gui/QMSMplot.h \
		include/CClient.hpp \
		include/CProtocolC.hpp \
		include/CConfig.h \
		include/gui/QMSMtemperatureplot.h \
		include/gui/QMSMclockplot.h \
		include/gui/QMSMutilplot.h \
		include/gui/QMSMmemoryplot.h \
		include/gui/QMSMheatmap.h \
		include/gui/QMSMsettings.h \
		include/gui/QMSMsystemoverview.h \
		../common/CProtocol.hpp \
		../common/utils.h \
                ../common/CComClient.h \
                ../common/CComAbstract.h \
                include/gui/QMSMlinearcolormaprgb.h \
                include/gui/QMSMabout.h

LIBS	+= -L/usr/local/qwt-6.1.3/lib -lqwt

FORMS	+= mainwindow.ui \
		qmsmsettings.ui \
		qmsmplot.ui \
		qmsmsystemoverview.ui \
                qmsmabout.ui

OTHER_FILES += \
                include/default.conf

RESOURCES += \
    resource/rsrc.qrc
